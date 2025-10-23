#include "shared_data.h"
#include <Arduino.h>

SharedSensorData sharedData;
SemaphoreHandle_t dataMutex;

void initSharedData() {
    // Initialize ZE40 data
    sharedData.ze40_tvoc_ppb = 0.0;
    sharedData.ze40_tvoc_ppm = 0.0;
    sharedData.ze40_dac_voltage = 0.0;
    sharedData.ze40_dac_ppm = 0.0;
    sharedData.ze40_uart_data_valid = false;
    sharedData.ze40_analog_data_valid = false;

    // Initialize ZPHS01B data
    sharedData.zphs01b_pm1 = 0.0;
    sharedData.zphs01b_pm25 = 0.0;
    sharedData.zphs01b_pm10 = 0.0;
    sharedData.zphs01b_co2 = 0.0;
    sharedData.zphs01b_voc = 0.0;
    sharedData.zphs01b_ch2o = 0.0;
    sharedData.zphs01b_co = 0.0;
    sharedData.zphs01b_o3 = 0.0;
    sharedData.zphs01b_no2 = 0.0;
    sharedData.zphs01b_temperature = 0.0;
    sharedData.zphs01b_humidity = 0.0;
    sharedData.zphs01b_data_valid = false;

    // Initialize MR007 data
    sharedData.mr007_voltage = 0.0;
    sharedData.mr007_rawValue = 0;
    sharedData.mr007_lel_concentration = 0.0;
    sharedData.mr007_data_valid = false;

    // Initialize ME4-SO2 data
    sharedData.me4so2_voltage = 0.0;
    sharedData.me4so2_rawValue = 0;
    sharedData.me4so2_current_ua = 0.0;
    sharedData.me4so2_so2_concentration = 0.0;
    sharedData.me4so2_data_valid = false;

    // Initialize network data
    strcpy(sharedData.ip_address, "0.0.0.0");
    sharedData.network_ready = false;
    sharedData.last_update = 0;

    // Create mutex
    dataMutex = xSemaphoreCreateMutex();
    if (dataMutex == NULL) {
        DEBUG_PRINTLN("ERROR: Failed to create data mutex");
    }
}

bool takeDataMutex(TickType_t timeout) {
    return (xSemaphoreTake(dataMutex, timeout) == pdTRUE);
}

void giveDataMutex() {
    xSemaphoreGive(dataMutex);
}