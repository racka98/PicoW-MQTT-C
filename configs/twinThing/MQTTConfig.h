/***
 * MQTTConfig.h
 * Configuration for MQTT framework
 * Jon Durrant
 * 21-Nov-2021
 */

#ifndef MQTTCONFIG_H__
#define MQTTCONFIG_H__

#define LIBRARY_LOG_NAME "MQTT EXP"
#define LIBRARY_LOG_LEVEL LOG_INFO
//#define SdkLog( X ) printf X
#include <logging_stack.h>



#define MQTT_RECON_DELAY 3000 //Ticks to wait on reconnect
#define MQTT_KEEP_ALIVE 10 //Keep alive publish timer, seconds


#define MQTT_TOPIC_THING_HEADER "TNG"
#define MQTT_TOPIC_HEADER "TPC"
#define MQTT_TOPIC_LIFECYCLE "LC"
#define MQTT_TOPIC_LIFECYCLE_OFFLINE "OFF"
#define MQTT_TOPIC_LIFECYCLE_ONLINE "ON"
#define MQTT_TOPIC_LIFECYCLE_KEEP_ALIVE "KEEP"
#define MQTT_TOPIC_GROUP_HEADER "GRP"
#define MQTT_STATE_TOPIC "STATE"
#define MQTT_STATE_TOPIC_UPDATE "UPD"
#define MQTT_STATE_TOPIC_GET "GET"
#define MQTT_STATE_TOPIC_SET "SET"

#define STATE_MSG_BUF_LEN 512
#define STATE_MAX_MSG_LEN 350
#define MQTT_JSON_BUF_NUM 25 //Number of JSON objects within payloads



#define ESP01_RST_PIN 11
#define ESP01_RST_DELAY 2000
#define ESP01_SNTP_TIMEZONE 0
#define ESP01_SNTP_LOCAL_HOST "nas3.local.jondurrant.com"


#endif
