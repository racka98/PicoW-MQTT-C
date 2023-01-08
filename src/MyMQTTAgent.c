
#include "MyMQTTAgent.h"

#include <stdlib.h>

/* MQTT Agent ports. */
#include <freertos_agent_message.h>
#include <freertos_command_pool.h>

#include "NetTCPTransport.h"
#include "WiFiHelper.h"

/// START PRIVATE METHODS/VARIABLES

/***
 * Initialisation code
 * @return
 */
static MQTTStatus_t _mqtt_agent_init();

/***
 * Task object running to manage MQTT interface
 * @param pvParameters
 */
static void _mqtt_agent_vTask(void *pvParameters);

/***
 * Call back function on connect
 * @param pCmdCallbackContext
 * @param pReturnInfo
 */
static void _mqtt_agent_connectCmdCallback(MQTTAgentCommandContext_t *pCmdCallbackContext,
                                           MQTTAgentReturnInfo_t *pReturnInfo);

/***
 * Callback on when new data is received
 * @param pMqttAgentContext
 * @param packetId
 * @param pxPublishInfo
 */
static void _mqtt_agent_incomingPublishCallback(MQTTAgentContext_t *pMqttAgentContext,
                                                uint16_t packetId,
                                                MQTTPublishInfo_t *pxPublishInfo);
/***
 * Run loop for the task
 */
static void _mqtt_agent_run();

/***
 * Connect to MQTT hub
 * @return
 */
static MQTTStatus_t _mqtt_agent_MQTTconn();

/***
 * Perform TCP Connection
 * @return true if succeeds
 */
static bool _mqtt_agent_TCPconn();

/***
 * Set the connection state variable
 * @param s
 */
static void _mqtt_agent_setConnState(MyMQTTState s);

NetworkContext_t _xNetworkContext;
// TCPTransport _xTcpTrans;

// MQTT Server and credentials
const char *_pUser;
const char *_pPasswd;
const char *_pId;
const char *_pTarget = NULL;
char _mqtt_agent_xMacStr[14];
uint16_t _mqtt_agent_xPort = 1883;
bool _mqtt_agent_xRecon = false;

// MQTT Will object
static const char *_WILLTOPICFORMAT;
char *_pWillTopic = NULL;
static const char *_WILLPAYLOAD = "{'online':0}";
MQTTPublishInfo_t _xWillInfo;

// Buffers and queues
uint8_t _xNetworkBuffer[MQTT_AGENT_NETWORK_BUFFER_SIZE];
uint8_t _xStaticQueueStorageArea[MQTT_AGENT_COMMAND_QUEUE_LENGTH * sizeof(MQTTAgentCommand_t *)];
StaticQueue_t _xStaticQueueStructure;
MQTTAgentMessageContext_t _xCommandQueue;
MQTTAgentContext_t _xGlobalMqttAgentContext;
/**
 * @brief The global array of subscription elements.
 *
 * @note No thread safety is required to this array, since the updates the array
 * elements are done only from one task at a time. The subscription manager
 * implementation expects that the array of the subscription elements used for
 * storing subscriptions to be initialized to 0. As this is a global array, it
 * will be initialized to 0 by default.
 */
int _xGlobalSubscriptionList = 0;
TaskHandle_t _mqtt_agent_xHandle = NULL;

// State machine state
MyMQTTState _mqtt_agent_xConnState = Offline;

// Storage for publishing message
MQTTAgentCommandInfo_t _mqtt_agent_xCommandInfo;

// Single Observer
// MQTTAgentObserver *_pObserver = NULL;
/// END PRIVATE METHODS/VARIABLES

/***
 * Destructor
 */
TaskHandle_t mqtt_agent_getTask() {
    return _mqtt_agent_xHandle;
}

unsigned int mqtt_agent_getStakHighWater() {
    if (_mqtt_agent_xHandle != NULL)
        return uxTaskGetStackHighWaterMark(_mqtt_agent_xHandle);
    else
        return 0;
}

void mqtt_agent_destructor() {
    if (_pWillTopic != NULL) {
        vPortFree(_pWillTopic);
        _pWillTopic = NULL;
    }
}

MQTTStatus_t _mqtt_agent_init() {
    TransportInterface_t xTransport;
    MQTTStatus_t xReturn;
    MQTTFixedBuffer_t xFixedBuffer = {.pBuffer = _xNetworkBuffer, .size = MQTT_AGENT_NETWORK_BUFFER_SIZE};

    MQTTAgentMessageInterface_t messageInterface =
        {
            .pMsgCtx = NULL,
            .send = Agent_MessageSend,
            .recv = Agent_MessageReceive,
            .getCommand = Agent_GetCommand,
            .releaseCommand = Agent_ReleaseCommand};

    LogDebug(("Creating command queue."));
    _xCommandQueue.queue = xQueueCreateStatic(MQTT_AGENT_COMMAND_QUEUE_LENGTH,
                                              sizeof(MQTTAgentCommand_t *),
                                              _xStaticQueueStorageArea,
                                              &_xStaticQueueStructure);

    if (_xCommandQueue.queue == NULL) {
        LogDebug(("MQTTAgent::mqttInit ERROR Queue not initialised"));
        return MQTTIllegalState;
    }
    messageInterface.pMsgCtx = &_xCommandQueue;

    // Setup TCP
    tcp_setup();

    /* Initialize the task pool. */
    Agent_InitializePool();

    // Fill in Transforp interface
    _xNetworkContext.mqttTask = NULL;
    _xNetworkContext.tcpTransport = NULL;  //&_xTcpTrans;
    xTransport.pNetworkContext = &_xNetworkContext;
    xTransport.send = tcp_transSend;
    xTransport.recv = tcp_transRead;

    /* Initialize MQTT library. */
    xReturn = MQTTAgent_Init(&_xGlobalMqttAgentContext,
                             &messageInterface,
                             &xFixedBuffer,
                             &xTransport,
                             tcp_getCurrentTime,
                             _mqtt_agent_incomingPublishCallback,
                             /* Context to pass into the callback.
                             Passing the pointer to subscription array. */
                             &_xGlobalSubscriptionList);

    return xReturn;
}

void _mqtt_agent_incomingPublishCallback(MQTTAgentContext_t *pMqttAgentContext,
                              uint16_t packetId, MQTTPublishInfo_t *pxPublishInfo) {
    LogDebug(("MSG(%d,%d) %.*s:%.*s\n",
              pxPublishInfo->topicNameLength,
              pxPublishInfo->payloadLength,
              pxPublishInfo->topicNameLength,
              pxPublishInfo->pTopicName,
              pxPublishInfo->payloadLength,
              (char *)pxPublishInfo->pPayload));
}

void mqtt_agent_credentials(const char *user, const char *passwd, const char *id) {
    _pUser = user;
    _pPasswd = passwd;

    if (id != NULL) {
        _pId = id;
    } else {
        _pId = _pUser;
    }
    LogInfo(("MQTT Credentials Id=%s, usr=%s\n", _pId, _pUser));

    if (_pWillTopic == NULL) {
        _pWillTopic = (char *)pvPortMalloc(mqtt_helper_lenLifeCycleTopic(_pId,
                                                                         MQTT_TOPIC_LIFECYCLE_OFFLINE));
        if (_pWillTopic != NULL) {
            mqtt_helper_genLifeCycleTopic(_pWillTopic, _pId, MQTT_TOPIC_LIFECYCLE_OFFLINE);
        } else {
            LogError(("Unable to allocate LC topic"));
        }
    }
}

bool mqtt_agent_connect(const char *target, uint16_t port, bool recon) {
    _pTarget = target;
    _mqtt_agent_xPort = port;
    _mqtt_agent_xRecon = recon;
    _mqtt_agent_setConnState(TCPReq);
    LogDebug(("TCP Requested\n"));
    return true;
}

void mqtt_agent_start(UBaseType_t priority) {
    if (_mqtt_agent_init() == MQTTSuccess) {
        xTaskCreate(
            _mqtt_agent_vTask,
            "MQTTAgent",
            2512,
            NULL,  //( void * ) this,
            priority,
            &_mqtt_agent_xHandle);
    }
}

void mqtt_agent_stop() {
    if (_mqtt_agent_xConnState != Offline) {
        tcp_transClose();
        _mqtt_agent_setConnState(Offline);
    }
    if (_mqtt_agent_xHandle != NULL) {
        vTaskDelete(_mqtt_agent_xHandle);
        _mqtt_agent_xHandle = NULL;
    }
}

void _mqtt_agent_vTask(void *pvParameters) {
    _mqtt_agent_run();
}

void _mqtt_agent_run() {
    LogDebug(("MQTTAgent run\n"));

    MQTTStatus_t status;

    for (;;) {
        switch (_mqtt_agent_xConnState) {
            case Offline: {
                break;
            }
            case TCPReq: {
                if (wifi_isJoined()) {
                    _mqtt_agent_TCPconn();
                } else {
                    LogInfo(("Network offline, awaiting reconnect"));
                    vTaskDelay(MQTT_RECON_DELAY);
                }
                break;
            }
            case TCPConned: {
                LogDebug(("Attempting MQTT conn\n"));
                status = _mqtt_agent_MQTTconn();
                if (status == MQTTSuccess) {
                    _mqtt_agent_setConnState(MQTTReq);
                    LogDebug(("MQTTconn ok\n"));
                } else {
                    _mqtt_agent_setConnState(Offline);
                    LogDebug(("MQTTConn failed\n"));
                }
                break;
            }
            case MQTTReq: {
                _mqtt_agent_setConnState(MQTTConned);
                break;
            }
            case MQTTConned: {
                _mqtt_agent_setConnState(Online);
                break;
            }
            case Online: {
                LogDebug(("Starting CMD loop\n"));

                status = MQTTAgent_CommandLoop(&_xGlobalMqttAgentContext);

                // The function returns on either receiving a terminate command,
                // undergoing network disconnection OR encountering an error.
                if ((status == MQTTSuccess) && (_xGlobalMqttAgentContext.mqttContext.connectStatus == MQTTNotConnected)) {
                    // A terminate command was processed and MQTT connection was closed.
                    // Need to close socket connection.
                    // Platform_DisconnectNetwork( mqttAgentContext.mqttContext.transportInterface.pNetworkContext );
                    LogDebug(("MQTT Closed\n"));
                    tcp_transClose();
                    _mqtt_agent_setConnState(Offline);
                } else if (status == MQTTSuccess) {
                    // Terminate command was processed but MQTT connection was not
                    // closed. Thus, need to close both MQTT and socket connections.
                    LogDebug(("Terminated processed\n"));
                    status = MQTT_Disconnect(&(_xGlobalMqttAgentContext.mqttContext));
                    // assert( status == MQTTSuccess );
                    // Platform_DisconnectNetwork( mqttAgentContext.mqttContext.transportInterface.pNetworkContext );
                    tcp_transClose();
                    _mqtt_agent_setConnState(Offline);
                } else {
                    // Handle error.
                    LogDebug(("Command Loop error %d\n", status));
                    _mqtt_agent_setConnState(Offline);
                }
                break;
            }
            case MQTTRecon: {
                if (wifi_isJoined()) {
                    tcp_transClose();
                }
                vTaskDelay(MQTT_RECON_DELAY);
                _mqtt_agent_setConnState(TCPReq);
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

const char *mqtt_agent_getId() {
    return _pId;
}

void mqtt_agent_close() {
    tcp_transClose();
    _mqtt_agent_xRecon = false;
    _mqtt_agent_setConnState(Offline);
}

void _mqtt_agent_connectCmdCallback(MQTTAgentCommandContext_t *pCmdCallbackContext,
                                    MQTTAgentReturnInfo_t *pReturnInfo) {
    LogDebug(("ConnectCmdCallback called\n"));
}

MQTTStatus_t _mqtt_agent_MQTTconn() {
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
    xConnectInfo.pClientIdentifier = _pId;
    xConnectInfo.clientIdentifierLength = (uint16_t)strlen(_pId);
    xConnectInfo.pUserName = _pUser;
    xConnectInfo.userNameLength = (uint16_t)strlen(_pUser);
    xConnectInfo.pPassword = _pPasswd;
    xConnectInfo.passwordLength = (uint16_t)strlen(_pPasswd);

    _xWillInfo.qos = MQTTQoS1;
    _xWillInfo.pTopicName = _pWillTopic;
    _xWillInfo.topicNameLength = strlen(_xWillInfo.pTopicName);
    _xWillInfo.pPayload = _WILLPAYLOAD;
    _xWillInfo.payloadLength = strlen(_WILLPAYLOAD);
    _xWillInfo.retain = false;

    /* Set MQTT keep-alive period. It is the responsibility of the application
     * to ensure that the interval between Control Packets being sent does not
     * exceed the Keep Alive value.  In the absence of sending any other
     * Control Packets, the Client MUST send a PINGREQ Packet. */
    xConnectInfo.keepAliveSeconds = MQTTKEEPALIVETIME;

    /* Send MQTT CONNECT packet to broker. LWT is not used in this demo, so it
     * is passed as NULL. */
    LogDebug(("MQTT Connect \n"));
    xResult = MQTT_Connect(&(_xGlobalMqttAgentContext.mqttContext),
                           &xConnectInfo,
                           &_xWillInfo,
                           30000U,
                           &xSessionPresent);

    if (xResult != MQTTSuccess) {
        LogError(("MQTTConnect error %d", xResult));
    }
    return xResult;
}

bool _mqtt_agent_TCPconn() {
    LogDebug(("TCP Connect...."));
    if (tcp_transConnect(_pTarget, _mqtt_agent_xPort)) {
        _mqtt_agent_setConnState(TCPConned);
        LogDebug(("TCP Connected"));
        return true;
    } else {
        LogDebug(("TCP Connection failed"));
        _mqtt_agent_setConnState(Offline);
    }
    return false;
}

void _mqtt_agent_setConnState(MyMQTTState s) {
    _mqtt_agent_xConnState = s;

    switch (_mqtt_agent_xConnState) {
        case Offline: {
            MQTTOffline();
            if (_mqtt_agent_xRecon) {
                _mqtt_agent_setConnState(MQTTRecon);
            }
            break;
        }
        case Online: {
            MQTTOnline();
            break;
        }
        default: {
            ;
        }
    }
}
