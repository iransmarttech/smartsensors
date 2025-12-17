#include "task_manager.h"
#include "ze40_sensor.h"
#include "zphs01b_sensor.h"
#include "mr007_sensor.h"
#include "me4_so2_sensor.h"
#include "web_server.h"
#include "network_manager.h"
#include "django_client.h"
#include "config.h"
#include "shared_data.h"
#include <Arduino.h>

#ifdef MDNS_ENABLED
#include <ESPmDNS.h>
#endif

TaskManager taskManager;

// LED/Relay control
volatile unsigned long ledOnTime = 0;
volatile bool ledActive = false;
volatile bool relayActive = false;

void TaskManager::createTasks() {
    DEBUG_PRINTLN("Creating FreeRTOS tasks...");

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
    DEBUG_PRINTLN("✓ Ethernet task created on Core 0");
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
    DEBUG_PRINTLN("✓ Sensor task created on Core 1");
}

#ifdef ETHERNET_ENABLED
void TaskManager::ethernetTask(void *pvParameters) {
    DEBUG_PRINTLN("→ Ethernet task started on Core 0");
    
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    
    int retries = 0;
    while (!isDataReady() && retries < 10) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        retries++;
    }
    
    if (!isDataReady()) {
        DEBUG_PRINTLN("✗ FATAL: Shared data not ready");
        vTaskDelete(NULL);
        return;
    }
    
    if (networkManager.initEthernet()) {
        DEBUG_PRINTLN("✓ Ethernet initialized successfully");
        
        vTaskDelay(100 / portTICK_PERIOD_MS);
        
        if (lockData(2000)) {
            String ip = networkManager.getIPAddress();
            strncpy(sharedData.ip_address, ip.c_str(), sizeof(sharedData.ip_address)-1);
            sharedData.network_ready = true;
            sharedData.last_update = millis();
            unlockData();
            DEBUG_PRINT("✓ IP address: ");
            DEBUG_PRINTLN(sharedData.ip_address);
        }
        
        DEBUG_PRINTLN("Starting web server...");
        webServer.init();
        DEBUG_PRINTLN("✓ System ready - Web interface available");
        DEBUG_PRINTLN("=============================================================\n");
        
        unsigned long lastMaintain = 0;
        while (true) {
            webServer.handleEthernetClient();
            
            #ifdef ETHERNET_ENABLED
            if (millis() - lastMaintain > 1000) {
                Ethernet.maintain();
                lastMaintain = millis();
            }
            #endif
            
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    } else {
        DEBUG_PRINTLN("✗ Ethernet initialization failed");
    }
    
    vTaskDelete(NULL);
}
#endif

void TaskManager::sensorTask(void *pvParameters) {
    DEBUG_PRINTLN("→ Sensor task started on Core 1");
    
    initSensors();
    
    vTaskDelay(8000 / portTICK_PERIOD_MS);
    
    #ifdef ETHERNET_ENABLED
    if (!networkManager.isEthernetActive()) {
        handleNetworkFallback();
    }
    #endif
    
    unsigned long lastReadings[4] = {0};
    bool buttonPressed = false;
    
    while (true) {
        unsigned long currentTime = millis();
        
        readSensors(currentTime);
        
        #ifdef BUTTON_LED_ENABLED
        handleButtonAndRelay(currentTime, buttonPressed);
        #endif
        
        #ifdef ETHERNET_ENABLED
        if (!networkManager.isEthernetActive()) {
            webServer.handleWiFiClient();
        }
        #endif
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

void TaskManager::initSensors() {
    DEBUG_PRINTLN("Initializing sensors...");
    
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
    pinMode(LED_PIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(RELAY_PIN, LOW);
    DEBUG_PRINTLN("✓ Button/LED/Relay initialized");
    #endif
    
    #ifdef DJANGO_ENABLED
    djangoClient.init();
    djangoClient.setServerURL(DJANGO_SERVER_URL);
    DEBUG_PRINTLN("✓ Django client initialized");
    #endif
    
    DEBUG_PRINTLN("✓ All sensors initialized successfully");
}

void TaskManager::handleNetworkFallback() {
    DEBUG_PRINTLN("Starting network fallback...");
    
    #ifdef WIFI_FALLBACK_ENABLED
    // Using SensorNetworkManager methods
    if (networkManager.initWiFi()) {
        DEBUG_PRINTLN("WiFi connected successfully");
    } else {
        DEBUG_PRINTLN("WiFi failed, starting Access Point...");
        networkManager.startAccessPoint();
    }
    
    // Update IP in shared data
    if (lockData()) {
        String ip = networkManager.getIPAddress();
        strncpy(sharedData.ip_address, ip.c_str(), sizeof(sharedData.ip_address)-1);
        sharedData.network_ready = true;
        sharedData.last_update = millis();
        unlockData();
    }
    
    webServer.init();
    #endif
}

void TaskManager::readSensors(unsigned long currentTime) {
    #ifdef ZE40_SENSOR_ENABLED
    ze40Sensor.processData();
    
    // Read ZE40 analog data periodically
    static unsigned long lastZE40Analog = 0;
    if (currentTime - lastZE40Analog >= DAC_READ_INTERVAL) {
        float voltage = ze40Sensor.readDACVoltage();
        float ppm = ze40Sensor.readDACPPM(voltage);
        
        if (lockData()) {
            sharedData.ze40_dac_voltage = voltage;
            sharedData.ze40_dac_ppm = ppm;
            sharedData.ze40_analog_valid = true;
            sharedData.last_update = currentTime;
            unlockData();
        }
        lastZE40Analog = currentTime;
    }
    
    // Request ZE40 UART data periodically
    static unsigned long lastZE40Request = 0;
    if (ze40Sensor.isPreheatComplete() && (currentTime - lastZE40Request >= ZE40_REQUEST_INTERVAL)) {
        ze40Sensor.requestReading();
        lastZE40Request = currentTime;
    }
    #endif
    
    #ifdef ZPHS01B_SENSOR_ENABLED
    zphs01bSensor.processData();
    
    static unsigned long lastZPHS01BRequest = 0;
    if (currentTime - lastZPHS01BRequest >= ZPHS01B_READ_INTERVAL) {
        zphs01bSensor.requestReading();
        lastZPHS01BRequest = currentTime;
    }
    #endif
    
    #ifdef MR007_SENSOR_ENABLED
    static unsigned long lastMR007Read = 0;
    if (currentTime - lastMR007Read >= MR007_READ_INTERVAL) {
        mr007Sensor.readSensor();
        lastMR007Read = currentTime;
    }
    #endif
    
    #ifdef ME4_SO2_SENSOR_ENABLED
    static unsigned long lastME4SO2Read = 0;
    if (currentTime - lastME4SO2Read >= ME4_SO2_READ_INTERVAL) {
        me4so2Sensor.readSensor();
        lastME4SO2Read = currentTime;
    }
    #endif
    
    // Add small delay before sending to Django to avoid network contention
    // with the Ethernet server on the other core
    vTaskDelay(pdMS_TO_TICKS(50));
    
    // Send data to Django server
    #ifdef DJANGO_ENABLED
    djangoClient.sendSensorData();
    #endif
}

void TaskManager::handleButtonAndRelay(unsigned long currentTime, bool& buttonPressed) {
    if (digitalRead(BUTTON_PIN) == LOW && !buttonPressed) {
        digitalWrite(LED_PIN, HIGH);
        digitalWrite(RELAY_PIN, HIGH);
        ledActive = true;
        relayActive = true;
        ledOnTime = currentTime;
        buttonPressed = true;
        DEBUG_PRINTLN("⚡ Relay activated");
    } 
    else if (digitalRead(BUTTON_PIN) == HIGH && buttonPressed) {
        buttonPressed = false;
    }
    
    if ((ledActive || relayActive) && (currentTime - ledOnTime >= LED_TIMEOUT)) {
        digitalWrite(LED_PIN, LOW);
        digitalWrite(RELAY_PIN, LOW);
        ledActive = false;
        relayActive = false;
        DEBUG_PRINTLN("⏹ Relay deactivated");
    }
}