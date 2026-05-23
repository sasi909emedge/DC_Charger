idf_component_register(SRCS 
                    "NetworkModule.cpp"
                    "wifi.cpp"
                    "gsm.cpp"
                    "ethernet.cpp"
                    INCLUDE_DIRS "include"

                    REQUIRES 
                    esp_driver_spi
                    esp_driver_uart
                    ConfigModule
                    esp_event
                    tcp_transport
                    esp_wifi
                    esp_eth
                    nvs_flash
                    esp_system
                    esp_modem
                    esp_websocket_client
                    PRIV_REQUIRES
                    espressif__cjson
                    espressif__w5500
                    )
