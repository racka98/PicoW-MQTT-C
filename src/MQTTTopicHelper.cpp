
#include "MQTTTopicHelper.h"

#include <stdio.h>
#include <stdlib.h>

#include <cstring>

MQTTTopicHelper::MQTTTopicHelper() {
    // TODO Auto-generated constructor stub
}

MQTTTopicHelper::~MQTTTopicHelper() {
    // TODO Auto-generated destructor stub
}

/***
 * Get length of the lifecycle topic, to allow dynamic creation of string
 * @param id
 * @param name  = name of the lifecycle topic (ON, OFF, KEEP)
 * @return
 */
size_t MQTTTopicHelper::lenLifeCycleTopic(const char *id, const char *name) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(MQTT_TOPIC_LIFECYCLE) +
          strlen(id) +
          strlen(name) +
          4;
    return res;
}

/***
 * generate the lifecycle topic for thing
 * @param topic - out to write the topic
 * @param id - id of the thing
 * @param name  = name of the lifecycle topic (ON, OFF, KEEP)
 */
void MQTTTopicHelper::genLifeCycleTopic(char *topic, const char *id, const char *name) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_TOPIC_LIFECYCLE, name);
}

/***
 * Get length of the thing topic, under the ID in structure
 * @param id - string id of the thing
 * @param topic - string name of the topic
 * @return
 */
size_t MQTTTopicHelper::lenThingTopic(const char *id, const char *name) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_TOPIC_HEADER) +
          strlen(name) +
          5;
    return res;
}

/***
 * Generate the thing topic full name
 * @param topic - output to write topic too
 * @param id - string id of the thing
 * @param name - string name of the topic
 */
void MQTTTopicHelper::genThingTopic(char *topic, const char *id, const char *name) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_TOPIC_HEADER, name);
}

/***
 * Get length of the Group topic, under the ID in structure
 * @param grp - string  of the group name
 * @param name - string name of the topic
 * @return
 */
size_t MQTTTopicHelper::lenGroupTopic(const char *grp, const char *name) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_GROUP_HEADER) +
          strlen(grp) +
          strlen(MQTT_TOPIC_HEADER) +
          strlen(name) +
          5;
    return res;
}

/***
 * Generate the group topic full name
 * @param topic - output to write topic too
 * @param grp - string of group anme
 * @param name - string name of the topic
 */
void MQTTTopicHelper::genGroupTopic(char *topic, const char *grp, const char *name) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_GROUP_HEADER,
            grp,
            MQTT_TOPIC_HEADER,
            name);
}

/***
 * Topic length of update topics published by thing
 * @param id of thing
 * @return
 */
size_t MQTTTopicHelper::lenThingUpdate(const char *id) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_STATE_TOPIC) +
          strlen(MQTT_STATE_TOPIC_UPDATE) +
          5;
    return res;
}

/***
 * Generate update topic for thing
 * @param topic - output to write to
 * @param id - Id of thing
 */
void MQTTTopicHelper::getThingUpdate(char *topic, const char *id) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_STATE_TOPIC, MQTT_STATE_TOPIC_UPDATE);
}

/***
 * Topic length of get topics published by thing
 * @param id of thing
 * @return
 */
size_t MQTTTopicHelper::lenThingGet(const char *id) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_STATE_TOPIC) +
          strlen(MQTT_STATE_TOPIC_GET) +
          5;
    return res;
}

/***
 * Generate get topic for thing
 * @param topic - output to write to
 * @param id - Id of thing
 */
void MQTTTopicHelper::getThingGet(char *topic, const char *id) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_STATE_TOPIC, MQTT_STATE_TOPIC_GET);
}

/***
 * Topic length of set topics published by thing
 * @param id of thing
 * @return
 */
size_t MQTTTopicHelper::lenThingSet(const char *id) {
    size_t res = 0;
    res = strlen(MQTT_TOPIC_THING_HEADER) +
          strlen(id) +
          strlen(MQTT_STATE_TOPIC) +
          strlen(MQTT_STATE_TOPIC_SET) +
          5;
    return res;
}

/***
 * Generate set topic for thing
 * @param topic - output to write to
 * @param id - Id of thing
 */
void MQTTTopicHelper::getThingSet(char *topic, const char *id) {
    sprintf(topic, "%s/%s/%s/%s", MQTT_TOPIC_THING_HEADER, id,
            MQTT_STATE_TOPIC, MQTT_STATE_TOPIC_SET);
}
