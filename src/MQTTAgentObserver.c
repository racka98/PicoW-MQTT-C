
#include "MQTTAgentObserver.h"

#include <MQTTConfig.h>

void MQTTOffline() {
    LogInfo(("MQTT Offline"));
}

void MQTTOnline() {
    LogInfo(("MQTT Online"));
}

void MQTTSend() {
    LogInfo(("MQTT Send"));
}

void MQTTRecv() {
    LogInfo(("MQTT Receive"));
}
