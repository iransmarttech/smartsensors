#include "mr007_sensor.h"
#include "config.h"
#include <Arduino.h>  // ADD: Include Arduino core

MR007Sensor mr007Sensor;

void MR007Sensor::init() {
    dataMutex = xSemaphoreCreateMutex();
    
    // FIXED: Use correct ADC configuration for ESP32
    analogReadResolution(ADC_RESOLUTION);
    
    // Take initial reading
    readSensor();
    
    DEBUG_PRINTLN("MR007 Sensor Initialized");
}

void MR007Sensor::readSensor() {
    MR007Data newData;
    
    newData.voltage = readVoltage();
    newData.rawValue = analogRead(MR007_PIN);
    newData.lastUpdate = millis();
    
    // Calculate LEL concentration: (voltage / V_REF) * 100%
    newData.lel_concentration = (newData.voltage / V_REF) * 100.0;
    
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        sensorData = newData;
        dataValid = true;
        xSemaphoreGive(dataMutex);
    }
    
    DEBUG_PRINTF("MR007 - Voltage: %.3fV, Raw: %d, LEL: %.1f%%\n", 
                 newData.voltage, newData.rawValue, newData.lel_concentration);
}

float MR007Sensor::readVoltage() {
    int rawValue = analogRead(MR007_PIN);
    return rawValue * (V_REF / (1 << ADC_RESOLUTION));
}

MR007Data MR007Sensor::getSensorData() {
    MR007Data data;
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        data = sensorData;
        xSemaphoreGive(dataMutex);
    }
    return data;
}

void MR007Sensor::setSensorData(const MR007Data& data) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        sensorData = data;
        xSemaphoreGive(dataMutex);
    }
}

bool MR007Sensor::isDataValid() {
    return dataValid && (millis() - sensorData.lastUpdate < 5000);
}