#include "config.h"
#include "ze40_sensor.h"
#include "zphs01b_sensor.h"
#include "mr007_sensor.h"    
#include "me4_so2_sensor.h"  
#include "network_manager.h"
#include "web_server.h"
#include "task_manager.h"

void setup() {
    #ifdef DEBUG_SERIAL_ENABLED
    Serial.begin(115200);
    delay(1000);
    DEBUG_PRINTLN("Air Quality Monitor");
    DEBUG_PRINTLN("ZE40 TVOC + ZPHS01B Air Quality + MR007 Combustible + ME4-SO2");
    DEBUG_PRINTLN("=============================================================");
    #endif
    
    taskManager.createTasks();
}

void loop() {
    vTaskDelete(NULL);
}