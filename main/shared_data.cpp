#include "shared_data.h"
#include <Arduino.h>

SharedSensorData sharedData;
SemaphoreHandle_t dataMutex = NULL;
bool dataInitialized = false;

// Static storage for mutex to prevent heap issues
static StaticSemaphore_t mutexBuffer;

void initSharedData() {
    if (dataInitialized) return;

    memset(&sharedData, 0, sizeof(SharedSensorData));
    strcpy(sharedData.ip_address, "0.0.0.0");
    sharedData.last_update = millis();

    if (dataMutex == NULL) {
        dataMutex = xSemaphoreCreateMutexStatic(&mutexBuffer);
        if (dataMutex != NULL) {
            dataInitialized = true;
            DEBUG_PRINTLN("✓ Shared data initialized");
            vTaskDelay(pdMS_TO_TICKS(100));
        } else {
            DEBUG_PRINTLN("✗ ERROR: Failed to create data mutex");
        }
    }
}

bool lockData(int timeout_ms) {
    if (!dataInitialized || dataMutex == NULL) {
        DEBUG_PRINTLN("ERROR: Mutex not initialized in lockData");
        return false;
    }
    
    // Prevent ISR context access
    if (xPortInIsrContext()) {
        DEBUG_PRINTLN("ERROR: Cannot lock mutex from ISR context");
        return false;
    }
    
    TickType_t timeout = pdMS_TO_TICKS(timeout_ms);
    BaseType_t result = xSemaphoreTake(dataMutex, timeout);
    
    if (result != pdTRUE) {
        DEBUG_PRINTLN("WARNING: Failed to acquire mutex lock");
    }
    
    return (result == pdTRUE);
}

void unlockData() {
    if (!dataInitialized || dataMutex == NULL) {
        DEBUG_PRINTLN("ERROR: Mutex not initialized in unlockData");
        return;
    }
    
    // Prevent ISR context access
    if (xPortInIsrContext()) {
        DEBUG_PRINTLN("ERROR: Cannot unlock mutex from ISR context");
        return;
    }
    
    xSemaphoreGive(dataMutex);
}

bool isDataReady() {
    return dataInitialized;
}