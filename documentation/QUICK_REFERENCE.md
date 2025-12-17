# Quick Reference - ESP32-S3 Air Quality Monitor

## System Overview
- **Platform**: ESP32-S3
- **Network**: W5500 Ethernet + WiFi fallback
- **Sensors**: ZE40 TVOC, ZPHS01B Air Quality, MR007 Gas, ME4-SO2
- **Web Server**: Port 80, Real-time JSON API

## Network Access
```
Ethernet IP: Check serial monitor at boot
Web Interface: http://<IP_ADDRESS>
JSON API: http://<IP_ADDRESS>/data
mDNS: http://smartsensor.local
```

## Pin Configuration

### Ethernet (W5500)
- CS: GPIO 10
- SCK: GPIO 12
- MISO: GPIO 11
- MOSI: GPIO 13

### Sensors
- ZE40 RX: GPIO 18, TX: GPIO 17, DAC: GPIO 4
- ZPHS01B RX: GPIO 8, TX: GPIO 7
- MR007: GPIO 5 (Analog)
- ME4-SO2: GPIO 9 (Analog)

### Control
- LED: GPIO 2
- Relay: GPIO 48
- Button: GPIO 16

## Important Code Patterns

### ✅ Correct Server Initialization
```cpp
// In class header
EthernetServer* ethServer = nullptr;

// In init() method (AFTER FreeRTOS starts)
ethServer = new EthernetServer(80);
ethServer->begin();
```

### ✅ Correct SPI Initialization
```cpp
portMUX_TYPE ethMux = portMUX_INITIALIZER_UNLOCKED;
portENTER_CRITICAL(&ethMux);
SPI.begin(SCK, MISO, MOSI, CS);
portEXIT_CRITICAL(&ethMux);
vTaskDelay(pdMS_TO_TICKS(200));
```

### ✅ Correct Mutex Usage
```cpp
// Static allocation
static StaticSemaphore_t mutexBuffer;
dataMutex = xSemaphoreCreateMutexStatic(&mutexBuffer);

// Always check ISR context
if (xPortInIsrContext()) {
    return false;
}
```

## Common Issues & Fixes

### Issue: Crash on Ethernet Init
**Symptom**: `assert failed: xQueueSemaphoreTake`
**Fix**: Use delayed server construction (pointer-based)

### Issue: DHCP Fails
**Symptom**: No IP address assigned
**Fix**: Add 200ms delays in SPI initialization

### Issue: Mutex Lock Fails
**Symptom**: Data access errors
**Fix**: Use static mutex allocation

### Issue: Random Crashes
**Symptom**: Unstable operation
**Fix**: Increase task stack sizes to 20KB

## Serial Monitor Commands (Debug)

Enable verbose output:
```cpp
#define DEBUG_SERIAL_ENABLED  // In config.h
```

Monitor heap usage:
```cpp
Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
```

## JSON API Response Format
```json
{
  "dac_voltage": 1.23,
  "dac_ppm": 0.456,
  "tvoc_ppb": 123,
  "tvoc_ppm": 0.123,
  "air_quality": {
    "pm25": 12,
    "pm10": 15,
    "co2": 450,
    "temperature": 23.5,
    "humidity": 55
  },
  "mr007": {
    "voltage": 0.123,
    "lel_concentration": 5.2
  },
  "me4_so2": {
    "voltage": 0.0012,
    "current_ua": 120.5,
    "so2_concentration": 0.15
  },
  "ip_address": "192.168.1.3",
  "network_mode": "eth"
}
```

## Task Configuration

```cpp
// Stack Sizes (bytes)
ETH_TASK_STACK_SIZE = 20480
SENSOR_TASK_STACK_SIZE = 20480

// Priorities
ETH_TASK_PRIORITY = 2
SENSOR_TASK_PRIORITY = 1

// Core Assignment
Ethernet Task → Core 0
Sensor Task → Core 1
```

## Timing Configuration

```cpp
DAC_READ_INTERVAL = 5000ms
ZE40_REQUEST_INTERVAL = 30000ms
ZPHS01B_READ_INTERVAL = 5000ms
MR007_READ_INTERVAL = 2000ms
ME4_SO2_READ_INTERVAL = 2000ms
SENSOR_WARMUP_TIME = 180000ms
LED_TIMEOUT = 5000ms
```

## Build Settings

**Board**: ESP32-S3 Dev Module
**Flash Size**: 4MB (or higher)
**PSRAM**: Enabled (if available)
**Partition**: Default 4MB with spiffs

## Version History

- **v1.0** - Initial ESP32-WROOM version
- **v2.0** - ESP32-S3 port with crash fixes ✅
  - Fixed EthernetServer global object issue
  - Added SPI critical sections
  - Implemented static mutex allocation
  - Added ISR context protection

## Support

For issues or questions:
1. Check SOLUTION_FINAL.md for detailed explanation
2. Check ESP32_S3_PORTING_GUIDE.md for porting tips
3. Review serial monitor output for error messages

---
**Last Updated**: November 16, 2025
**Status**: Production Ready ✅
