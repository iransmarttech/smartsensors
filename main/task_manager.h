#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <Arduino.h>
#include "config.h"

class TaskManager {
public:
    void createTasks();

private:
    #ifdef ETHERNET_ENABLED
    static void ethernetTask(void *pvParameters);
    #endif
    
    static void sensorTask(void *pvParameters);
    
    // Helper functions
    static void initSensors();
    static void handleNetworkFallback();
    static void readSensors(unsigned long currentTime);
    static void handleButtonAndRelay(unsigned long currentTime, bool& buttonPressed);
};

extern TaskManager taskManager;

#endif