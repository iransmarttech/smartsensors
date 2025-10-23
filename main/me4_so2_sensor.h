#ifndef ME4_SO2_SENSOR_H
#define ME4_SO2_SENSOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "config.h"

struct ME4SO2Data {
    float voltage = 0.0;
    int rawValue = 0;
    float so2_concentration = 0.0;  // SO2 concentration in ppm
    float current_ua = 0.0;         // Current in microamperes
    uint32_t lastUpdate = 0;
};

class ME4SO2Sensor {
public:
    void init();
    void readSensor();
    ME4SO2Data getSensorData();
    void setSensorData(const ME4SO2Data& data);
    bool isDataValid();

private:
    float readVoltage();
    void calculateConcentration();

    ME4SO2Data sensorData;
    SemaphoreHandle_t dataMutex;
    bool dataValid = false;
};

extern ME4SO2Sensor me4so2Sensor;

#endif