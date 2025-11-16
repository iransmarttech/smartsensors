#include "mr007_sensor.h"
#include "config.h"
#include "shared_data.h"
#include <Arduino.h>

MR007Sensor mr007Sensor;

void MR007Sensor::init() {
    analogReadResolution(ADC_RESOLUTION);
    readSensor();
    DEBUG_PRINTLN("✓ MR007 Combustible Gas Sensor initialized");
}

void MR007Sensor::readSensor() {
    float voltage = readVoltage();
    int rawValue = analogRead(MR007_PIN);
    float lel_concentration = (voltage / V_REF) * 100.0;

    if (lockData()) {
        sharedData.mr007_voltage = voltage;
        sharedData.mr007_raw = rawValue;
        sharedData.mr007_lel = lel_concentration;
        sharedData.mr007_valid = true;
        sharedData.last_update = millis();
        unlockData();
    }
}

float MR007Sensor::readVoltage() {
    int rawValue = analogRead(MR007_PIN);
    return rawValue * (V_REF / (1 << ADC_RESOLUTION));
}

bool MR007Sensor::isDataValid() {
    bool valid = false;
    if (lockData()) {
        valid = sharedData.mr007_valid && (millis() - sharedData.last_update < 5000);
        unlockData();
    }
    return valid;
}