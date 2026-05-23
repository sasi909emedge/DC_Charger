# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

execute_process(
    COMMAND git describe --tags --abbrev=0
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_TAG
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)

if(GIT_TAG)
    message("Using git tag as project version: ${GIT_TAG}")
    # add_definitions(-DPROJECT_VER="${GIT_TAG}")
else()
    message("Git tag not found. Using git commit hash as project version.")
    # add_definitions(-DPROJECT_VER="${PROJECT_VER}")
endif()
# set(EXTRA_COMPONENT_DIRS "esp-idf-lib/components")
set(EXTRA_COMPONENT_DIRS
    CCSConnector
    ConfigModule
    DisplayModule
    EnergyModule
    GpioModule
    IsolationModule
    NetworkModule
    NVSModule
    OCPPModule
    PLCModule
    PowerMuxModule
    SerialModule
    TimeModule
    TonhePowerModule
)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(DC_Charger)
