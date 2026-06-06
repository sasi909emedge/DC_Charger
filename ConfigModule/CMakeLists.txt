idf_component_register(SRCS "ConfigModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    NVSModule
                    esp_https_ota
                    app_update
                    PRIV_REQUIRES
                    espressif__cjson
                    )
                    
