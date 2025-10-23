#ifndef MR007_SENSOR_H
#define MR007_SENSOR_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "config.h"

struct MR007Data {
    float voltage = 0.0;
    int rawValue = 0;
    float lel_concentration = 0.0;  // Lower Explosive Limit percentage
    uint32_t lastUpdate = 0;
};

class MR007Sensor {
public:
    void init();
    void readSensor();
    MR007Data getSensorData();
    void setSensorData(const MR007Data& data);
    bool isDataValid();

private:
    float readVoltage();
    void calculateConcentration();

    MR007Data sensorData;
    SemaphoreHandle_t dataMutex;
    bool dataValid = false;
};

extern MR007Sensor mr007Sensor;

#endif