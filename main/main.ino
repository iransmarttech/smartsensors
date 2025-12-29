#include "config.h"
#include "shared_data.h"
#include "task_manager.h"

void setup() {
    // Initialize serial with sufficient delay
    Serial.begin(115200);
    delay(3000);
    
    DEBUG_PRINTLN("Air Quality Monitor");
    DEBUG_PRINTLN("=============================================================");

    // Initialize shared data FIRST - this creates the mutex
    initSharedData();
    
    // Wait for mutex initialization to complete
    delay(1000);
    
    // Verify mutex is ready
    if (!isDataReady()) {
        DEBUG_PRINTLN("FATAL ERROR: Shared data initialization failed!");
        while(1) {
            delay(1000);
        }
    }
    
    // Create tasks - this starts the system
    taskManager.createTasks();
}

void loop() {
    // All work is done in FreeRTOS tasks
    vTaskDelete(NULL);
}