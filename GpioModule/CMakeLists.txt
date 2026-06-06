idf_component_register(SRCS "GpioModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    ConfigModule
                    esp_driver_gpio
                    )
