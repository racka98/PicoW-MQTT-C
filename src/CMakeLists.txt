add_executable(${NAME} 
        blink.c
        blink_queue.c
        scheduling_priorities.c
        mutex.c
        semaphore.c
        )

# pull in common dependencies
target_link_libraries(${NAME}
        pico_stdlib                        # for core pico functionality
        pico_cyw43_arch_lwip_sys_freertos  # Wifi with FreeRTOS
        FreeRTOS-Kernel-Heap4              # FreeRTOS kernel and dynamic heap
        FREERTOS_PORT                      # FreeRTOS config files
        LWIP_PORT                          # LWIP config files
        twinThingPicoW                     # twinThingPicoW helper lib
	json_maker                         # Library for constructing json
	tiny_json                          # Library for parsing json
	coreMQTT                           # coreMQTT for FreeRTOS
	coreMQTTAgent                      # coreMQTT Agent for FreeRTOS
        )

# Compile definitions needed by the project
target_compile_definitions(${NAME} PRIVATE
    WIFI_SSID=\"$ENV{WIFI_SSID}\"
    WIFI_PASSWORD=\"$ENV{WIFI_PASSWORD}\"
    MQTT_CLIENT=\"$ENV{MQTT_CLIENT}\"
    MQTT_USER=\"$ENV{MQTT_USER}\"
    MQTT_PASSWD=\"$ENV{MQTT_PASSWD}\"
    MQTT_HOST=\"$ENV{MQTT_HOST}\"
    MQTT_PORT=$ENV{MQTT_PORT}
    configNUM_CORES=2
)

        
# Enable print functionality on usb & disable on uart
pico_enable_stdio_usb(${NAME} 1) 
pico_enable_stdio_uart(${NAME} 0)

# create map/bin/hex file etc.
pico_add_extra_outputs(${NAME})

target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR} )
