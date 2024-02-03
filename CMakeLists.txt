cmake_minimum_required(VERSION 3.12)

# Change your executable name to something creative!
set(NAME PICO_MQTT) # <-- Name your project/executable here!

# Pull in Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_led_control C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_BOARD pico_w) # Set the board used

# Set config files paths to be used
set(TWIN_THING_PICO_CONFIG_PATH ${CMAKE_CURRENT_LIST_DIR}/configs/twinThing) # twinThingPicoW config files
set(COREMQTT_PORT_PATH ${CMAKE_CURRENT_LIST_DIR}/configs/CoreMQTT) # coreMQTT config files
set(COREMQTT_AGENT_PORT_PATH ${CMAKE_CURRENT_LIST_DIR}/configs/CoreMQTT-Agent) # coreMQTT config files
include(lwip_import.cmake) # lwip config files
SET(FREERTOS_CONFIG_FILE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/configs/FreeRTOS-Kernel" CACHE STRING "Local Config")
include_directories("${FREERTOS_CONFIG_FILE_DIRECTORY}") # FreeRTOS config files
include_directories("${TWIN_THING_PICO_CONFIG_PATH}") # twinThingPicoW config files

# Initialize the Pico SDK
pico_sdk_init() 

# Initialize all other external libs & dependencies
include(FreeRTOS_Kernel_import.cmake) # Configure FreeRTOS Kernel
include(FreeRTOS_core_MQTT.cmake) # Configure FreeRTOS coreMQTT
include(FreeRTOS_core_MQTT_Agent.cmake) # Configure FreeRTOS coreMQTT Agent
# include(json_maker.cmake) # Configure json-maker lib
# include(tiny_json.cmake) # Configure tiny-json lib
#include(twinThingPicoW_import.cmake) # Import twinThingPicoW lib

add_subdirectory(src) # Source code dir
