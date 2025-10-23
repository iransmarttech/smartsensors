#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// Feature Configuration
#define ETHERNET_ENABLED
#define WIFI_FALLBACK_ENABLED  
#define WEB_SERVER_ENABLED
#define MDNS_ENABLED
#define ZE40_SENSOR_ENABLED
#define ZPHS01B_SENSOR_ENABLED
#define MR007_SENSOR_ENABLED
#define ME4_SO2_SENSOR_ENABLED
#define BUTTON_LED_ENABLED
#define DEBUG_SERIAL_ENABLED

// Hardware Configuration
#define ZE40_DAC_PIN 4
#define ZE40_RX_PIN 18
#define ZE40_TX_PIN 17

#define ZPHS01B_RX_PIN 8
#define ZPHS01B_TX_PIN 7

#define MR007_PIN 5
#define ME4_SO2_PIN 9

// UPDATED: LED and Relay pins
#define LED_PIN 2
#define RELAY_PIN 48
#define BUTTON_PIN 16

// Ethernet Configuration
#define ETH_CS_PIN 10
#define ETH_SCK_PIN 12
#define ETH_MISO_PIN 11
#define ETH_MOSI_PIN 13

// Network Configuration
extern const char* HOSTNAME;
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* AP_SSID;
extern const char* AP_PASS;

// Task Configuration
#define ETH_TASK_STACK_SIZE 8192
#define SENSOR_TASK_STACK_SIZE 12288
#define ETH_TASK_PRIORITY 2
#define SENSOR_TASK_PRIORITY 1

// Timing Configuration
#define DAC_READ_INTERVAL 5000
#define ZE40_REQUEST_INTERVAL 30000
#define ZPHS01B_READ_INTERVAL 5000
#define MR007_READ_INTERVAL 2000
#define ME4_SO2_READ_INTERVAL 2000
#define LED_TIMEOUT 10000  // 10 seconds timeout
// #define INITIAL_PREHEAT 0
#define SENSOR_WARMUP_TIME 18  // ADDED: 3 minutes warmup for ZPHS01B 180000 

// ZE40 Configuration
#define FRAME_TIMEOUT 150
#define DAC_ZERO_VOLTAGE 0.4
#define DAC_FULLSCALE_VOLTAGE 2.0
#define DAC_PPM_RANGE 5.0

// ZPHS01B Configuration
#define SENSOR_BAUD_RATE 9600
#define RESPONSE_LENGTH 26

// MR007 Configuration
#define ADC_RESOLUTION 12
#define V_REF 3.3

// ME4-SO2 Configuration
#define SO2_SENSITIVITY 0.8
#define SO2_LOAD_RESISTOR 10.0

// Connection Management
#define MAX_CONCURRENT_CONNECTIONS 3
#define CONNECTION_TIMEOUT_MS 5000

// Debug Configuration
#ifdef DEBUG_SERIAL_ENABLED
#define DEBUG_PRINT(x) do { if (Serial) Serial.print(x); } while(0)
#define DEBUG_PRINTLN(x) do { if (Serial) Serial.println(x); } while(0)
#define DEBUG_PRINTF(format, ...) do { if (Serial) Serial.printf(format, ##__VA_ARGS__); } while(0)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x) 
#define DEBUG_PRINTF(format, ...)
#endif

#endif