#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include "config.h"

class TaskManager {
public:
    void createTasks();

    #ifdef ETHERNET_ENABLED
    static void ethernetTask(void *pvParameters);
    #endif

    static void sensorTask(void *pvParameters);

private:
    void handleButtonLED();
};

extern TaskManager taskManager;

#endif