#include "MQTTAgent.h"

#include <stdlib.h>

#include "WiFiHelper.h"

/* MQTT Agent ports. */
#include "freertos_agent_message.h"
#include "freertos_command_pool.h"

const char *MQTTAgent::WILLPAYLOAD = "{'online':0}";

/***
 * Constructor
 * @param sockNum - Socket Number to use
 * @param eth - Ethernet helper for communicating to hardware
 */
MQTTAgent::MQTTAgent() {
    // NOP
}

/***
 * Destructor
 */
MQTTAgent::~MQTTAgent() {
    if (pWillTopic != NULL) {
        vPortFree(pWillTopic);
        pWillTopic = NULL;
    }
}

/***
 * Initialisation code
 * @return
 */
MQTTStatus_t MQTTAgent::init() {
    TransportInterface_t xTransport;
    MQTTStatus_t xReturn;
    MQTTFixedBuffer_t xFixedBuffer = {.pBuffer = xNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE};

    MQTTAgentMessageInterface_t messageInterface =
        {
            .pMsgCtx = NULL,
            .send = Agent_MessageSend,
            .recv = Agent_MessageReceive,
            .getCommand = Agent_GetCommand,
            .releaseCommand = Agent_ReleaseCommand};

    LogDebug(("Creating command queue."));
    xCommandQueue.queue = xQueueCreateStatic(MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                             sizeof(MQTTAgentCommand_t *),
                                             xStaticQueueStorageArea,
                                             &xStaticQueueStructure);

    if (xCommandQueue.queue == NULL) {
        LogDebug(("MQTTAgent::mqttInit ERROR Queue not initialised"));
        return MQTTIllegalState;
    }
    messageInterface.pMsgCtx = &xCommandQueue;

    /* Initialize the task pool. */
    Agent_InitializePool();

    // Fill in Transforp interface
    xNetworkContext.mqttTask = NULL;
    xNetworkContext.tcpTransport = &xTcpTrans;
    xTransport.pNetworkContext = &xNetworkContext;
    xTransport.send = TCPTransport::staticSend;
    xTransport.recv = TCPTransport::staticRead;

    /* Initialize MQTT library. */
    xReturn = MQTTAgent_Init(&xGlobalMqttAgentContext,
                             &messageInterface,
                             &xFixedBuffer,
                             &xTransport,
                             TCPTransport::getCurrentTime,
                             MQTTAgent::incomingPublishCallback,
                             /* Context to pass into the callback. Passing the pointer to subscription array. */
                             this);

    return xReturn;
}

/***
 * Callback on when new data is received
 * @param pMqttAgentContext
 * @param packetId
 * @param pxPublishInfo
 */
void MQTTAgent::incomingPublishCallback(MQTTAgentContext_t *pMqttAgentContext,
                                        uint16_t packetId,
                                        MQTTPublishInfo_t *pxPublishInfo) {
    LogDebug(("MSG(%d,%d) %.*s:%.*s\n",
              pxPublishInfo->topicNameLength,
              pxPublishInfo->payloadLength,
              pxPublishInfo->topicNameLength,
              pxPublishInfo->pTopicName,
              pxPublishInfo->payloadLength,
              (char *)pxPublishInfo->pPayload));
}

/***
 * Set credentials
 * @param user - string pointer. Not copied so pointer must remain valid
 * @param passwd - string pointer. Not copied so pointer must remain valid
 * @param id - string pointer. Not copied so pointer must remain valid.
 * If not provide ID will be user
 *
 */
void MQTTAgent::credentials(const char *user, const char *passwd, const char *id) {
    this->pUser = user;
    this->pPasswd = passwd;

    if (id != NULL) {
        this->pId = id;
    } else {
        this->pId = this->pUser;
    }
    LogInfo(("MQTT Credentials Id=%s, usr=%s\n", this->pId, this->pUser));

    if (pWillTopic == NULL) {
        pWillTopic = (char *)pvPortMalloc(MQTTTopicHelper::lenLifeCycleTopic(this->pId, MQTT_TOPIC_LIFECYCLE_OFFLINE));
        if (pWillTopic != NULL) {
            MQTTTopicHelper::genLifeCycleTopic(pWillTopic, this->pId, MQTT_TOPIC_LIFECYCLE_OFFLINE);
        } else {
            LogError(("Unable to allocate LC topic"));
        }
    }
}

/***
 * Connect to mqtt server
 * @param target - hostname or ip address, Not copied so pointer must remain valid
 * @param port - port number
 * @param ssl - unused
 * @return
 */
bool MQTTAgent::mqttConnect(const char *target, uint16_t port, bool recon) {
    this->pTarget = target;
    this->xPort = port;
    this->xRecon = recon;
    setConnState(TCPReq);
    LogDebug(("TCP Requested\n"));
    return true;
}

/***
 *  create the vtask, will get picked up by scheduler
 *
 *  */
void MQTTAgent::start(UBaseType_t priority) {
    if (init() == MQTTSuccess) {
        xTaskCreate(
            MQTTAgent::vTask,
            "MQTTAgent",
            2512,
            (void *)this,
            priority,
            &xHandle);
    }
}

/***
 * Internal function used by FreeRTOS to run the task
 * @param pvParameters
 */
void MQTTAgent::vTask(void *pvParameters) {
    MQTTAgent *task = (MQTTAgent *)pvParameters;
    task->run();
}

/***
 * Run loop for the task
 */
void MQTTAgent::run() {
    LogDebug(("MQTTAgent run\n"));

    MQTTStatus_t status;

    for (;;) {
        switch (xConnState) {
            case Offline: {
                break;
            }
            case TCPReq: {
                if (WifiHelper::isJoined()) {
                    TCPconn();
                } else {
                    LogInfo(("Network offline, awaiting reconnect"));
                    vTaskDelay(MQTT_RECON_DELAY);
                }
                break;
            }
            case TCPConned: {
                LogDebug(("Attempting MQTT conn\n"));
                status = MQTTconn();
                if (status == MQTTSuccess) {
                    setConnState(MQTTReq);
                    LogDebug(("MQTTconn ok\n"));
                } else {
                    setConnState(Offline);
                    LogDebug(("MQTTConn failed\n"));
                }
                break;
            }
            case MQTTReq: {
                setConnState(MQTTConned);
                break;
            }
            case MQTTConned: {
                setConnState(Online);
                break;
            }
            case Online: {
                LogDebug(("Starting CMD loop\n"));

                status = MQTTAgent_CommandLoop(&xGlobalMqttAgentContext);

                // The function returns on either receiving a terminate command,
                // undergoing network disconnection OR encountering an error.
                if ((status == MQTTSuccess) && (xGlobalMqttAgentContext.mqttContext.connectStatus == MQTTNotConnected)) {
                    // A terminate command was processed and MQTT connection was closed.
                    // Need to close socket connection.
                    // Platform_DisconnectNetwork( mqttAgentContext.mqttContext.transportInterface.pNetworkContext );
                    LogDebug(("MQTT Closed\n"));
                    xTcpTrans.transClose();
                    setConnState(Offline);
                } else if (status == MQTTSuccess) {
                    // Terminate command was processed but MQTT connection was not
                    // closed. Thus, need to close both MQTT and socket connections.
                    LogDebug(("Terminated processed\n"));
                    status = MQTT_Disconnect(&(xGlobalMqttAgentContext.mqttContext));
                    // assert( status == MQTTSuccess );
                    // Platform_DisconnectNetwork( mqttAgentContext.mqttContext.transportInterface.pNetworkContext );
                    xTcpTrans.transClose();
                    setConnState(Offline);
                } else {
                    // Handle error.
                    LogDebug(("Command Loop error %d\n", status));
                    setConnState(Offline);
                }
                break;
            }
            case MQTTRecon: {
                if (WifiHelper::isJoined()) {
                    xTcpTrans.transClose();
                }
                vTaskDelay(MQTT_RECON_DELAY);
                setConnState(TCPReq);
                break;
            }
            default: {
            }
        };

        taskYIELD();
        // vTaskDelay(1);
    }

    LogError(("RUN STOPPED\n"));
}

/***
 * Returns the id of the client
 * @return
 */
const char *MQTTAgent::getId() {
    return pId;
}

/***
 * Close connection
 */
void MQTTAgent::close() {
    xTcpTrans.transClose();
    xRecon = false;
    setConnState(Offline);
}

/***
 * Call back function on connect
 * @param pCmdCallbackContext
 * @param pReturnInfo
 */
void MQTTAgent::connectCmdCallback(MQTTAgentCommandContext_t *pCmdCallbackContext,
                                   MQTTAgentReturnInfo_t *pReturnInfo) {
    LogDebug(("ConnectCmdCallback called\n"));
}

/***
 * Connect to MQTT server
 * @return
 */
MQTTStatus_t MQTTAgent::MQTTconn() {
    MQTTStatus_t xResult;
    MQTTConnectInfo_t xConnectInfo;
    bool xSessionPresent = false;

    /* Many fields not used in this demo so start with everything at 0. */
    (void)memset((void *)&xConnectInfo, 0x00, sizeof(xConnectInfo));

    /* Start with a clean session i.e. direct the MQTT broker to discard any
     * previous session data. Also, establishing a connection with clean
     * session will ensure that the broker does not store any data when this
     * client gets disconnected. */
    xConnectInfo.cleanSession = true;

    /* The client identifier is used to uniquely identify this MQTT client to
     * the MQTT broker. In a production device the identifier can be something
     * unique, such as a device serial number. */
    xConnectInfo.pClientIdentifier = pId;
    xConnectInfo.clientIdentifierLength = (uint16_t)strlen(pId);
    xConnectInfo.pUserName = pUser;
    xConnectInfo.userNameLength = (uint16_t)strlen(pUser);
    xConnectInfo.pPassword = pPasswd;
    xConnectInfo.passwordLength = (uint16_t)strlen(pPasswd);

    xWillInfo.qos = MQTTQoS1;
    xWillInfo.pTopicName = pWillTopic;
    xWillInfo.topicNameLength = strlen(xWillInfo.pTopicName);
    xWillInfo.pPayload = MQTTAgent::WILLPAYLOAD;
    xWillInfo.payloadLength = strlen(MQTTAgent::WILLPAYLOAD);
    xWillInfo.retain = false;

    /* Set MQTT keep-alive period. It is the responsibility of the application
     * to ensure that the interval between Control Packets being sent does not
     * exceed the Keep Alive value.  In the absence of sending any other
     * Control Packets, the Client MUST send a PINGREQ Packet. */
    xConnectInfo.keepAliveSeconds = MQTTKEEPALIVETIME;

    /* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
     * is passed as NULL. */
    LogDebug(("MQTT Connect \n"));
    xResult = MQTT_Connect(&(xGlobalMqttAgentContext.mqttContext),
                           &xConnectInfo,
                           &xWillInfo,
                           30000U,
                           &xSessionPresent);

    if (xResult != MQTTSuccess) {
        LogError(("MQTTConnect error %d", xResult));
    }
    return xResult;
}

/***
 * Connect to MQTT hub
 * @return
 */
bool MQTTAgent::TCPconn() {
    LogDebug(("TCP Connect...."));
    if (xTcpTrans.transConnect(pTarget, xPort)) {
        setConnState(TCPConned);
        LogDebug(("TCP Connected"));
        return true;
    } else {
        LogDebug(("TCP Connection failed"));
        setConnState(Offline);
    }
    return false;
}

/***
 * Set a single observer to get call back on state changes
 * @param obs
 */
void MQTTAgent::setObserver(MQTTAgentObserver *obs) {
    pObserver = obs;
}

/***
 * Stop task
 * @return
 */
void MQTTAgent::stop() {
    if (xConnState != Offline) {
        xTcpTrans.transClose();
        setConnState(Offline);
    }
    if (xHandle != NULL) {
        vTaskDelete(xHandle);
        xHandle = NULL;
    }
}

/***
 * Set the connection state variable
 * @param s
 */
void MQTTAgent::setConnState(MQTTState s) {
    xConnState = s;

    if (pObserver != NULL) {
        switch (xConnState) {
            case Offline: {
                pObserver->MQTTOffline();
                if (xRecon) {
                    setConnState(MQTTRecon);
                }
                break;
            }
            case Online: {
                pObserver->MQTTOnline();
                break;
            }
            default: {
                ;
            }
        }
    }
}

/***
 * Get the FreeRTOS task being used
 * @return
 */
TaskHandle_t MQTTAgent::getTask() {
    return xHandle;
}

/***
 * Get high water for stack
 * @return close to zero means overflow risk
 */
unsigned int MQTTAgent::getStakHighWater() {
    if (xHandle != NULL)
        return uxTaskGetStackHighWaterMark(xHandle);
    else
        return 0;
}
