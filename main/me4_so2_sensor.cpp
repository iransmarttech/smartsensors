#include "me4_so2_sensor.h"
#include "config.h"
#include "shared_data.h"
#include <Arduino.h>

ME4SO2Sensor me4so2Sensor;

void ME4SO2Sensor::init() {
    analogReadResolution(ADC_RESOLUTION);
    readSensor();
    DEBUG_PRINTLN("✓ ME4-SO2 Sensor initialized");
}

void ME4SO2Sensor::readSensor() {
    float voltage = readVoltage();
    int rawValue = analogRead(ME4_SO2_PIN);
    float current_ua = (voltage / SO2_LOAD_RESISTOR) * 1000000.0;
    float so2_concentration = current_ua / SO2_SENSITIVITY;

    if (lockData()) {
        sharedData.me4so2_voltage = voltage;
        sharedData.me4so2_raw = rawValue;
        sharedData.me4so2_current = current_ua;
        sharedData.me4so2_so2 = so2_concentration;
        sharedData.me4so2_valid = true;
        sharedData.last_update = millis();
        unlockData();
    }
}

float ME4SO2Sensor::readVoltage() {
    int rawValue = analogRead(ME4_SO2_PIN);
    return rawValue * (V_REF / (1 << ADC_RESOLUTION));
}

bool ME4SO2Sensor::isDataValid() {
    bool valid = false;
    if (lockData()) {
        valid = sharedData.me4so2_valid && (millis() - sharedData.last_update < 5000);
        unlockData();
    }
    return valid;
}