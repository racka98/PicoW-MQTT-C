#pragma once

class MQTTAgentObserver {
   public:
    MQTTAgentObserver();
    virtual ~MQTTAgentObserver();

    virtual void MQTTOffline();

    virtual void MQTTOnline();

    virtual void MQTTSend();

    virtual void MQTTRecv();
};
