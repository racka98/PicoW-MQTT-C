#include <stdio.h>
#include <string.h>

#include "FreeRTOS.h"
#include "MQTTConfig.h"
#include "MQTTRouter.h"
#include "MQTTTopicHelper.h"
#include "pico/stdlib.h"
#include "task.h"
#include "tiny-json.h"

// Definitions

#define LED_TOPIC "LED/req"
#define PAYLOAD_ON "on"
#define LED_JSON_POOL 4
#define LED_BUFFER 20

// Static variables

static uint8_t xLedPad;
static char* pLedTopic = NULL;
// Temporaty storage buffer so we can null terminate the string
char pBuffer[LED_BUFFER];
// Json decoding buffer
json_t pJsonPool[LED_JSON_POOL];

static void init_led(char* id);

/***
 * Constructor
 * @param id - ID of the device, used to build the topic
 * @param gp - GPIO PAD number for the LED to control
 */
void setup_mqtt_led(char* id, uint8_t gp);

/***
 * Use the interface to setup all the subscriptions
 * @param interface
 */
// void subscribe(MQTTInterface *interface);