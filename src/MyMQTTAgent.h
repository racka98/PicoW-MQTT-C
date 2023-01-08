#pragma once

#include <FreeRTOS.h>
#include <MQTTConfig.h>
#include <core_mqtt.h>
#include <core_mqtt_agent.h>

/* MQTT Agent ports. */
#include <freertos_agent_message.h>
#include <freertos_command_pool.h>

#include "MQTTAgentObserver.h"
#include "MQTTTopicHelper.h"
#include "NetTCPTransport.h"

#ifndef MQTT_AGENT_NETWORK_BUFFER_SIZE
#define MQTT_AGENT_NETWORK_BUFFER_SIZE 512
#endif

#ifndef MAXSUBS
#define MAXSUBS 12
#endif

#ifndef MQTTKEEPALIVETIME
#define MQTTKEEPALIVETIME 10
#endif

#ifndef MQTT_RECON_DELAY
#define MQTT_RECON_DELAY 10
#endif

// Enumerator used to control the state machine at centre of agent
typedef enum MyMQTTState {
    Offline,
    TCPReq,
    TCPConned,
    MQTTReq,
    MQTTConned,
    MQTTRecon,
    Online
} MyMQTTState;

/***
 * Set credentials
 * @param user - string pointer. Not copied so pointer must remain valid
 * @param passwd - string pointer. Not copied so pointer must remain valid
 * @param id - string pointer. Not copied so pointer must remain valid. I
 * f not provide ID will be user
 * @return lwespOK if succeeds
 */
void mqtt_agent_credentials(const char* user, const char* passwd, const char* id);

/***
 * Connect to mqtt server - Actual connection is done in the state machine so task must be running
 * @param target - hostname or ip address, Not copied so pointer must remain valid
 * @param port - port number
 * @param ssl - unused
 * @return
 */
bool mqtt_agent_connect(const char* target, uint16_t port, bool recon);

/***
 * Start the task running
 * @param priority - priority to run within FreeRTOS
 */
void mqtt_agent_start(UBaseType_t priority);

/***
 * Stop task
 * @return
 */
void mqtt_agent_stop();

/***
 * Returns the id of the MQTT client
 * @return
 */
const char* mqtt_agent_getId();

/***
 * Close connection
 */
void mqtt_agent_close();

/***
 * Set a single observer to get call back on state changes
 * @param obs
 */
//void mqtt_agent_setObserver(void* obs);

/***
 * Get the FreeRTOS task being used
 * @return
 */
TaskHandle_t mqtt_agent_getTask();

/***
 * Get high water for stack
 * @return close to zero means overflow risk
 */
unsigned int mqtt_agent_getStakHighWater();

/**
 * Destructor, to be called when you want to clear everything
 */
void mqtt_agent_destructor();
