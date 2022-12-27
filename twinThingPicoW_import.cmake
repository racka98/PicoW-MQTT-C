# Add library cpp files
#set(FREERTOS_PORT_PATH      ${CMAKE_CURRENT_LIST_DIR}/lib/twinThingPicoESP/port/FreeRTOS)

if (DEFINED ENV{TWIN_THING_PICO_PATH} AND (NOT TWIN_THING_PICO_PATH))
    set(TWIN_THING_PICO_PATH $ENV{TWIN_THING_PICO_PATH})
    message("Using TWIN_THING_PICO_PATH from environment ('${TWIN_THING_PICO_PATH}')")
endif ()

include("${TWIN_THING_PICO_PATH}/twinThingPicoW.cmake") # twinThingPicoW cmake file pulled from the lib
