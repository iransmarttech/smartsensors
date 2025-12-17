#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
// SECURITY: Import credentials from separate file
// ============================================================================
// credentials.h is NOT committed to git (in .gitignore)
// Use credentials_template.h as a template
#include "credentials.h"

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

#define LED_PIN 2
#define RELAY_PIN 48
#define BUTTON_PIN 16

// Ethernet Configuration
#define ETH_CS_PIN 10
#define ETH_SCK_PIN 12
#define ETH_MISO_PIN 11
#define ETH_MOSI_PIN 13

// Network Configuration
// Credentials are now imported from credentials.h (not in git)
// AP SSID will be generated dynamically with device MAC address

// Web Authentication
extern const char* WEB_ADMIN_USERNAME;
extern const char* WEB_ADMIN_PASSWORD;
extern const char* API_ACCESS_TOKEN;

// Django Backend Configuration
#define DJANGO_ENABLED
extern const char* DJANGO_SERVER_URL;  // e.g., "http://192.168.1.100:8000/api/sensors"

// Task Configuration - Optimized for ESP32-S3
// Increased stack sizes to prevent mutex assertion failures during concurrent network ops
#define ETH_TASK_STACK_SIZE 32768  // Increased from 20480 for HTTP client stability
#define SENSOR_TASK_STACK_SIZE 16384
#define SENSOR_TASK_STACK_SIZE 20480
#define ETH_TASK_PRIORITY 2
#define SENSOR_TASK_PRIORITY 1

// Timing Configuration
#define DAC_READ_INTERVAL 5000
#define ZE40_REQUEST_INTERVAL 30000
#define ZPHS01B_READ_INTERVAL 5000
#define MR007_READ_INTERVAL 2000
#define ME4_SO2_READ_INTERVAL 2000
#define SENSOR_WARMUP_TIME 180000

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
#define LED_TIMEOUT 5000

// Debug Configuration
#ifdef DEBUG_SERIAL_ENABLED
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(format, ...)
#endif

#endif