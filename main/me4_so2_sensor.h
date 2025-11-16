#ifndef ME4_SO2_SENSOR_H
#define ME4_SO2_SENSOR_H

#include "config.h"

class ME4SO2Sensor {
public:
    void init();
    void readSensor();
    bool isDataValid();

private:
    float readVoltage();
};

extern ME4SO2Sensor me4so2Sensor;

#endif