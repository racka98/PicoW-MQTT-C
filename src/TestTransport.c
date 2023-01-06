
#include "TestTransport.h"

#include "NetTCPTransport.h"
#include "WifiHelper.h"

// Private properties and methods

static bool _testConnect();
static bool _testTrans();
int _xTests = 0;
int _xSuccessful = 0;
// The task
static TaskHandle_t _xTransportHandle = NULL;

configSTACK_DEPTH_TYPE transport_getMaxStackSize() {
    return 5000;
}

unsigned int getTransportStakHighWater() {
    if (_xTransportHandle != NULL)
        return uxTaskGetStackHighWaterMark(_xTransportHandle);
    else
        return 0;
}

void set_transport_task_handle(TaskHandle_t pHandle) {
    _xTransportHandle = pHandle;
}

TaskHandle_t get_transport_task_handle() {
    return _xTransportHandle;
}

void stop_transport_agent() {
    if (_xTransportHandle != NULL) {
        vTaskDelete(_xTransportHandle);
        _xTransportHandle = NULL;
    }
}

void transport_test(void* pvParameters) {
    _xTests++;
    if (!_testConnect())
        printf("CONNECTIONED FAILED\n");
    else
        _xSuccessful++;

    _xTests++;
    if (!_testTrans())
        printf("Trans FAILED\n");
    else
        _xSuccessful++;
}

bool start_transport_agent(const char* name, UBaseType_t priority) {
    BaseType_t res;

    if (strlen(name) >= MAX_NAME_LEN) {
        memcpy(pTransportTaskName, name, MAX_NAME_LEN);
        pTransportTaskName[MAX_NAME_LEN - 1] = 0;
    } else {
        strcpy(pTransportTaskName, name);
    }

    // Create Your Task
    res = xTaskCreate(
        transport_test,               // Task to be run
        pTransportTaskName,           // Name of the Task for debugging and managing its Task Handle
        transport_getMaxStackSize(),  // Stack depth to be allocated for use with task's stack (see docs)
        NULL,                         // Arguments needed by the Task (NULL because we don't have any)
        priority,                     // Task Priority - Higher the number the more priority [max is (configMAX_PRIORITIES - 1) provided in FreeRTOSConfig.h]
        &_xTransportHandle            // Task Handle if available for managing the task
    );
    return (res == pdPASS);
}

bool _testConnect() {
    if (wifi_isJoined) return true;

    if (wifi_join(WIFI_SSID, WIFI_PASSWORD, WIFI_RETRIES)) {
        if (wifi_isJoined())
            return true;
        else
            printf("is Joined is false\n\r");
    } else {
        printf("Connect failed\n\r");
    }

    return false;
}

bool _testTrans() {
    uint16_t targetPort = 80;
    char targetHost[] = "racka.work";
    char message[] =
        "GET / HTTP/1.1\r\n"
        "Host: racka.work\r\n"
        "Connection: close\r\n"
        "\r\n";

    NetworkContext_t net;
    char buf[1024];
    int32_t retVal;

    // Setup tcp
    tcp_setup();

    if (!tcp_transConnect(targetHost, targetPort)) {
        printf("Socket connection failed!\n\r");
        return false;
    }

    // Check no data and nonblocking
    retVal = tcp_transRead(&net, buf, sizeof(buf));
    if (retVal != 0) {
        printf("Read data initially\n\r");
        return false;
    }

    retVal = tcp_transSend(&net, message, sizeof(message));
    // if (retVal != strlen(message)) {
    //     printf("Socket Send failed\n\r");
    //     return false;
    // }

    retVal = 1;
    while (retVal >= 0) {
        retVal = tcp_transRead(&net, buf, sizeof(buf));
        if (retVal > 0) {
            tcp_debugPrintBuffer("READ:", buf, retVal);
        }
    }

    tcp_transClose();
    tcp_destruct();

    return true;
}
