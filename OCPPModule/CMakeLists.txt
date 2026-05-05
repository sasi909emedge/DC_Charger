idf_component_register(SRCS "OCPPModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    json
                    ConfigModule
                    PLCModule
                    NetworkModule
                    TimeModule)
