idf_component_register(SRCS "TimeModule.cpp"
                    INCLUDE_DIRS "include"
                    REQUIRES 
                    NVSModule
                    PRIV_REQUIRES
                    espressif__cjson
                    )
                    
