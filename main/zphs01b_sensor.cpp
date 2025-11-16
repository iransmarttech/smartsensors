#include "zphs01b_sensor.h"
#include "config.h"
#include "shared_data.h"
#include <Arduino.h>

ZPHS01BSensor zphs01bSensor;

const uint8_t REQUEST_DATA_CMD[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

void ZPHS01BSensor::init() {
    sensorSerial = new HardwareSerial(2);
    sensorSerial->begin(SENSOR_BAUD_RATE, SERIAL_8N1, ZPHS01B_RX_PIN, ZPHS01B_TX_PIN);
    warmUpStart = millis();
    DEBUG_PRINTLN("✓ ZPHS01B Air Quality Sensor initialized");
}

void ZPHS01BSensor::processData() {
    if (millis() - warmUpStart < SENSOR_WARMUP_TIME) {
        return;
    }
    
    while (sensorSerial->available()) {
        uint8_t rawData[26] = {0};
        size_t bytesRead = sensorSerial->readBytes(rawData, 26);
        
        if (bytesRead == 26 && validateChecksum(rawData, bytesRead)) {
            processSensorData(rawData);
            dataValid = true;
        }
    }
}

void ZPHS01BSensor::requestReading() {
    if (millis() - warmUpStart < SENSOR_WARMUP_TIME) {
        return;
    }
    sensorSerial->write(REQUEST_DATA_CMD, sizeof(REQUEST_DATA_CMD));
}

bool ZPHS01BSensor::isDataValid() {
    bool valid = false;
    if (lockData()) {
        valid = sharedData.zphs01b_valid && (millis() - sharedData.last_update < 10000);
        unlockData();
    }
    return valid;
}

bool ZPHS01BSensor::validateChecksum(const uint8_t* response, size_t length) {
    if (length != 26) return false;

    uint16_t calculatedSum = 0;
    for (uint8_t i = 1; i < 25; i++) {
        calculatedSum += response[i];
    }
    uint8_t checksum = (~static_cast<uint8_t>(calculatedSum)) + 1;
    return (checksum == response[25]);
}

void ZPHS01BSensor::processSensorData(const uint8_t* data) {
    if (lockData()) {
        // Particulate Matter
        sharedData.zphs01b_pm1 = (data[2] << 8) | data[3];
        sharedData.zphs01b_pm25 = (data[4] << 8) | data[5];
        sharedData.zphs01b_pm10 = (data[6] << 8) | data[7];

        // Gases
        sharedData.zphs01b_co2 = (data[8] << 8) | data[9];
        sharedData.zphs01b_voc = data[10];

        // Environmental
        sharedData.zphs01b_temperature = ((data[11] << 8 | data[12]) - 500) * 0.1;
        sharedData.zphs01b_humidity = (data[13] << 8) | data[14];
        
        // Additional gases
        sharedData.zphs01b_ch2o = (data[15] << 8) | data[16];
        sharedData.zphs01b_co = (data[17] << 8 | data[18]) * 0.1;
        sharedData.zphs01b_o3 = (data[19] << 8 | data[20]) * 0.01;
        sharedData.zphs01b_no2 = (data[21] << 8 | data[22]) * 0.01;
        
        sharedData.zphs01b_valid = true;
        sharedData.last_update = millis();
        unlockData();
    }
}