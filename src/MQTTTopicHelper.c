
#include "MQTTTopicHelper.h"

#include <stdio.h>
#include <string.h>

size_t mqtt_helper_lenLifeCycleTopic(const char *id, const char *name) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(MQTT_TOPIC_LIFECYCLE) +
          strlen(id) +
          strlen(name) +
          4;
    return res;
}

void mqtt_helper_genLifeCycleTopic(char *topic, const char *id, const char *name) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id, MQTT_TOPIC_LIFECYCLE, name);
}

size_t mqtt_helper_lenThingTopic(const char *id, const char *name) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_TOPIC_HEADER) +
          strlen(name) +
          5;
    return res;
}

void mqtt_helper_genThingTopic(char *topic, const char *id, const char *name) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id, MQTT_TOPIC_HEADER, name);
}

size_t mqtt_helper_lenGroupTopic(const char *grp, const char *name) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_GROUP_HEADER) +
          strlen(grp) +
          strlen(MQTT_TOPIC_HEADER) +
          strlen(name) +
          5;
    return res;
}

void mqtt_helper_genGroupTopic(char *topic, const char *grp, const char *name) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_GROUP_HEADER, grp, MQTT_TOPIC_HEADER, name);
}

size_t mqtt_helper_lenThingUpdate(const char *id) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_STATE_TOPIC) +
          strlen(MQTT_STATE_TOPIC_UPDATE) +
          5;
    return res;
}

void mqtt_helper_getThingUpdate(char *topic, const char *id) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_STATE_TOPIC, MQTT_STATE_TOPIC_UPDATE);
}

size_t mqtt_helper_lenThingGet(const char *id) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_STATE_TOPIC) +
          strlen(MQTT_STATE_TOPIC_GET) +
          5;
    return res;
}

void mqtt_helper_getThingGet(char *topic, const char *id) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_STATE_TOPIC, MQTT_STATE_TOPIC_GET);
}

size_t mqtt_helper_lenThingSet(const char *id) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_STATE_TOPIC) +
          strlen(MQTT_STATE_TOPIC_SET) +
          5;
    return res;
}

void mqtt_helper_getThingSet(char *topic, const char *id) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_STATE_TOPIC, MQTT_STATE_TOPIC_SET);
}
