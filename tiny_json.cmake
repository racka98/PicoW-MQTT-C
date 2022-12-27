# Add library cpp files

# if (NOT DEFINED TINY_JSON_DIR)
#     set(TINY_JSON_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/tiny-json")
# endif()

if (DEFINED ENV{TINY_JSON_PATH} AND (NOT TINY_JSON_PATH))
    set(TINY_JSON_PATH $ENV{TINY_JSON_PATH})
    message("Using TINY_JSON_PATH from environment ('${TINY_JSON_PATH}')")
endif ()

add_library(tiny_json INTERFACE)
target_sources(tiny_json INTERFACE
    ${TINY_JSON_PATH}/tiny-json.c
)

# Add include directory
target_include_directories(tiny_json INTERFACE 
   ${TINY_JSON_PATH}/
)

# Add the standard library to the build
target_link_libraries(tiny_json INTERFACE pico_stdlib)
