#pragma once

#include <stdlib.h>

#ifndef MQTT_TOPIC_THING_HEADER
#define MQTT_TOPIC_THING_HEADER "TNG"
#endif
#ifndef MQTT_TOPIC_HEADER
#define MQTT_TOPIC_HEADER "TPC"
#endif
#ifndef MQTT_TOPIC_LIFECYCLE
#define MQTT_TOPIC_LIFECYCLE "LC"
#endif
#ifndef MQTT_TOPIC_GROUP_HEADER
#define MQTT_TOPIC_GROUP_HEADER "GRP"
#endif

#ifndef MQTT_TOPIC_LIFECYCLE_OFFLINE
#define MQTT_TOPIC_LIFECYCLE_OFFLINE "OFF"
#endif
#ifndef MQTT_TOPIC_LIFECYCLE_ONLINE
#define MQTT_TOPIC_LIFECYCLE_ONLINE "ON"
#endif
#ifndef MQTT_TOPIC_LIFECYCLE_KEEP_ALIVE
#define MQTT_TOPIC_LIFECYCLE_KEEP_ALIVE "KEEP"
#endif

#ifndef MQTT_STATE_TOPIC
#define MQTT_STATE_TOPIC "STATE"
#endif
#ifndef MQTT_STATE_TOPIC_UPDATE
#define MQTT_STATE_TOPIC_UPDATE "UPD"
#endif
#ifndef MQTT_STATE_TOPIC_GET
#define MQTT_STATE_TOPIC_GET "GET"
#endif
#ifndef MQTT_STATE_TOPIC_SET
#define MQTT_STATE_TOPIC_SET "SET"
#endif
#ifndef MQTT_STATE_PAYLOAD_GET
#define MQTT_STATE_PAYLOAD_GET "{\"GET\":1}"
#endif

/***
 * Get length of the lifecycle topic, to allow dynamic creation of string
 * @param id = think ID
 * @param name  = name of the lifecycle topic (ON, OFF, KEEP)
 * @return
 */
size_t mqtt_helper_lenLifeCycleTopic(const char *id, const char *name);

/***
 * generate the lifecycle topic for thing
 * @param topic - out to write the topic
 * @param id - id of the thing
 * @param name  = name of the lifecycle topic (ON, OFF, KEEP)
 */
void mqtt_helper_genLifeCycleTopic(char *topic, const char *id, const char *name);

/***
 * Get length of the thing topic, under the ID in structure
 * @param id - string id of the thing
 * @param name - string name of the topic
 * @return
 */
size_t mqtt_helper_lenThingTopic(const char *id, const char *name);

/***
 * Generate the thing topic full name
 * @param topic - output to write topic too
 * @param id - string id of the thing
 * @param name - string name of the topic
 */
void mqtt_helper_genThingTopic(char *topic, const char *id, const char *name);

/***
 * Get length of the Group topic, under the ID in structure
 * @param grp - string  of the group name
 * @param name - string name of the topic
 * @return
 */
size_t mqtt_helper_lenGroupTopic(const char *grp, const char *name);

/***
 * Generate the group topic full name
 * @param topic - output to write topic too
 * @param grp - string of group anme
 * @param name - string name of the topic
 */
void mqtt_helper_genGroupTopic(char *topic, const char *grp, const char *name);

/***
 * Topic length of update topics published by thing
 * @param id of thing
 * @return
 */
size_t mqtt_helper_lenThingUpdate(const char *id);

/***
 * Generate update topic for thing
 * @param topic - output to write to
 * @param id - Id of thing
 */
void mqtt_helper_getThingUpdate(char *topic, const char *id);

/***
 * Topic length of get topics published by thing
 * @param id of thing
 * @return
 */
size_t mqtt_helper_lenThingGet(const char *id);

/***
 * Generate get topic for thing
 * @param topic - output to write to
 * @param id - Id of thing
 */
void mqtt_helper_getThingGet(char *topic, const char *id);

/***
 * Topic length of set topics published by thing
 * @param id of thing
 * @return
 */
size_t mqtt_helper_lenThingSet(const char *id);

/***
 * Generate set topic for thing
 * @param topic - output to write to
 * @param id - Id of thing
 */
void mqtt_helper_getThingSet(char *topic, const char *id);
