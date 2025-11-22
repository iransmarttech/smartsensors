#ifndef SHARED_DATA_H
#define SHARED_DATA_H

#include <Arduino.h>
#include "config.h"
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

// Forward declarations
class DjangoClient;

// Unified sensor data structure
struct SharedSensorData {
    // ZE40 Sensor
    float ze40_tvoc_ppb = 0.0;
    float ze40_tvoc_ppm = 0.0;
    float ze40_dac_voltage = 0.0;
    float ze40_dac_ppm = 0.0;
    bool ze40_uart_valid = false;
    bool ze40_analog_valid = false;
    bool ze40_preheat_complete = false;

    // ZPHS01B Sensor
    float zphs01b_pm1 = 0.0;
    float zphs01b_pm25 = 0.0;
    float zphs01b_pm10 = 0.0;
    float zphs01b_co2 = 0.0;
    float zphs01b_voc = 0.0;
    float zphs01b_ch2o = 0.0;
    float zphs01b_co = 0.0;
    float zphs01b_o3 = 0.0;
    float zphs01b_no2 = 0.0;
    float zphs01b_temperature = 0.0;
    float zphs01b_humidity = 0.0;
    bool zphs01b_valid = false;

    // MR007 Sensor
    float mr007_voltage = 0.0;
    int mr007_raw = 0;
    float mr007_lel = 0.0;
    bool mr007_valid = false;

    // ME4-SO2 Sensor
    float me4so2_voltage = 0.0;
    int me4so2_raw = 0;
    float me4so2_current = 0.0;
    float me4so2_so2 = 0.0;
    bool me4so2_valid = false;

    // Network status
    char ip_address[16] = "0.0.0.0";
    bool network_ready = false;
    unsigned long last_update = 0;
};

extern SharedSensorData sharedData;
extern SemaphoreHandle_t dataMutex;

#ifdef DJANGO_ENABLED
#include "django_client.h"
extern DjangoClient djangoClient;
#endif

// Thread-safe data access functions
void initSharedData();
bool lockData(int timeout_ms = 5000);
void unlockData();
bool isDataReady();

#endif