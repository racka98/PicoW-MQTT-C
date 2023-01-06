#include <FreeRTOS.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

/**
 * START ESSENTIAL
*/

#define DELAY 500

#define MAX_NAME_LEN 20

static char pTransportTaskName[MAX_NAME_LEN];

/***
 * Get the static depth required in words
 * @return - words
 */
static configSTACK_DEPTH_TYPE transport_getMaxStackSize();

/***
 * Get high water for stack
 * @return close to zero means overflow risk
 */
unsigned int getTransportStakHighWater();

/***
 * Set the task handle
 * @return
 */
void set_transport_task_handle(TaskHandle_t pHandle);

/***
 * Get the task handle
 * @return TaskHandle_t
 */
TaskHandle_t get_transport_task_handle();

/***
 * Start the task
 * @param name - Give the task a name (<20 characters)
 * @param priority - priority - 0 is idle
 * @return
 */
bool start_transport_agent(const char* name, UBaseType_t priority);  // tskIDLE_PRIORITY

/***
 * Stop transport task
 * @return
 */
void stop_transport_agent();

/**
 * END ESSENTIAL
*/

/**
 * Test the task
*/
void transport_test(void *pvParameters);