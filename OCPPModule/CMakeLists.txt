idf_component_register(SRCS "OCPPModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    ConfigModule
                    PLCModule
                    NetworkModule
                    TimeModule
                    espressif__cjson
                    )
                    
