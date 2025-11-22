/*
 * ESP32 Configuration File
 * =========================
 * 
 * Central configuration for ESP32 Smart Sensors device.
 * Copy this file to config.h and modify the settings.
 * 
 * IMPORTANT: This is a template. Create config.h from this file.
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================

// Network Mode Selection
// Options: "ethernet", "wifi", "ap"
#define NETWORK_MODE "ethernet"  // Default: Ethernet

// Ethernet Configuration
#define ETHERNET_CS_PIN 5        // Chip select pin for W5500
#define ETHERNET_RESET_PIN 17    // Reset pin for W5500 (optional, -1 to disable)
#define ETHERNET_DHCP_TIMEOUT 30000  // DHCP timeout in milliseconds

// Static IP Configuration (used if DHCP fails)
#define ETHERNET_USE_STATIC_IP false  // Set to true to use static IP
#define ETHERNET_STATIC_IP "192.168.1.150"
#define ETHERNET_GATEWAY "192.168.1.1"
#define ETHERNET_SUBNET "255.255.255.0"
#define ETHERNET_DNS "8.8.8.8"

// WiFi Configuration (used when NETWORK_MODE = "wifi")
#define WIFI_SSID "YourWiFiSSID"
#define WIFI_PASSWORD "YourWiFiPassword"
#define WIFI_CONNECT_TIMEOUT 30000  // WiFi connection timeout in milliseconds

// WiFi Static IP (optional)
#define WIFI_USE_STATIC_IP false
#define WIFI_STATIC_IP "192.168.1.151"
#define WIFI_GATEWAY "192.168.1.1"
#define WIFI_SUBNET "255.255.255.0"
#define WIFI_DNS "8.8.8.8"

// Access Point Configuration (used when NETWORK_MODE = "ap")
#define AP_SSID "SmartSensors_AP"
#define AP_PASSWORD "12345678"  // Minimum 8 characters
#define AP_LOCAL_IP "192.168.4.1"
#define AP_GATEWAY "192.168.4.1"
#define AP_SUBNET "255.255.255.0"


// ============================================================================
// DJANGO BACKEND CONFIGURATION
// ============================================================================

// Django Server Settings
// IMPORTANT: Update this to your Django server's IP address
#define DJANGO_SERVER_IP "192.168.1.100"  // Change to your server IP
#define DJANGO_SERVER_PORT 8000

// Alternative configurations (uncomment as needed):
// #define DJANGO_SERVER_IP "localhost"      // Local development
// #define DJANGO_SERVER_IP "192.168.1.100"  // Local network
// #define DJANGO_SERVER_IP "your-domain.com"  // Production domain

// Django API Endpoint
#define DJANGO_API_ENDPOINT "/api/sensors"

// Full Django URL (constructed automatically)
#define DJANGO_URL "http://" DJANGO_SERVER_IP ":" STRINGIFY(DJANGO_SERVER_PORT) DJANGO_API_ENDPOINT

// HTTP Configuration
#define HTTP_TIMEOUT 10000  // HTTP request timeout in milliseconds
#define HTTP_MAX_RETRIES 3  // Number of retry attempts


// ============================================================================
// SENSOR CONFIGURATION
// ============================================================================

// ZPHS01B Particulate Matter Sensor
#define ZPHS01B_ENABLED true
#define ZPHS01B_RX_PIN 16
#define ZPHS01B_TX_PIN 17

// MR007 Combustible Gas Sensor
#define MR007_ENABLED true
#define MR007_ANALOG_PIN 34
#define MR007_ADC_RESOLUTION 4095  // 12-bit ADC

// ME4-SO2 Sulfur Dioxide Sensor
#define ME4SO2_ENABLED true
#define ME4SO2_ANALOG_PIN 35

// ZE40 TVOC Sensor
#define ZE40_ENABLED true
#define ZE40_RX_PIN 18
#define ZE40_TX_PIN 19
#define ZE40_ANALOG_PIN 36  // For analog DAC output


// ============================================================================
// TIMING CONFIGURATION
// ============================================================================

// How often to read sensors (milliseconds)
#define SENSOR_READ_INTERVAL 2000  // 2 seconds

// How often to send data to Django (milliseconds)
#define DJANGO_SEND_INTERVAL 2000  // 2 seconds

// Network check interval (milliseconds)
#define NETWORK_CHECK_INTERVAL 5000  // 5 seconds

// Watchdog timer (milliseconds)
#define WATCHDOG_TIMEOUT 30000  // 30 seconds


// ============================================================================
// SERIAL CONFIGURATION
// ============================================================================

#define SERIAL_BAUD_RATE 115200
#define SERIAL_DEBUG_ENABLED true  // Enable/disable debug output

// Debug levels
#define DEBUG_LEVEL_NONE 0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARNING 2
#define DEBUG_LEVEL_INFO 3
#define DEBUG_LEVEL_VERBOSE 4

#define DEBUG_LEVEL DEBUG_LEVEL_VERBOSE  // Set debug verbosity


// ============================================================================
// DATA VALIDATION
// ============================================================================

// Sensor reading ranges (for validation)
#define PM25_MIN 0
#define PM25_MAX 500
#define PM10_MIN 0
#define PM10_MAX 500
#define VOLTAGE_MIN 0.0
#define VOLTAGE_MAX 5.0
#define SO2_MIN 0.0
#define SO2_MAX 100.0
#define TVOC_MIN 0
#define TVOC_MAX 10000


// ============================================================================
// SYSTEM CONFIGURATION
// ============================================================================

// Device identification
#define DEVICE_NAME "SmartSensors_ESP32"
#define DEVICE_VERSION "1.0.0"
#define HARDWARE_VERSION "ESP32-S3"

// LED Configuration (for status indication)
#define LED_PIN 2  // Built-in LED
#define LED_ENABLED true

// LED blink patterns
#define LED_BLINK_NORMAL 1000     // Normal operation (1 sec)
#define LED_BLINK_ERROR 200       // Error state (fast blink)
#define LED_BLINK_NO_NETWORK 500  // No network (medium blink)


// ============================================================================
// MEMORY CONFIGURATION
// ============================================================================

// JSON document sizes (adjust if needed)
#define JSON_BUFFER_SIZE 2048  // Size for JSON serialization

// Task stack sizes
#define SENSOR_TASK_STACK_SIZE 4096
#define NETWORK_TASK_STACK_SIZE 8192


// ============================================================================
// FAIL-SAFE CONFIGURATION
// ============================================================================

// Network failure handling
#define MAX_NETWORK_FAILURES 5  // Restart after this many failures
#define NETWORK_FAILURE_RESET_TIME 60000  // Reset failure counter after (ms)

// Django communication failure
#define MAX_DJANGO_FAILURES 10  // Log warning after this many failures
#define DJANGO_FAILURE_RESTART_COUNT 50  // Restart device after this many failures


// ============================================================================
// ADVANCED CONFIGURATION
// ============================================================================

// FreeRTOS Task Priorities
#define SENSOR_TASK_PRIORITY 1
#define NETWORK_TASK_PRIORITY 2
#define WATCHDOG_TASK_PRIORITY 3

// Enable/disable features
#define ENABLE_OTA_UPDATES false  // Over-the-air firmware updates
#define ENABLE_WEB_CONFIG false   // Web-based configuration interface
#define ENABLE_MDNS true          // mDNS service discovery


// ============================================================================
// HELPER MACROS
// ============================================================================

// String conversion macro
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// Debug print macros
#if SERIAL_DEBUG_ENABLED
  #define DEBUG_PRINT(x) Serial.print(x)
  #define DEBUG_PRINTLN(x) Serial.println(x)
  #define DEBUG_PRINTF(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINTF(fmt, ...)
#endif

// Error handling
#define CHECK_ERROR(x, msg) if(!(x)) { DEBUG_PRINTLN(msg); return false; }


// ============================================================================
// CONFIGURATION VALIDATION
// ============================================================================

// Compile-time checks
#if !defined(DJANGO_SERVER_IP)
  #error "DJANGO_SERVER_IP must be defined!"
#endif

#if DJANGO_SERVER_PORT < 1 || DJANGO_SERVER_PORT > 65535
  #error "DJANGO_SERVER_PORT must be between 1 and 65535!"
#endif

#if SENSOR_READ_INTERVAL < 100
  #warning "SENSOR_READ_INTERVAL is very low. This may cause performance issues."
#endif

#if JSON_BUFFER_SIZE < 512
  #error "JSON_BUFFER_SIZE is too small. Minimum 512 bytes required."
#endif


// ============================================================================
// CONFIGURATION INFO FUNCTION
// ============================================================================

inline void printConfiguration() {
  Serial.println("============================================");
  Serial.println("ESP32 Smart Sensors Configuration");
  Serial.println("============================================");
  Serial.print("Device: "); Serial.println(DEVICE_NAME);
  Serial.print("Version: "); Serial.println(DEVICE_VERSION);
  Serial.print("Hardware: "); Serial.println(HARDWARE_VERSION);
  Serial.println();
  
  Serial.println("Network Configuration:");
  Serial.print("  Mode: "); Serial.println(NETWORK_MODE);
  Serial.print("  Django Server: "); Serial.println(DJANGO_SERVER_IP);
  Serial.print("  Django Port: "); Serial.println(DJANGO_SERVER_PORT);
  Serial.print("  Django URL: "); Serial.println(DJANGO_URL);
  Serial.println();
  
  Serial.println("Sensor Configuration:");
  Serial.print("  ZPHS01B: "); Serial.println(ZPHS01B_ENABLED ? "Enabled" : "Disabled");
  Serial.print("  MR007: "); Serial.println(MR007_ENABLED ? "Enabled" : "Disabled");
  Serial.print("  ME4-SO2: "); Serial.println(ME4SO2_ENABLED ? "Enabled" : "Disabled");
  Serial.print("  ZE40: "); Serial.println(ZE40_ENABLED ? "Enabled" : "Disabled");
  Serial.println();
  
  Serial.println("Timing Configuration:");
  Serial.print("  Sensor Read Interval: "); Serial.print(SENSOR_READ_INTERVAL); Serial.println("ms");
  Serial.print("  Django Send Interval: "); Serial.print(DJANGO_SEND_INTERVAL); Serial.println("ms");
  Serial.println();
  
  Serial.println("Debug Configuration:");
  Serial.print("  Serial Baud Rate: "); Serial.println(SERIAL_BAUD_RATE);
  Serial.print("  Debug Enabled: "); Serial.println(SERIAL_DEBUG_ENABLED ? "Yes" : "No");
  Serial.println("============================================");
}

#endif // CONFIG_H
