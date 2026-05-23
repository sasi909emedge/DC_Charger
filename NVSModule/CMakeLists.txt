idf_component_register(SRCS "NVSModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    nvs_flash
                    spiffs
                    PRIV_REQUIRES
                    espressif__cjson
                    )
