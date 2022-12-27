# Add ${CMAKE_CURRENT_LIST_DIR}rary cpp files

# if (NOT DEFINED CORE_MQTT_DIR)
#     set(CORE_MQTT_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/coreMQTT")
# endif()

if (DEFINED ENV{CORE_MQTT_PATH} AND (NOT CORE_MQTT_PATH))
    set(CORE_MQTT_PATH $ENV{CORE_MQTT_PATH})
    message("Using CORE_MQTT_PATH from environment ('${CORE_MQTT_PATH}')")
endif ()

add_library(coreMQTT STATIC)
include("${CORE_MQTT_PATH}/mqttFilePaths.cmake")

target_sources(coreMQTT PUBLIC
	${MQTT_SOURCES}
	${MQTT_SERIALIZER_SOURCES}
)

target_include_directories(coreMQTT PUBLIC 
	${MQTT_INCLUDE_PUBLIC_DIRS}
	${COREMQTT_PORT_PATH}
)


# Add the standard ${CMAKE_CURRENT_LIST_DIR}rary to the build
target_link_libraries(coreMQTT FreeRTOS-Kernel-Heap4 pico_stdlib)
