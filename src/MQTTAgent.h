#pragma once

#include "FreeRTOS.h"
#include "MQTTAgentObserver.h"
#include "MQTTConfig.h"
#include "MQTTTopicHelper.h"
#include "TCPTransport.h"
#include "core_mqtt.h"
#include "core_mqtt_agent.h"

extern "C" {
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"
}

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
enum MQTTState { Offline,
                 TCPReq,
                 TCPConned,
                 MQTTReq,
                 MQTTConned,
                 MQTTRecon,
                 Online };

class MQTTAgent {
   public:
    /***
     * Constructor
     */
    MQTTAgent();

    /***
     * Distructor
     */
    virtual ~MQTTAgent();

    /***
     * Set credentials
     * @param user - string pointer. Not copied so pointer must remain valid
     * @param passwd - string pointer. Not copied so pointer must remain valid
     * @param id - string pointer. Not copied so pointer must remain valid. I
     * f not provide ID will be user
     * @return lwespOK if succeeds
     */
    void credentials(const char *user, const char *passwd, const char *id = NULL);

    /***
     * Connect to mqtt server - Actual connection is done in the state machine so task must be running
     * @param target - hostname or ip address, Not copied so pointer must remain valid
     * @param port - port number
     * @param ssl - unused
     * @return
     */
    bool mqttConnect(const char *target, uint16_t port, bool recon = false);

    /***
     * Start the task running
     * @param priority - priority to run within FreeRTOS
     */
    void start(UBaseType_t priority = tskIDLE_PRIORITY);

    /***
     * Stop task
     * @return
     */
    void stop();

    /***
     * Returns the id of the MQTT client
     * @return
     */
    virtual const char *getId();

    /***
     * Close connection
     */
    virtual void close();

    /***
     * Set a single observer to get call back on state changes
     * @param obs
     */
    virtual void setObserver(MQTTAgentObserver *obs);

    /***
     * Get the FreeRTOS task being used
     * @return
     */
    virtual TaskHandle_t getTask();

    /***
     * Get high water for stack
     * @return close to zero means overflow risk
     */
    virtual unsigned int getStakHighWater();

   private:
    /***
     * Initialisation code
     * @return
     */
    MQTTStatus_t init();

    /***
     * Task object running to manage MQTT interface
     * @param pvParameters
     */
    static void vTask(void *pvParameters);

    /***
     * Call back function on connect
     * @param pCmdCallbackContext
     * @param pReturnInfo
     */
    static void connectCmdCallback(MQTTAgentCommandContext_t *pCmdCallbackContext,
                                   MQTTAgentReturnInfo_t *pReturnInfo);

    /***
     * Callback on when new data is received
     * @param pMqttAgentContext
     * @param packetId
     * @param pxPublishInfo
     */
    static void incomingPublishCallback(MQTTAgentContext_t *pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t *pxPublishInfo);
    /***
     * Run loop for the task
     */
    void run();

    /***
     * Connect to MQTT hub
     * @return
     */
    MQTTStatus_t MQTTconn();

    /***
     * Perform TCP Connection
     * @return true if succeeds
     */
    bool TCPconn();

    /***
     * Set the connection state variable
     * @param s
     */
    void setConnState(MQTTState s);

    NetworkContext_t xNetworkContext;
    TCPTransport xTcpTrans;

    // MQTT Server and credentials
    const char *pUser;
    const char *pPasswd;
    const char *pId;
    const char *pTarget = NULL;
    char xMacStr[14];
    uint16_t xPort = 1883;
    bool xRecon = false;

    // MQTT Will object
    static const char *WILLTOPICFORMAT;
    char *pWillTopic = NULL;
    static const char *WILLPAYLOAD;
    MQTTPublishInfo_t xWillInfo;

    // Buffers and queues
    uint8_t xNetworkBuffer[MQTT_AGENT_NETWORK_BUFFER_SIZE];
    uint8_t xStaticQueueStorageArea[MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof(MQTTAgentCommand_t *)];
    StaticQueue_t xStaticQueueStructure;
    MQTTAgentMessageContext_t xCommandQueue;
    MQTTAgentContext_t xGlobalMqttAgentContext;
    TaskHandle_t xHandle = NULL;

    // State machine state
    MQTTState xConnState = Offline;

    // Storage for publishing message
    MQTTAgentCommandInfo_t xCommandInfo;

    // Single Observer
    MQTTAgentObserver *pObserver = NULL;
};
