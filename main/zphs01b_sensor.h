#ifndef ZPHS01B_SENSOR_H
#define ZPHS01B_SENSOR_H

#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "config.h"

struct AirQualityData {
    // Particulate Matter
    uint16_t pm1;
    uint16_t pm25;
    uint16_t pm10;
    
    // Gases
    uint16_t co2;
    uint8_t voc;
    uint16_t ch2o;
    double co;
    double o3;
    double no2;
    
    // Environmental
    double temperature;
    uint16_t humidity;
    
    uint32_t lastUpdate;
};

class ZPHS01BSensor {
public:
    void init();
    void processData();
    void requestReading();
    
    AirQualityData getSensorData();
    void setSensorData(const AirQualityData& data);
    bool isDataValid();

private:
    bool validateChecksum(const uint8_t* response, size_t length);
    void processSensorData(const uint8_t* response, AirQualityData* output);

    HardwareSerial* sensorSerial;
    AirQualityData sensorData;
    SemaphoreHandle_t dataMutex;
    bool dataValid = false;
    uint32_t warmUpStart = 0;
};

extern ZPHS01BSensor zphs01bSensor;

#endif