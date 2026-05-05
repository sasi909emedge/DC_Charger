idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES 
                    NetworkModule
                    driver
                    esp_eth
                    esp_wifi
                    esp_event
                    tcp_transport
                    )
