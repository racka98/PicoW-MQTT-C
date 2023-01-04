#include <FreeRTOS.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

#define BLINK_LED_PAD CYW43_WL_GPIO_LED_PIN

// Blink Delay
#define DELAY 500

#define MAX_NAME_LEN 20

// The task
static TaskHandle_t xBlinkHandle = NULL;

static char pBlinkTaskName[MAX_NAME_LEN];

/***
 * Get the static depth required in words
 * @return - words
 */
static configSTACK_DEPTH_TYPE getMaxStackSize();

/***
 * Get high water for stack
 * @return close to zero means overflow risk
 */
unsigned int getBlinkStakHighWater();

/***
 * Set the task handle
 * @return
 */
void set_blink_task_handle(TaskHandle_t pHandle);

/***
 * Get the task handle
 * @return TaskHandle_t
 */
TaskHandle_t get_blink_task_handle();

/***
 * Start the task
 * @param name - Give the task a name (<20 characters)
 * @param priority - priority - 0 is idle
 * @return
 */
bool start_blink_agent(const char* name, UBaseType_t priority);  // tskIDLE_PRIORITY

/***
 * Stop blink task
 * @return
 */
void stop_blink_agent();

static configSTACK_DEPTH_TYPE getMaxStackSize() {
    return 150;
}

unsigned int getBlinkStakHighWater() {
    if (xBlinkHandle != NULL)
        return uxTaskGetStackHighWaterMark(xBlinkHandle);
    else
        return 0;
}

void set_blink_task_handle(TaskHandle_t pHandle) {
    xBlinkHandle = pHandle;
}

TaskHandle_t get_blink_task_handle() {
    return xBlinkHandle;
}

void stop_blink_agent() {
    if (xBlinkHandle != NULL) {
        vTaskDelete(xBlinkHandle);
        xBlinkHandle = NULL;
    }
}

// we assume WiFi was already connected since we are blinking onboard LED
static void blink_agent_task(void* pvParameters) {
    printf("Blink Started\n");

    while (true) {
        cyw43_arch_gpio_put(BLINK_LED_PAD, 1);
        vTaskDelay(DELAY);  // Delay by TICKS defined by FreeRTOS priorities
        cyw43_arch_gpio_put(BLINK_LED_PAD, 0);
        vTaskDelay(DELAY);
    }
}

bool start_blink_agent(const char* name, UBaseType_t priority) {
    BaseType_t res;

    if (strlen(name) >= MAX_NAME_LEN) {
        memcpy(pBlinkTaskName, name, MAX_NAME_LEN);
        pBlinkTaskName[MAX_NAME_LEN - 1] = 0;
    } else {
        strcpy(pBlinkTaskName, name);
    }

    // Create Your Task
    res = xTaskCreate(
        blink_agent_task,   // Task to be run
        pBlinkTaskName,     // Name of the Task for debugging and managing its Task Handle
        getMaxStackSize(),  // Stack depth to be allocated for use with task's stack (see docs)
        NULL,               // Arguments needed by the Task (NULL because we don't have any)
        priority,           // Task Priority - Higher the number the more priority [max is (configMAX_PRIORITIES - 1) provided in FreeRTOSConfig.h]
        &xBlinkHandle       // Task Handle if available for managing the task
    );
    return (res == pdPASS);
}
