idf_component_register(SRCS "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES 
                    nvs_flash
                    TonhePowerModule 
                    PLCModule 
                    SerialModule 
                    CCSConnector 
                    OCPPModule
                    ConfigModule
                    NVSModule
                    EnergyModule
                    NetworkModule
                    DisplayModule
                    GpioModule
                    PowerMuxModule)
