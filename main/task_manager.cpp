#include "task_manager.h"
#include "ze40_sensor.h"
#include "zphs01b_sensor.h"
#include "mr007_sensor.h"
#include "me4_so2_sensor.h"
#include "web_server.h"
#include "network_manager.h"
#include "config.h"
#include <Arduino.h>

TaskManager taskManager;

// UPDATED: Add relay control variables
volatile unsigned long ledOnTime = 0;
volatile bool ledActive = false;
volatile bool relayActive = false;
SemaphoreHandle_t dataMutex;

void TaskManager::createTasks() {
    dataMutex = xSemaphoreCreateMutex();

    #ifdef ETHERNET_ENABLED
    xTaskCreatePinnedToCore(
        ethernetTask,
        "Ethernet_Task",
        ETH_TASK_STACK_SIZE,
        NULL,
        ETH_TASK_PRIORITY,
        NULL,
        0
    );
    #endif

    xTaskCreatePinnedToCore(
        sensorTask,
        "Sensor_Task", 
        SENSOR_TASK_STACK_SIZE,
        NULL,
        SENSOR_TASK_PRIORITY,
        NULL,
        1
    );
    
    DEBUG_PRINTLN("Tasks created successfully");
}

#ifdef ETHERNET_ENABLED
void TaskManager::ethernetTask(void *pvParameters) {
    DEBUG_PRINTLN("Ethernet Task started on Core 0");
    
    if (networkManager.initEthernet()) {
        DEBUG_PRINTLN("Ethernet connected successfully");
        
        webServer.init();
        
        while (true) {
            webServer.handleEthernetClient();
            
            #ifdef ETHERNET_ENABLED
            Ethernet.maintain();
            #endif
            
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    } else {
        DEBUG_PRINTLN("Ethernet initialization failed");
    }
    
    vTaskDelete(NULL);
}
#endif

void TaskManager::sensorTask(void *pvParameters) {
    DEBUG_PRINTLN("Sensor Task started on Core 1");
    
    #ifdef ZE40_SENSOR_ENABLED
    ze40Sensor.init();
    #endif
    
    #ifdef ZPHS01B_SENSOR_ENABLED
    zphs01bSensor.init();
    #endif
    
    #ifdef MR007_SENSOR_ENABLED
    mr007Sensor.init();
    #endif
    
    #ifdef ME4_SO2_SENSOR_ENABLED
    me4so2Sensor.init();
    #endif
    
    #ifdef BUTTON_LED_ENABLED
    // UPDATED: Initialize LED, Relay, and Button pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    
    // Ensure both are off initially
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    DEBUG_PRINTLN("Button/LED/Relay initialized - Pin 16:Button, Pin 2:LED, Pin 48:Relay");
    #endif
    
    DEBUG_PRINTLN("Waiting for Ethernet outcome...");
    vTaskDelay(11000 / portTICK_PERIOD_MS);
    
    if (!networkManager.isEthernetActive()) {
        DEBUG_PRINTLN("Ethernet failed, starting WiFi fallback...");
        
        #ifdef WIFI_FALLBACK_ENABLED
        if (networkManager.initWiFi()) {
            DEBUG_PRINTLN("WiFi connected successfully");
        } else {
            DEBUG_PRINTLN("WiFi failed, starting Access Point...");
            networkManager.startAccessPoint();
        }
        
        webServer.init();
        #endif
    } else {
        DEBUG_PRINTLN("Ethernet is active, skipping WiFi/AP");
    }
    
    unsigned long lastDACRead = 0;
    unsigned long lastZE40Request = 0;
    unsigned long lastZPHS01BRequest = 0;
    unsigned long lastMR007Read = 0;
    unsigned long lastME4SO2Read = 0;
    bool buttonPressed = false;
    
    while (true) {
        unsigned long currentTime = millis();
        
        #ifdef ZE40_SENSOR_ENABLED
        // Process UART data from ZE40
        ze40Sensor.processData();
        
        // Read analog data from ZE40
        if (currentTime - lastDACRead >= DAC_READ_INTERVAL) {
            float voltage = ze40Sensor.readDACVoltage();
            float ppm = ze40Sensor.readDACPPM(voltage);
            
            SensorData data = ze40Sensor.getSensorData();
            data.dac_voltage = voltage;
            data.dac_ppm = ppm;
            data.analog_data_valid = true;
            ze40Sensor.setSensorData(data);
            
            DEBUG_PRINTF("ZE40 Analog - Voltage: %.2fV, PPM: %.3f\n", voltage, ppm);
            
            lastDACRead = currentTime;
        }
        
        // Request UART data from ZE40
        if (ze40Sensor.isPreheatComplete() && (currentTime - lastZE40Request >= ZE40_REQUEST_INTERVAL)) {
            ze40Sensor.requestReading();
            lastZE40Request = currentTime;
        }
        #endif
        
        #ifdef ZPHS01B_SENSOR_ENABLED
        zphs01bSensor.processData();
        
        if (currentTime - lastZPHS01BRequest >= ZPHS01B_READ_INTERVAL) {
            zphs01bSensor.requestReading();
            lastZPHS01BRequest = currentTime;
        }
        #endif
        
        #ifdef MR007_SENSOR_ENABLED
        if (currentTime - lastMR007Read >= MR007_READ_INTERVAL) {
            mr007Sensor.readSensor();
            lastMR007Read = currentTime;
        }
        #endif
        
        #ifdef ME4_SO2_SENSOR_ENABLED
        if (currentTime - lastME4SO2Read >= ME4_SO2_READ_INTERVAL) {
            me4so2Sensor.readSensor();
            lastME4SO2Read = currentTime;
        }
        #endif
        
        #ifdef BUTTON_LED_ENABLED
        // UPDATED: Button handling for LED and Relay
        if (digitalRead(BUTTON_PIN) == LOW && !buttonPressed) {
            // Button pressed - turn on both LED and Relay
            digitalWrite(LED_PIN, HIGH);
            digitalWrite(RELAY_PIN, HIGH);
            ledActive = true;
            relayActive = true;
            ledOnTime = currentTime;
            buttonPressed = true;
            DEBUG_PRINTLN("Button pressed - LED and Relay ON");
        } 
        else if (digitalRead(BUTTON_PIN) == HIGH && buttonPressed) {
            buttonPressed = false;
        }
        
        // Check if timeout reached for LED and Relay
        if ((ledActive || relayActive) && (currentTime - ledOnTime >= LED_TIMEOUT)) {
            digitalWrite(LED_PIN, LOW);
            digitalWrite(RELAY_PIN, LOW);
            ledActive = false;
            relayActive = false;
            DEBUG_PRINTLN("LED and Relay OFF - Timeout reached");
        }
        #endif
        
        // Handle WiFi/AP clients only if Ethernet is not active
        if (!networkManager.isEthernetActive()) {
            webServer.handleWiFiClient();
        }
        
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
}