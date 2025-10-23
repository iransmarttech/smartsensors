#ifndef ZE40_SENSOR_H
#define ZE40_SENSOR_H

#include <HardwareSerial.h>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "config.h"

struct SensorData {
    // UART Data from ZE40
    float tvoc_ppb = 0.0;
    float tvoc_ppm = 0.0;
    bool uart_data_valid = false;
    
    // Analog Data from ZE40
    float dac_voltage = 0.0;
    float dac_ppm = 0.0;
    bool analog_data_valid = false;
    
    char ipAddress[16] = "0.0.0.0";
};

class ZE40Sensor {
public:
    void init();
    void processData();
    void requestReading();
    void switchToQAMode();
    void switchToInitiativeMode();
    float readDACVoltage();
    float readDACPPM(float voltage);

    SensorData getSensorData();
    void setSensorData(const SensorData& data);
    bool isPreheatComplete();
    bool isUARTDataValid();
    bool isAnalogDataValid();

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

    ZE40State ze40State;
    SensorData sensorData;
    SemaphoreHandle_t dataMutex;
};

extern ZE40Sensor ze40Sensor;

#endif