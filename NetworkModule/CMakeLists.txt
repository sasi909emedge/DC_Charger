idf_component_register(SRCS 
                    "NetworkModule.cpp"
                    "wifi.cpp"
                    "gsm.cpp"
                    "ethernet.cpp"
                    INCLUDE_DIRS "include"

                    REQUIRES 
                    ConfigModule
                    json
                    esp_event
                    tcp_transport
                    esp_wifi
                    esp_eth
                    nvs_flash
                    esp_system
                    esp_modem
                    esp_websocket_client
                    )
