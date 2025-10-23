#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <Arduino.h>
#include "config.h"

struct SharedSensorData {
    // ZE40 Sensor Data
    float ze40_tvoc_ppb;
    float ze40_tvoc_ppm;
    float ze40_dac_voltage;
    float ze40_dac_ppm;
    bool ze40_uart_data_valid;
    bool ze40_analog_data_valid;

    // ZPHS01B Sensor Data
    float zphs01b_pm1;
    float zphs01b_pm25;
    float zphs01b_pm10;
    float zphs01b_co2;
    float zphs01b_voc;
    float zphs01b_ch2o;
    float zphs01b_co;
    float zphs01b_o3;
    float zphs01b_no2;
    float zphs01b_temperature;
    float zphs01b_humidity;
    bool zphs01b_data_valid;

    // MR007 Sensor Data
    float mr007_voltage;
    int mr007_rawValue;
    float mr007_lel_concentration;
    bool mr007_data_valid;

    // ME4-SO2 Sensor Data
    float me4so2_voltage;
    int me4so2_rawValue;
    float me4so2_current_ua;
    float me4so2_so2_concentration;
    bool me4so2_data_valid;

    // Network Information
    char ip_address[16];
    bool network_ready;
    unsigned long last_update;
};

extern SharedSensorData sharedData;
extern SemaphoreHandle_t dataMutex;

void initSharedData();
bool takeDataMutex(TickType_t timeout = portMAX_DELAY);
void giveDataMutex();

#endif