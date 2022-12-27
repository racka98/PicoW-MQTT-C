# Add ${CMAKE_CURRENT_LIST_DIR}rary cpp files

# if (NOT DEFINED CORE_MQTT_AGENT_DIR)
#     set(CORE_MQTT_AGENT_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/coreMQTT-Agent")
# endif()

# Get coreMQTTAgent from PATH
if (DEFINED ENV{CORE_MQTT_AGENT_PATH} AND (NOT CORE_MQTT_AGENT_PATH))
    set(CORE_MQTT_AGENT_PATH $ENV{CORE_MQTT_AGENT_PATH})
    message("Using CORE_MQTT_AGENT_PATH from environment ('${CORE_MQTT_AGENT_PATH}')")
endif ()

add_library(coreMQTTAgent STATIC)
include("${CORE_MQTT_AGENT_PATH}/mqttAgentFilePaths.cmake")

target_sources(coreMQTTAgent PUBLIC
	${MQTT_AGENT_SOURCES}
)
add_subdirectory(${COREMQTT_AGENT_PORT_PATH})

target_include_directories(coreMQTTAgent PUBLIC 
	${MQTT_AGENT_INCLUDE_PUBLIC_DIRS}
	${COREMQTT_AGENT_PORT_PATH}
)



# Add the standard ${CMAKE_CURRENT_LIST_DIR}rary to the build
target_link_libraries(coreMQTTAgent coreMQTT FreeRTOS-Kernel-Heap4 pico_stdlib)
