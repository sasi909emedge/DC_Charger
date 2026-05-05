#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include "freertos/semphr.h"
#include "esp_netif.h"
#include "esp_eth_driver.h"
#include "esp_eth.h"
#include "esp_event.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth_mac.h"
#include "sdkconfig.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "NetworkModule.hpp"
#include "ConfigModule.hpp"
#include <cstring>

#define TAG "ETH"

static EventGroupHandle_t wifi_events;

uint8_t mac_addr[6] = {};

esp_eth_handle_t eth_handle;

namespace NetworkModule
{

    uint8_t reverse_byte(uint8_t byte)
    {
        uint8_t result = 0;
        for (int i = 0; i < 8; i++)
        {
            result <<= 1;         // Shift result left
            result |= (byte & 1); // Copy the least significant bit of byte to result
            byte >>= 1;           // Shift byte right
        }
        return result;
    }
    static void eth_event_handler(void *args, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data)
    {
        switch (event_id)
        {
        case ETHERNET_EVENT_CONNECTED:

            if (config->ethernetConfig == ConfigModule::EthernetConfigType::DHCP)
            {
                esp_netif_ip_info_t ip_info;
                if (!esp_netif_str_to_ip4(config->ipAddress, &ip_info.ip))
                {
                    ESP_LOGI(TAG, "Invalid ipAddress");
                }
                if (!esp_netif_str_to_ip4(config->gatewayAddress, &ip_info.gw))
                {
                    ESP_LOGI(TAG, "Invalid gatewayAddress");
                }
                if (!esp_netif_str_to_ip4(config->subnetMask, &ip_info.netmask))
                {
                    ESP_LOGI(TAG, "Invalid subnetMask");
                }

                // IP4_ADDR(&ip_info.ip, 192, 168, 0, 173);
                // IP4_ADDR(&ip_info.gw, 192, 168, 0, 1);
                // IP4_ADDR(&ip_info.netmask, 255, 255, 255, 0);

                esp_netif_dhcpc_stop(network->netif[2]);
                esp_netif_set_ip_info(network->netif[2], &ip_info);

                esp_netif_dns_info_t dns_info;
                if (!esp_netif_str_to_ip4(config->dnsAddress, &dns_info.ip.u_addr.ip4))
                {
                    ESP_LOGI(TAG, "Invalid dnsAddress");
                }
                // IP4_ADDR(&dns_info.ip.u_addr.ip4, 192, 168, 0, 1); // Set DNS server address here
                dns_info.ip.type = ESP_NETIF_DNS_MAIN;
                esp_netif_set_dns_info(network->netif[2], ESP_NETIF_DNS_MAIN, &dns_info);
            }

            esp_eth_ioctl(eth_handle, ETH_CMD_G_MAC_ADDR, mac_addr);
            ESP_LOGI(TAG, "Ethernet Link Up");
            ESP_LOGI(TAG, "Ethernet HW Addr %02x:%02x:%02x:%02x:%02x:%02x",
                     mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
            if (config->ethernetConfig == ConfigModule::EthernetConfigType::DHCP)
            {
                network->isEthernetConnected = true;
            }

            break;
        case ETHERNET_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "Ethernet Link Down");
            network->isEthernetConnected = false;
            break;
        case ETHERNET_EVENT_START:
            ESP_LOGI(TAG, "Ethernet Started");
            break;
        case ETHERNET_EVENT_STOP:
            ESP_LOGI(TAG, "Ethernet Stopped");
            break;
        default:
            break;
        }
    }

    static void got_ip_event_handler(void *args, esp_event_base_t event_base,
                                     int32_t event_id, void *event_data)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        const esp_netif_ip_info_t *ip_info = &event->ip_info;

        ESP_LOGI(TAG, "Ethernet Got IP Address");
        ESP_LOGI(TAG, "~~~~~~~~~~~");
        ESP_LOGI(TAG, "IP:" IPSTR, IP2STR(&ip_info->ip));
        ESP_LOGI(TAG, "MASK:" IPSTR, IP2STR(&ip_info->netmask));
        ESP_LOGI(TAG, "GW:" IPSTR, IP2STR(&ip_info->gw));
        ESP_LOGI(TAG, "~~~~~~~~~~~");
        if (config->ethernetConfig == ConfigModule::EthernetConfigType::DHCP)
        {
            network->isEthernetConnected = true;
        }
        ESP_LOGI(TAG, "~~~~~~~~~~~");
    }

    void ethernet_initialise(int priority)
    {
        spi_bus_config_t buscfg = {
            .mosi_io_num = ETH_MOSI_PIN,
            .miso_io_num = ETH_MISO_PIN,
            .sclk_io_num = ETH_SCLK_PIN,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        };
        spi_bus_initialize(ETH_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);

        spi_device_interface_config_t spi_devcfg = {
            .mode = 0,
            .clock_speed_hz = 10000000,
            .spics_io_num = ETH_CS_PIN,
            .queue_size = 20,
        };

        // Init common MAC and PHY configs to default
        eth_mac_config_t mac_config = ETH_MAC_DEFAULT_CONFIG();
        // mac_config.rx_task_stack_size = 2048;
        eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();
        phy_config.reset_gpio_num = -1;
        phy_config.phy_addr = 1;

        eth_w5500_config_t w5500_config = ETH_W5500_DEFAULT_CONFIG(ETH_SPI_HOST, &spi_devcfg);
        w5500_config.poll_period_ms = 20;
        w5500_config.int_gpio_num = -1;
        esp_eth_mac_t *mac = esp_eth_mac_new_w5500(&w5500_config, &mac_config);
        esp_eth_phy_t *phy = esp_eth_phy_new_w5500(&phy_config);

        // Init Ethernet driver to default and install it
        esp_eth_config_t eth_config_spi = ETH_DEFAULT_CONFIG(mac, phy);
        esp_err_t err = esp_eth_driver_install(&eth_config_spi, &eth_handle);
        if (err == ESP_OK)
        {
            memcpy(mac_addr, config->ByteMacAddress, 6);
            // mac_addr[0] = reverse_byte(mac_addr[0]);
            // mac_addr[1] = reverse_byte(mac_addr[1]);
            // mac_addr[2] = reverse_byte(mac_addr[2]);
            mac_addr[3] = reverse_byte(mac_addr[3]);
            mac_addr[4] = reverse_byte(mac_addr[4]);
            mac_addr[5] = reverse_byte(mac_addr[5]);
            mac->set_addr(mac, mac_addr);

            // Create an instance of esp-netif for Ethernet
            esp_netif_config_t base_netif_cfg = ESP_NETIF_DEFAULT_ETH();
            // base_netif_cfg.base->route_prio = priority; //constant can not set

            // esp_netif_config_t cfg = {
            //     .base = &base_netif_cfg,
            //     .stack = ESP_NETIF_NETSTACK_DEFAULT_ETH,
            // };
            network->netif[2] = esp_netif_new(&base_netif_cfg);
            esp_eth_netif_glue_handle_t glue = esp_eth_new_netif_glue(eth_handle);
            // Attach Ethernet driver to TCP/IP stack
            esp_netif_attach(network->netif[2], glue);

            // Register user defined event handers
            esp_event_handler_register(ETH_EVENT, ESP_EVENT_ANY_ID, &eth_event_handler, NULL);
            esp_event_handler_register(IP_EVENT, IP_EVENT_ETH_GOT_IP, &got_ip_event_handler, NULL);

            // Start Ethernet driver state machine
            esp_eth_start(eth_handle);
        }
    }

    void restartEthernet(void)
    {
        ESP_LOGE(TAG, "Restarting Ethernet");
        esp_eth_stop(eth_handle);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        esp_eth_start(eth_handle);
    }
} // namespace NetworkModule
