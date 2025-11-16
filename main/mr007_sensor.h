#ifndef MR007_SENSOR_H
#define MR007_SENSOR_H

#include "config.h"

class MR007Sensor {
public:
    void init();
    void readSensor();
    bool isDataValid();

private:
    float readVoltage();
};

extern MR007Sensor mr007Sensor;

#endif