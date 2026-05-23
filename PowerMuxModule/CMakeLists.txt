idf_component_register(SRCS "PowerMuxModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    driver
                    CCSConnector
                    PLCModule 
                    OCPPModule 
                    ConfigModule
                    TimeModule
                    NVSModule
                    TonhePowerModule
                    GpioModule)