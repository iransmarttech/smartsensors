#ifndef ZPHS01B_SENSOR_H
#define ZPHS01B_SENSOR_H

#include <HardwareSerial.h>
#include "config.h"

class ZPHS01BSensor {
public:
    void init();
    void processData();
    void requestReading();
    bool isDataValid();

private:
    bool validateChecksum(const uint8_t* response, size_t length);
    void processSensorData(const uint8_t* response);

    HardwareSerial* sensorSerial;
    uint32_t warmUpStart = 0;
    bool dataValid = false;
};

extern ZPHS01BSensor zphs01bSensor;

#endif