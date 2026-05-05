#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "NetworkModule.hpp"
#include "esp_netif_ppp.h"
#include "esp_event.h"
#include "esp_netif_ppp.h"
#include "esp_modem_api.h"
#include <cstring>

#define TAG "GSM"

TaskHandle_t modemTaskkHandle = NULL;
const esp_netif_driver_ifconfig_t *ppp_driver_cfg = NULL;
static EventGroupHandle_t event_group = NULL;
bool eventsRegisteredAlready = false;
uint8_t modem_reset_count = 0;
bool ImeiRead = false;

#define SOF_MARKER 0xF9
#define PF 0x10      /* Poll / Final */
#define FT_DISC 0x43 /* Disconnect */

namespace NetworkModule
{

    uint8_t fcs_crc(const uint8_t frame[6])
    {
        //    #define FCS_GOOD_VALUE 0xCF
        uint8_t crc = 0xFF; // FCS_INIT_VALUE

        for (int i = 1; i < 4; i++)
        {
            crc ^= frame[i];

            for (int j = 0; j < 8; j++)
            {
                if (crc & 0x01)
                {
                    crc = (crc >> 1) ^ 0xe0; // FCS_POLYNOMIAL
                }
                else
                {
                    crc >>= 1;
                }
            }
        }
        return crc;
    }

    void send_disconnect(size_t i)
    {
        if (i == 0)
        { // control terminal
            uint8_t frame[] = {
                SOF_MARKER, 0x3, 0xEF, 0x5, 0xC3, 0x1, 0xF2, SOF_MARKER};
            uart_write_bytes(UART_NUM_1, (const char *)frame, sizeof(frame));
        }
        else
        { // separate virtual terminal
            uint8_t frame[] = {
                SOF_MARKER, 0x3, FT_DISC | PF, 0x1, 0, SOF_MARKER};
            frame[1] |= i << 2;
            frame[4] = 0xFF - fcs_crc(frame);
            uart_write_bytes(UART_NUM_1, (const char *)frame, sizeof(frame));
        }
    }

    void send_cmux_disc_frame()
    {
        ESP_LOGI(TAG, "Sending DISC frame to close CMUX connection");
        send_disconnect(0);
        ESP_LOGI(TAG, "Sending DISC frame to close CMUX connection");
        send_disconnect(1);
        vTaskDelay(pdMS_TO_TICKS(100));
    }

    void readImei(esp_modem_dce_t *dce)
    {
        esp_err_t ret = esp_modem_get_imei(dce, network->gsm_imei);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_modem_get_imei failed with %d %s", ret, esp_err_to_name(ret));
        }
        else
        {
            memset(config->simIMEINumber, 0, sizeof(config->simIMEINumber));
            memcpy(config->simIMEINumber, network->gsm_imei, strlen(network->gsm_imei));
            config->WriteConfigurationToFlash();
            ImeiRead = true;
            ESP_LOGI(TAG, "IMEI: %s", network->gsm_imei);
        }
    }

    void modemTask(void *args)
    {
        uint32_t websocketOffCount = 0;
        int backoff_time = 5000;
        bool gsmWeakSignal_old;
        esp_modem_dce_config_t dce_config = ESP_MODEM_DCE_DEFAULT_CONFIG((char *)config->gsmAPN);
        esp_modem_dte_config_t dte_config = ESP_MODEM_DTE_DEFAULT_CONFIG();
        dte_config.dte_buffer_size = 4096;
        dte_config.task_priority = 24;
        dte_config.uart_config.tx_buffer_size = 1024;
        dte_config.uart_config.tx_io_num = MODEM_TX_PIN;
        dte_config.uart_config.rx_io_num = MODEM_RX_PIN;

        esp_modem_dce_t *dce = esp_modem_new_dev(ESP_MODEM_DCE_SIM7600, &dte_config, &dce_config, network->netif[1]);

        esp_err_t ret = esp_modem_get_signal_quality(dce, &network->gsm_rssi, &network->gsm_ber);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_modem_get_signal_quality failed with %d %s", ret, esp_err_to_name(ret));
        }
        ESP_LOGI(TAG, "Signal quality: rssi=%d, ber=%d", network->gsm_rssi, network->gsm_ber);
        ret = esp_modem_get_imei(dce, network->gsm_imei);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_modem_get_imei failed with %d %s", ret, esp_err_to_name(ret));
        }
        else
        {
            memset(config->simIMEINumber, 0, sizeof(config->simIMEINumber));
            memcpy(config->simIMEINumber, network->gsm_imei, strlen(network->gsm_imei));
            config->WriteConfigurationToFlash();
        }
        ESP_LOGI(TAG, "IMEI: %s", network->gsm_imei);
        ret = esp_modem_get_imsi(dce, network->gsm_imsi);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_modem_get_imsi failed with %d %s", ret, esp_err_to_name(ret));
            // goto failed;
        }
        else
        {
            memset(config->simIMSINumber, 0, sizeof(config->simIMSINumber));
            memcpy(config->simIMSINumber, network->gsm_imsi, strlen(network->gsm_imsi));
            config->WriteConfigurationToFlash();
        }
        ESP_LOGI(TAG, "IMSI: %s", network->gsm_imsi);
        ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_CMUX);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_CMUX) failed with %d", ret);
            // esp_modem_destroy(dce);
            // esp_modem_destroy_dte(dte_config.uart_config.uart_port);
            send_cmux_disc_frame();
            // dce = esp_modem_new_dev(ESP_MODEM_DCE_SIM7600, &dte_config, &dce_config, network->netif[1]);
            // ppp_info->context = dce;
            vTaskDelay(pdMS_TO_TICKS(1000));
            ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_COMMAND) failed with %d", ret);
                // esp_restart();
            }
            ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_CMUX);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_CMUX) failed with %d", ret);
                network->modemDiscoonectedFromPPPServer = true;
            }
            else
            {
                ESP_LOGW(TAG, "Modem Entered CMUX mode");
            }
        }
        else
        {
            ESP_LOGW(TAG, "Modem Entered CMUX mode");
        }

        // failed:

#define CONTINUE_LATER() \
    backoff_time = 5000; \
    continue;

        // now let's keep retrying
        while (1)
        {
            if (network->isWebsocketConnected == false)
            {
                websocketOffCount++;
                if (websocketOffCount > 15)
                {
                    websocketOffCount = 0;
                    network->modemDiscoonectedFromPPPServer = true;
                }
                int rssi_local, ber_local;
                ret = esp_modem_get_signal_quality(dce, &rssi_local, &ber_local);
                if (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "esp_modem_get_signal_quality failed with %d %s", ret, esp_err_to_name(ret));
                }
                else
                {
                    if (rssi_local < 90)
                    {
                        int SignalStrength = -113 + (rssi_local * 2);
                        ESP_LOGW(TAG, "Signal quality: rssi=%d strength=%d", rssi_local, SignalStrength);
                    }
                }
            }
            else
            {
                websocketOffCount = 0;
            }
            if (network->modemDiscoonectedFromPPPServer)
            {
                ESP_LOGE(TAG, "Resetting Modem:");
                esp_modem_reset(dce);
                modem_reset_count++;
                ESP_LOGE(TAG, "Reset Modem Completed");
                send_cmux_disc_frame();
                vTaskDelay(pdMS_TO_TICKS(1000));
                ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
                if (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_COMMAND) failed with %d", ret);
                }
                else
                {
                    ESP_LOGW(TAG, "Modem Entered COMMAND mode");
                }
                ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_CMUX);
                if (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_CMUX) failed with %d", ret);
                    send_cmux_disc_frame();

                    vTaskDelay(pdMS_TO_TICKS(1000));
                    ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_COMMAND);
                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_COMMAND) failed with %d", ret);
                        // esp_restart();
                    }
                    ret = esp_modem_set_mode(dce, ESP_MODEM_MODE_CMUX);
                    if (ret != ESP_OK)
                    {
                        ESP_LOGE(TAG, "esp_modem_set_mode(ESP_MODEM_MODE_CMUX) failed with %d", ret);
                        ESP_LOGE(TAG, "Resetting Modem:");
                        esp_modem_reset(dce);
                        modem_reset_count++;
                        ESP_LOGE(TAG, "Reset Modem Completed");
                    }
                    else
                    {
                        network->modemDiscoonectedFromPPPServer = false;
                        ESP_LOGW(TAG, "Modem Entered CMUX mode");
                    }
                }
                else
                {
                    network->modemDiscoonectedFromPPPServer = false;
                    ESP_LOGW(TAG, "Modem Entered CMUX mode");
                }
            }
            ESP_LOGI(TAG, "sleeping for %d ms", backoff_time);
            vTaskDelay(pdMS_TO_TICKS(backoff_time));
            if (network->isGsmConnected)
            {
                backoff_time = 5000;
                int rssi_local, ber_local;
                if (ImeiRead == false)
                    readImei(dce);
                ret = esp_modem_get_signal_quality(dce, &rssi_local, &ber_local);
                if (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "esp_modem_get_signal_quality failed with %d %s", ret, esp_err_to_name(ret));
                }
                else
                {
                    network->gsmSignalStrengthReceived = true;
                    if (rssi_local < 100)
                    {
                        int SignalStrength = -113 + (rssi_local * 2);
                        ESP_LOGW(TAG, "Signal quality: rssi=%d strength=%d", rssi_local, SignalStrength);
                    }
                    // ESP_LOGW(TAG, "Signal quality: rssi=%d, ber=%d", rssi_local, ber_local);
                    if (rssi_local <= 4)
                    {
                        if (network->gsmWeakSignal)
                        {
                            if (rssi_local < network->gsm_rssi)
                            {
                                gsmWeakSignal_old = false;
                            }
                        }
                        network->gsm_rssi = rssi_local;
                        network->gsmWeakSignal = true;
                    }
                    else
                    {
                        network->gsmWeakSignal = false;
                    }
                    if ((rssi_local > 0) && (rssi_local < 55))
                        network->gsmSignalStrength = -113 + (rssi_local * 2);
                }

                continue;
            }
        }
        vTaskDelete(NULL);
    }

    static void on_ppp_changed(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
    {
        ESP_LOGI(TAG, "PPP state changed event %" PRIu32, event_id);
        if (event_id == NETIF_PPP_ERRORUSER)
        {
            ESP_LOGI(TAG, "User interrupted event from netif: %p", network->netif[1]);
            network->isGsmConnected = false;
        }
    }

    static void on_ip_event(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
    {
        ESP_LOGD(TAG, "IP event! %" PRIu32, event_id);

        if (event_id == IP_EVENT_PPP_GOT_IP)
        {
            ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;

            ESP_LOGI(TAG, "Modem Connect to PPP Server");
            xEventGroupSetBits(event_group, 1);
            network->isGsmConnected = true;
            network->isGsmConnected = true;
            ESP_LOGI(TAG, "GOT ip event!!!");
        }
        else if (event_id == IP_EVENT_PPP_LOST_IP)
        {
            ESP_LOGI(TAG, "Modem Disconnect from PPP Server");
            network->isGsmConnected = false;
            network->modemDiscoonectedFromPPPServer = true;
            network->isGsmConnected = false;
        }
        else if (event_id == IP_EVENT_GOT_IP6)
        {
            ESP_LOGI(TAG, "GOT IPv6 event!");
            ip_event_got_ip6_t *event = (ip_event_got_ip6_t *)event_data;
            ESP_LOGI(TAG, "Got IPv6 address " IPV6STR, IPV62STR(event->ip6_info.ip));
        }
    }

    void modem_initialise(int priority)
    {
        event_group = xEventGroupCreate();

        if (eventsRegisteredAlready == false)
            esp_event_handler_register(IP_EVENT, ESP_EVENT_ANY_ID, &on_ip_event, NULL);
        if (eventsRegisteredAlready == false)
            esp_event_handler_register(NETIF_PPP_STATUS, ESP_EVENT_ANY_ID, &on_ppp_changed, NULL);

        esp_netif_inherent_config_t base_netif_cfg = ESP_NETIF_INHERENT_DEFAULT_PPP();
        base_netif_cfg.route_prio = priority;
        esp_netif_config_t netif_ppp_config = {.base = &base_netif_cfg,
                                               .driver = ppp_driver_cfg,
                                               .stack = ESP_NETIF_NETSTACK_DEFAULT_PPP};
        if (eventsRegisteredAlready == false)
            network->netif[1] = esp_netif_new(&netif_ppp_config);

        if (network->netif[1] == NULL)
        {
            return;
        }
        if (xTaskCreate(modemTask, "modemTask", 3072, NULL, 5, &modemTaskkHandle) != pdTRUE)
        {
            return;
        }

        ESP_LOGI(TAG, "Waiting for IP address");
        if (eventsRegisteredAlready == false)
            xEventGroupWaitBits(event_group, 1, pdFALSE, pdFALSE, pdMS_TO_TICKS(10000));
    }
} // namespace NetworkModule
