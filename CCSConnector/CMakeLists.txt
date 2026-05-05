idf_component_register(
                    SRCS 
                    "Display.cpp" 
                    "ConnectorTask.cpp" 
                    "OcppTask.cpp" 
                    "CCSConnector.cpp"
                    "AuthorizeTask.cpp"

                    INCLUDE_DIRS "include"

                    REQUIRES 
                    PLCModule 
                    OCPPModule 
                    ConfigModule
                    TimeModule
                    EnergyModule
                    NetworkModule
                    GpioModule)
