#ifndef ZE40_SENSOR_H
#define ZE40_SENSOR_H

#include <HardwareSerial.h>
#include "config.h"

class ZE40Sensor {
public:
    void init();
    void processData();
    void requestReading();
    float readDACVoltage();
    float readDACPPM(float voltage);
    bool isPreheatComplete();

private:
    struct ZE40State {
        HardwareSerial* serial;
        uint8_t frame[9];
        uint8_t frameIndex = 0;
        uint32_t lastByteTime = 0;
        bool preheatComplete = false;
        uint32_t powerOnTime = 0;
        bool uartDataReceived = false;
        bool inQAMode = false;
    };

    bool validateChecksum();
    void parseDataFrame();
    void processByte(uint8_t byte);
    void switchToQAMode();
    void switchToInitiativeMode();

    ZE40State ze40State;
};

extern ZE40Sensor ze40Sensor;

#endif