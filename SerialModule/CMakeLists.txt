idf_component_register(SRCS "SerialModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_uart TimeModule ConfigModule)
