#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "freertos/semphr.h"
#include "NetworkModule.hpp"
#include <cJSON.h>
#include <cstring>

#define TAG "NETWORK"
#define TAG_WEBSOCKET "WEBSOCKET"
#define NO_DATA_TIMEOUT_SEC 5

NetworkModule::NetworkController *network;

namespace NetworkModule
{
    static TimerHandle_t shutdown_signal_timer;
    static SemaphoreHandle_t shutdown_sema;
    bool wifiConnectStarted = false;
    void SafeWifiConnect()
    {
        if (network->isWifiConnected == false && wifiConnectStarted == false)
        {
            esp_err_t ret = esp_wifi_connect();

            if (ret == ESP_OK)
            {
                wifiConnectStarted = true;
                ESP_LOGI(TAG, "WiFi connect started");
            }
            else
            {
                ESP_LOGW(TAG, "WiFi connect skipped: %s", esp_err_to_name(ret));
            }
        }
    }

    void updateHighPriorityNetwork(void)
    {
        bool connectingToEthernet = false;
        bool connectingToWifi = false;
        bool connectingToGsm = false;

        if (config->ethernetEnable)
        {
            if (config->wifiEnable && config->gsmEnable)
            {
                if ((config->ethernetPriority > config->gsmPriority) && (config->ethernetPriority > config->wifiPriority))
                {
                    connectingToEthernet = true;
                }
            }
            else if (config->wifiEnable)
            {
                if (config->ethernetPriority > config->wifiPriority)
                {
                    connectingToEthernet = true;
                }
            }
            else if (config->gsmEnable)
            {
                if (config->ethernetPriority > config->gsmPriority)
                {
                    connectingToEthernet = true;
                }
            }
            else
            {
                connectingToEthernet = true;
            }
        }
        if (config->wifiEnable)
        {
            if (config->gsmEnable && config->ethernetEnable)
            {
                if ((config->wifiPriority > config->gsmPriority) && (config->wifiPriority > config->ethernetPriority))
                {
                    connectingToWifi = true;
                }
            }
            else if (config->gsmEnable)
            {
                if (config->wifiPriority > config->gsmPriority)
                {
                    connectingToWifi = true;
                }
            }
            else if (config->ethernetEnable)
            {
                if (config->wifiPriority > config->ethernetPriority)
                {
                    connectingToWifi = true;
                }
            }
            else
            {
                connectingToWifi = true;
            }
        }
        if (config->gsmEnable)
        {
            if (config->ethernetEnable && config->wifiEnable)
            {
                if ((config->gsmPriority > config->ethernetPriority) && (config->gsmPriority > config->wifiPriority))
                {
                    connectingToGsm = true;
                }
            }
            else if (config->ethernetEnable)
            {
                if (config->gsmPriority > config->ethernetPriority)
                {
                    connectingToGsm = true;
                }
            }
            else if (config->wifiEnable)
            {
                if (config->gsmPriority > config->wifiPriority)
                {
                    connectingToGsm = true;
                }
            }
            else
            {
                connectingToGsm = true;
            }
        }

        if (connectingToWifi)
        {
            network->HighPriorityNetwork = 0;
        }
        else if (connectingToGsm)
        {
            network->HighPriorityNetwork = 1;
        }
        else if (connectingToEthernet)
        {
            network->HighPriorityNetwork = 2;
        }
        else
        {
            network->HighPriorityNetwork = 3;
        }
    }

    void log_error_if_nonzero(const char *message, int error_code)
    {
        if (error_code != 0)
        {
            ESP_LOGE(TAG_WEBSOCKET, "Last error %s: 0x%x", message, error_code);
        }
    }

    void shutdown_signaler(TimerHandle_t xTimer)
    {
        ESP_LOGD(TAG_WEBSOCKET, "No data received for %d seconds, signaling shutdown", NO_DATA_TIMEOUT_SEC);
        xSemaphoreGive(shutdown_sema);
    }

    void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
    {
        esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
        switch (event_id)
        {
        case WEBSOCKET_EVENT_BEGIN:
            ESP_LOGI(TAG_WEBSOCKET, "WEBSOCKET_EVENT_BEGIN");
            break;
        case WEBSOCKET_EVENT_CONNECTED:
            network->isWebsocketConnected = true;
            ESP_LOGI(TAG_WEBSOCKET, "WEBSOCKET_EVENT_CONNECTED");
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            network->isWebsocketConnected = false;
            network->client = NULL;
            ESP_LOGI(TAG_WEBSOCKET, "WEBSOCKET_EVENT_DISCONNECTED");
            log_error_if_nonzero("HTTP status code", data->error_handle.esp_ws_handshake_status_code);
            if (data->error_handle.error_type == WEBSOCKET_ERROR_TYPE_TCP_TRANSPORT)
            {
                log_error_if_nonzero("reported from esp-tls", data->error_handle.esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", data->error_handle.esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", data->error_handle.esp_transport_sock_errno);
            }
            break;
        case WEBSOCKET_EVENT_DATA:
        {
            ESP_LOGD(TAG_WEBSOCKET, "WEBSOCKET_EVENT_DATA");
            ESP_LOGD(TAG_WEBSOCKET, "Received opcode=%d", data->op_code);
            if (data->op_code == 0x2)
            { // Opcode 0x2 indicates binary data
                ESP_LOG_BUFFER_HEX(TAG_WEBSOCKET "Received binary data", data->data_ptr, data->data_len);
            }
            else if (data->op_code == 0x08 && data->data_len == 2)
            {
                ESP_LOGW(TAG_WEBSOCKET, "Received closed message with code=%d", 256 * data->data_ptr[0] + data->data_ptr[1]);
            }
            else if (data->op_code == 0x0A)
            {
                ESP_LOGI(TAG_WEBSOCKET, "PONG Received");
            }
            else
            {
                ESP_LOGI(TAG_WEBSOCKET, "Received=%.*s", data->data_len, (char *)data->data_ptr);
                std::string rxData((char *)data->data_ptr, data->data_len);
                network->ReceiveData(rxData);
            }

            xTimerReset(shutdown_signal_timer, portMAX_DELAY);
            break;
        }
        case WEBSOCKET_EVENT_ERROR:
            ESP_LOGI(TAG_WEBSOCKET, "WEBSOCKET_EVENT_ERROR");
            log_error_if_nonzero("HTTP status code", data->error_handle.esp_ws_handshake_status_code);
            if (data->error_handle.error_type == WEBSOCKET_ERROR_TYPE_TCP_TRANSPORT)
            {
                log_error_if_nonzero("reported from esp-tls", data->error_handle.esp_tls_last_esp_err);
                log_error_if_nonzero("reported from tls stack", data->error_handle.esp_tls_stack_err);
                log_error_if_nonzero("captured as transport's socket errno", data->error_handle.esp_transport_sock_errno);
            }
            break;
        case WEBSOCKET_EVENT_FINISH:
            ESP_LOGI(TAG_WEBSOCKET, "WEBSOCKET_EVENT_FINISH");
            break;
        }
    }

    void websocket_app_start(void)
    {
        esp_websocket_client_config_t websocket_cfg = {};

        shutdown_signal_timer = xTimerCreate("Websocket shutdown timer", NO_DATA_TIMEOUT_SEC * 1000 / portTICK_PERIOD_MS,
                                             pdFALSE, NULL, shutdown_signaler);
        shutdown_sema = xSemaphoreCreateBinary();

        websocket_cfg.uri = config->webSocketURL;
        websocket_cfg.task_prio = 6;
        websocket_cfg.task_stack = 3 * 1024;
        websocket_cfg.disable_auto_reconnect = false;
        websocket_cfg.transport = WEBSOCKET_TRANSPORT_OVER_TCP;
        websocket_cfg.keep_alive_enable = true;
        websocket_cfg.buffer_size = 2 * 1024;
        websocket_cfg.keep_alive_idle = 10;
        websocket_cfg.keep_alive_interval = 10;
        websocket_cfg.keep_alive_count = 3;
        websocket_cfg.subprotocol = "ocpp1.6";
        websocket_cfg.ping_interval_sec = network->WebSocketPingInterval;
        websocket_cfg.reconnect_timeout_ms = 10000;
        websocket_cfg.network_timeout_ms = 10000;
        websocket_cfg.pingpong_timeout_sec = 10;
        websocket_cfg.disable_pingpong_discon = false;
        ESP_LOGI(TAG_WEBSOCKET, "Connecting to %s...", websocket_cfg.uri);

        network->client = esp_websocket_client_init(&websocket_cfg);
        esp_websocket_register_events(network->client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)network->client);

        esp_websocket_client_start(network->client);
        xTimerStart(shutdown_signal_timer, portMAX_DELAY);
    }

    bool NetworkController::ReceiveData(std::string jsonData)
    {
        if (receiveFunc)
        {
            receiveFunc(jsonData);
        }
        return false;
    }

    bool NetworkController::SendData(std::string jsonData)
    {
        if (isWebsocketConnected)
        {
            if (esp_websocket_client_send_text(client, jsonData.c_str(), strlen(jsonData.c_str()), pdMS_TO_TICKS(5000) > 0))
                return true;
        }
        return false;
    }

    void NetworkController::networkTask(void *pvParameters)
    {
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay to ensure other modules are initialized
        bool wifiWeakSignal_old;

        uint8_t modem_reset_count = 0;
        uint8_t available_interfaces = 0;
        uint32_t WebsocketOffWithNetworkCount = 0;
        uint32_t WebsocketOffCount = 0;
        uint32_t EthernetInternetDisconnectTime = 0;
        uint32_t TaskStartedTime = 0;

        bool WifiConnectedButConnectivityLost = false;
        bool PPPConnectedButConnectivityLost = false;
        bool EthernetConnectedButConnectivityLost = false;
        esp_netif_init();
        esp_event_loop_create_default();

        if (config->wifiEnable)
        {
            available_interfaces++;
            network->isWifiConnected = false;
            wifi_initialise((int)config->wifiPriority);
        }
        if (config->gsmEnable)
        {
            available_interfaces++;
            network->isGsmConnected = false;
            modem_initialise((int)config->gsmPriority);
        }
        if (config->ethernetEnable)
        {
            available_interfaces++;
            network->isEthernetConnected = false;
            ethernet_initialise((int)config->ethernetPriority);
        }

        updateHighPriorityNetwork();
        if (network->HighPriorityNetwork == 0)
        {
            if (available_interfaces > 1)
                esp_netif_set_default_netif(network->netif[0]);
        }
        else if (network->HighPriorityNetwork == 1)
        {
            if (available_interfaces > 1)
                esp_netif_set_default_netif(network->netif[1]);
        }
        else if (network->HighPriorityNetwork == 2)
        {
            if (available_interfaces > 1)
                esp_netif_set_default_netif(network->netif[2]);
        }
        ESP_LOGI(TAG, "Num of Interfaces %hhu", available_interfaces);

        // if (network->HighPriorityNetwork == 0)
        // {
        //     vTaskDelay(pdMS_TO_TICKS(2000));
        //     SafeWifiConnect();
        // }
        vTaskDelay(pdMS_TO_TICKS(5000));
        if (network->isWifiConnected || network->isGsmConnected || network->isEthernetConnected)
        {
            websocket_app_start();
        }
        uint32_t loopCount = 0;
        uint8_t InterfaceId = 3;
        uint8_t InterfaceId_old = 3;
        while (1)
        {

            if (loopCount % 10 == 0)
            {
                esp_netif_t *default_netif = esp_netif_get_default_netif();
                if (default_netif != NULL)
                {
                    if (strcmp(esp_netif_get_desc(default_netif), "sta") == 0)
                    {
                        if (network->isWifiConnected)
                        {
                            InterfaceId = 0;
                            int rssi = 0;
                            esp_err_t ret = esp_wifi_sta_get_rssi(&rssi);
                            if (ret == ESP_OK)
                            {
                                network->wifiSignalStrengthReceived = true;
                                ESP_LOGD("WIFI", "RSSI: %ld dBm", rssi);
                                if (rssi <= -90)
                                {
                                    if (network->wifiWeakSignal)
                                    {
                                        if (rssi < network->wifi_rssi)
                                        {
                                            wifiWeakSignal_old = false;
                                        }
                                    }
                                    network->wifi_rssi = rssi;
                                    network->wifiWeakSignal = true;
                                }
                                else
                                {
                                    network->wifiWeakSignal = false;
                                }
                                network->wifiSignalStrength = rssi;
                            }
                            else
                            {
                                ESP_LOGE("WIFI", "Failed to get RSSI: %s", esp_err_to_name(ret));
                            }
                        }
                    }
                    else if (strcmp(esp_netif_get_desc(default_netif), "ppp") == 0)
                    {
                        InterfaceId = 1;
                    }
                    else if (strcmp(esp_netif_get_desc(default_netif), "eth") == 0)
                    {
                        InterfaceId = 2;
                    }
                    else
                    {
                        InterfaceId = 3;
                    }
                }
                if ((InterfaceId_old != InterfaceId) &&
                    // (InterfaceId_old >= 0) &&
                    (InterfaceId != 3))
                {
                    // ToDo networkSwitchedUpdateDisplay();
                }

                if (network->isWebsocketConnected)
                {
                    WebsocketOffWithNetworkCount = 0;
                }

                if ((network->isWebsocketConnected == false) && (network->isWifiConnected || network->isGsmConnected))
                {
                    WebsocketOffWithNetworkCount++;
                }
                ESP_LOGD(TAG, "WebsocketOffWithNetworkCount %ld", WebsocketOffWithNetworkCount);
                if (WebsocketOffWithNetworkCount > 5)
                {
                    if (config->gsmEnable)
                    {
                        if ((network->isGsmConnected) & (InterfaceId == 1))
                        {
                            network->isGsmConnected = false;
                        }
                    }
                    WebsocketOffWithNetworkCount = 0;
                }
                ESP_LOGD(TAG, "isWebsocketConnected %s", network->isWebsocketConnected ? "True" : "False");
                if (config->wifiEnable)
                    ESP_LOGD(TAG, "wifi connected : %s", network->isWifiConnected ? "True" : "False");
                if (config->gsmEnable)
                    ESP_LOGD(TAG, "GSM connected : %s", network->isGsmConnected ? "True" : "False");
                if (config->ethernetEnable)
                    ESP_LOGD(TAG, "Ethernet connected : %s", network->isEthernetConnected ? "True" : "False");

                ESP_LOGD(TAG, "loopCount %ld", loopCount);

                if (network->isWifiConnected == false) // && (FirmwareUpdateStarted == false))
                    SafeWifiConnect();

                if ((network->isWebsocketConnected == false) && (default_netif != NULL))
                {
                    if (((InterfaceId == 0) && network->isWifiConnected) ||
                        ((InterfaceId == 1) && network->isGsmConnected) ||
                        ((InterfaceId == 2) && network->isEthernetConnected))
                    {
                        if (InterfaceId == 0)
                        {
                            ESP_LOGI(TAG, "Trying Wifi");
                        }
                        else if (InterfaceId == 1)
                        {
                            ESP_LOGE(TAG, "Trying Modem");
                        }
                        else if (InterfaceId == 2)
                        {
                            ESP_LOGE(TAG, "Trying ETH");
                            EthernetInternetDisconnectTime++;
                        }

                        if ((network->isWifiConnected || network->isGsmConnected || network->isEthernetConnected) && (network->client == NULL))
                        {
                            websocket_app_start();
                        }

                        vTaskDelay(1000 / portTICK_PERIOD_MS);
                        if (network->isWebsocketConnected == false)
                        {
                            // check_connectivity(HOST);
                        }
                    }
                }
                if ((network->isWifiConnected == false) && config->wifiEnable)
                {
                    if (config->wifiEnable &&
                        ((network->HighPriorityNetwork == 1) || (TaskStartedTime > 10)))
                    {
                        if (network->isWifiConnected == false) // && (FirmwareUpdateStarted == false))
                            SafeWifiConnect();
                    }
                }

                if ((network->isWebsocketConnected == false) && (TaskStartedTime > 10))
                {
                    if (InterfaceId == 0)
                    {
                        if (network->isWifiConnected)
                        {
                            WifiConnectedButConnectivityLost = true;
                        }
                        if (available_interfaces == 3)
                        {
                            if (config->gsmEnable & network->isEthernetConnected &
                                config->ethernetEnable & network->isGsmConnected)
                            {
                                if (config->ethernetPriority > config->gsmPriority)
                                {
                                    esp_netif_set_default_netif(network->netif[2]);
                                }
                                else
                                {
                                    esp_netif_set_default_netif(network->netif[1]);
                                }
                            }
                            else if (config->gsmEnable & network->isGsmConnected)
                            {
                                esp_netif_set_default_netif(network->netif[1]);
                            }
                            else if (config->ethernetEnable & network->isEthernetConnected)
                            {
                                esp_netif_set_default_netif(network->netif[2]);
                            }
                        }
                        else if (available_interfaces == 2)
                        {
                            if (config->gsmEnable)
                            {
                                if (network->isGsmConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[1]);
                                }
                            }
                            else if (config->ethernetEnable)
                            {
                                if (network->isEthernetConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[2]);
                                }
                            }
                        }
                    }
                    else if (InterfaceId == 2)
                    {
                        if (network->isEthernetConnected)
                        {
                            EthernetConnectedButConnectivityLost = true;
                        }
                        if (available_interfaces == 3)
                        {
                            if (network->isWifiConnected == false)
                                SafeWifiConnect();
                            if (config->gsmEnable & network->isGsmConnected &
                                config->wifiEnable & network->isWifiConnected)
                            {
                                if (config->wifiPriority > config->gsmPriority)
                                {
                                    esp_netif_set_default_netif(network->netif[0]);
                                }
                                else
                                {
                                    esp_netif_set_default_netif(network->netif[1]);
                                }
                            }
                            else if (config->gsmEnable & network->isGsmConnected)
                            {
                                esp_netif_set_default_netif(network->netif[1]);
                            }
                            else if (config->wifiEnable & network->isWifiConnected)
                            {
                                esp_netif_set_default_netif(network->netif[0]);
                            }
                        }
                        else if (available_interfaces == 2)
                        {
                            if (config->gsmEnable)
                            {
                                if (network->isGsmConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[1]);
                                }
                            }
                            else if (config->wifiEnable)
                            {
                                if (network->isWifiConnected == false)
                                    SafeWifiConnect();
                                if (network->isWifiConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[0]);
                                }
                            }
                        }
                    }
                    else if (InterfaceId == 1)
                    {
                        if (network->isGsmConnected)
                        {
                            PPPConnectedButConnectivityLost = true;
                        }
                        if (available_interfaces == 3)
                        {
                            if (network->isWifiConnected == false)
                                SafeWifiConnect();
                            if (config->wifiEnable & network->isWifiConnected &
                                config->ethernetEnable & network->isEthernetConnected)
                            {
                                if (config->ethernetPriority > config->wifiPriority)
                                {
                                    esp_netif_set_default_netif(network->netif[2]);
                                }
                                else
                                {
                                    esp_netif_set_default_netif(network->netif[0]);
                                }
                            }
                            else if (config->wifiEnable)
                            {
                                if (network->isWifiConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[0]);
                                }
                            }
                            else if (config->ethernetEnable)
                            {
                                if (network->isEthernetConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[2]);
                                }
                            }
                        }
                        else if (available_interfaces == 2)
                        {
                            if (network->isWifiConnected == false)
                                SafeWifiConnect();
                            if (config->wifiEnable & network->isWifiConnected)
                            {
                                esp_netif_set_default_netif(network->netif[0]);
                            }
                            else if (config->ethernetEnable)
                            {
                                if (network->isEthernetConnected)
                                {
                                    esp_netif_set_default_netif(network->netif[2]);
                                }
                            }
                        }
                    }
                }
                InterfaceId_old = InterfaceId;
            }
            if ((network->isWebsocketConnected == false) && (network->isWifiConnected || network->isGsmConnected || network->isEthernetConnected))
            {
                WebsocketOffCount++;
                // if ((WebsocketOffCount >= 600) && (isAnyConnectorCharging() == false))
                // {
                //     WebsocketOffCount = 0;
                //     if (config.vcharge_lite_1_4)
                //     {
                //         SlaveSendID(RESTART_ID);
                //         vTaskDelay(2000 / portTICK_PERIOD_MS);
                //     }
                //     esp_restart();
                // }
            }
            // if ((network->isWebsocketConnected == false) && (modem_reset_count > 3) && (isAnyConnectorCharging() == false))
            // {
            //     modem_reset_count = 0;
            //     esp_restart();
            // }

            if (network->isWebsocketConnected)
            {
                EthernetInternetDisconnectTime = 0;
                WebsocketOffCount = 0;
                modem_reset_count = 0;
            }

            if (EthernetInternetDisconnectTime > 2)
            {
                EthernetInternetDisconnectTime = 0;
                restartEthernet();
            }
            if (TaskStartedTime > 10)
                loopCount++;
            else
            {
                loopCount = 0;
            }

            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
    // Constructor
    NetworkController::NetworkController(ReceiveDataFunc receiveFunction)
    {
        this->receiveFunc = receiveFunction;
        network = this;
        isWebsocketConnected = false;
        client = NULL;
        isDiagnosticServerConnected = false;
        isApConnected = false;
        isWifiConnected = false;
        isGsmConnected = false;
        isEthernetConnected = false;
        isWifiEnabled = config->wifiEnable;
        isModemEnabled = config->gsmEnable;
        isEthernetEnabled = config->ethernetEnable;

        ESP_LOGI(TAG, "Wifi SSID %s", config->wifiSSID);
        ESP_LOGI(TAG, "Wifi Password %s", config->wifiPassword);
        xTaskCreate(&networkTask, "networkTask", 4096, NULL, 2, NULL);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // Destructor
    NetworkController::~NetworkController()
    {
        // Cleanup, if necessary
    }

} // namespace NetworkModule
