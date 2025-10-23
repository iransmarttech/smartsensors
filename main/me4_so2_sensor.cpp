#include "me4_so2_sensor.h"
#include "config.h"
#include <Arduino.h>  // ADD: Include Arduino core

ME4SO2Sensor me4so2Sensor;

void ME4SO2Sensor::init() {
    dataMutex = xSemaphoreCreateMutex();
    
    // FIXED: Use correct ADC configuration for ESP32
    analogReadResolution(ADC_RESOLUTION);
    
    // Take initial reading
    readSensor();
    
    DEBUG_PRINTLN("ME4-SO2 Sensor Initialized");
}

void ME4SO2Sensor::readSensor() {
    ME4SO2Data newData;
    
    newData.voltage = readVoltage();
    newData.rawValue = analogRead(ME4_SO2_PIN);
    newData.lastUpdate = millis();
    
    // Calculate current in microamperes: (voltage / load resistor) * 1,000,000
    newData.current_ua = (newData.voltage / SO2_LOAD_RESISTOR) * 1000000.0;
    
    // Calculate SO2 concentration: current (uA) / sensitivity
    newData.so2_concentration = newData.current_ua / SO2_SENSITIVITY;
    
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        sensorData = newData;
        dataValid = true;
        xSemaphoreGive(dataMutex);
    }
    
    DEBUG_PRINTF("ME4-SO2 - Voltage: %.4fV, Raw: %d, Current: %.2fuA, SO2: %.2fppm\n", 
                 newData.voltage, newData.rawValue, newData.current_ua, newData.so2_concentration);
}

float ME4SO2Sensor::readVoltage() {
    int rawValue = analogRead(ME4_SO2_PIN);
    return rawValue * (V_REF / (1 << ADC_RESOLUTION));
}

ME4SO2Data ME4SO2Sensor::getSensorData() {
    ME4SO2Data data;
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        data = sensorData;
        xSemaphoreGive(dataMutex);
    }
    return data;
}

void ME4SO2Sensor::setSensorData(const ME4SO2Data& data) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        sensorData = data;
        xSemaphoreGive(dataMutex);
    }
}

bool ME4SO2Sensor::isDataValid() {
    return dataValid && (millis() - sensorData.lastUpdate < 5000);
}