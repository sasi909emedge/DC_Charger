idf_component_register(SRCS
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
