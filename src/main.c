
#include <FreeRTOS.h>
#include <lwip/dns.h>
#include <lwip/ip4_addr.h>
#include <lwip/sockets.h>
#include <pico/cyw43_arch.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <task.h>

#include "WiFiHelper.h"

// Check these definitions where added from the makefile
#ifndef WIFI_SSID
#error "WIFI_SSID not defined"
#endif
#ifndef WIFI_PASSWORD
#error "WIFI_PASSWORD not defined"
#endif

// LED PAD defintions
#define PULSE_LED 0

#define TASK_PRIORITY (tskIDLE_PRIORITY + 1UL)

void runTimeStats();

void main_task(void *pvParameters);

int main() {
    stdio_init_all();  // Initialize

    // Create Your Task
    xTaskCreate(
        main_task,   // Task to be run
        "LED_TASK",  // Name of the Task for debugging and managing its Task Handle
        256,         // Stack depth to be allocated for use with task's stack (see docs)
        NULL,        // Arguments needed by the Task (NULL because we don't have any)
        1,           // Task Priority - Higher the number the more priority [max is (configMAX_PRIORITIES - 1) provided in FreeRTOSConfig.h]
        NULL         // Task Handle if available for managing the task
    );

    // Should start you scheduled Tasks (such as the LED_Task above)
    vTaskStartScheduler();

    while (true) {
        // Your program should never get here
    };

    return 0;
}

void main_task(void *pvParameters) {
    printf("Main task started\n");

    if (wifi_init()) {
        printf("Wifi Controller Initialised\n");
    } else {
        printf("Failed to initialise controller\n");
        return;
    }

    printf("Connecting to WiFi... %s \n", WIFI_SSID);

    if (wifi_join(WIFI_SSID, WIFI_PASSWORD, WIFI_RETRIES)) {
        printf("Connect to Wifi\n");
    } else {
        printf("Failed to connect to Wifi \n");
    }

    // Print MAC Address
    char macStr[20];
    wifi_getMACAddressStr(macStr);
    printf("MAC ADDRESS: %s\n", macStr);

    // Print IP Address
    char ipStr[20];
    wifi_getIPAddressStr(ipStr);
    printf("IP ADDRESS: %s\n", ipStr);

    // Print Net Mask
    wifi_getNetMaskStr(ipStr);
    printf("Net Mask: %s\n", ipStr);

    // Print Gateway
    wifi_getGWAddressStr(ipStr);
    printf("Gateway: %s\n", ipStr);

    while (true) {
        runTimeStats();

        vTaskDelay(3000);

        if (!wifi_isJoined()) {
            printf("AP Link is down\n");

            if (wifi_join(WIFI_SSID, WIFI_PASSWORD, WIFI_RETRIES)) {
                printf("Connect to Wifi\n");
            } else {
                printf("Failed to connect to Wifi \n");
            }
        }
    }
}

void runTimeStats() {
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    unsigned long ulTotalRunTime;

    /* Take a snapshot of the number of tasks in case it changes while this
    function is executing. */
    uxArraySize = uxTaskGetNumberOfTasks();
    printf("Number of tasks %d\n", uxArraySize);

    /* Allocate a TaskStatus_t structure for each task.  An array could be
    allocated statically at compile time. */
    pxTaskStatusArray = (TaskStatus_t *)pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL) {
        /* Generate raw status information about each task. */
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
                                           uxArraySize,
                                           &ulTotalRunTime);

        /* For each populated position in the pxTaskStatusArray array,
        format the raw data as human readable ASCII data. */
        for (x = 0; x < uxArraySize; x++) {
            printf("Task: %d \t cPri:%d \t bPri:%d \t hw:%d \t%s\n",
                   pxTaskStatusArray[x].xTaskNumber,
                   pxTaskStatusArray[x].uxCurrentPriority,
                   pxTaskStatusArray[x].uxBasePriority,
                   pxTaskStatusArray[x].usStackHighWaterMark,
                   pxTaskStatusArray[x].pcTaskName);
        }

        /* The array is no longer needed, free the memory it consumes. */
        vPortFree(pxTaskStatusArray);
    } else {
        printf("Failed to allocate space for stats\n");
    }

    HeapStats_t heapStats;
    vPortGetHeapStats(&heapStats);
    printf("HEAP avl: %d, blocks %d, alloc: %d, free: %d\n",
           heapStats.xAvailableHeapSpaceInBytes,
           heapStats.xNumberOfFreeBlocks,
           heapStats.xNumberOfSuccessfulAllocations,
           heapStats.xNumberOfSuccessfulFrees);
}
