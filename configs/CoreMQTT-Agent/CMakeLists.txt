
target_sources(coreMQTTAgent
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/freertos_agent_message.c
    ${CMAKE_CURRENT_LIST_DIR}/freertos_command_pool.c
)
