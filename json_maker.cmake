# Add library cpp files

# if (NOT DEFINED JSON_MAKER_DIR)
#     set(JSON_MAKER_DIR "${CMAKE_CURRENT_LIST_DIR}/lib/json-maker")
# endif()

if (DEFINED ENV{JSON_MAKER_PATH} AND (NOT JSON_MAKER_PATH))
    set(JSON_MAKER_PATH $ENV{JSON_MAKER_PATH})
    message("Using JSON_MAKER_PATH from environment ('${JSON_MAKER_PATH}')")
endif ()

add_library(json_maker STATIC)
target_sources(json_maker PUBLIC
    ${JSON_MAKER_PATH}/src/json-maker.c
)

# Add include directory
target_include_directories(json_maker PUBLIC 
    ${JSON_MAKER_PATH}/src/include
)

# Add the standard library to the build
target_link_libraries(json_maker PUBLIC pico_stdlib)
