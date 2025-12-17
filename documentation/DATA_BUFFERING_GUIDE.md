# ESP32 Data Buffering Guide

## Overview

When the network connection is interrupted or Django is unavailable, sensor data is lost. This guide explains how to implement **persistent data buffering** on the ESP32 board so that:

1. **Sensor readings are saved** to flash memory (SPIFFS/LittleFS) when network is unavailable
2. **Data persists** across reboots
3. **Buffered data is retrieved and sent** once the network reconnects
4. **Old data is automatically cleaned up** to prevent filling flash memory

---

## Table of Contents

1. [Storage Options](#storage-options)
2. [Implementation Approaches](#implementation-approaches)
3. [Complete Buffer Manager Implementation](#complete-buffer-manager-implementation)
4. [Integration with Django Client](#integration-with-django-client)
5. [Recovery & Cleanup](#recovery--cleanup)
6. [Memory Considerations](#memory-considerations)
7. [Testing & Verification](#testing--verification)

---

## Storage Options

### Option 1: SPIFFS (SPI Flash File System)

**Pros:**
- Built into Arduino ESP32 framework
- Easy file-based storage
- Good for large amounts of data
- Survives reboots

**Cons:**
- Slower than memory
- Uses more flash space
- Overhead for file system

**Best for:** Large buffer files, hourly or daily data dumps

**Code:**
```cpp
#include <SPIFFS.h>

// Initialize
SPIFFS.begin(true);  // true = auto-format if needed

// Write
File f = SPIFFS.open("/buffer.json", FILE_APPEND);
f.println(jsonData);
f.close();

// Read
File f = SPIFFS.open("/buffer.json", FILE_READ);
String content = f.readString();
f.close();

// Delete
SPIFFS.remove("/buffer.json");
```

---

### Option 2: LittleFS (Recommended for newer projects)

**Pros:**
- Faster than SPIFFS
- Better wear leveling
- Recommended by modern ESP32 docs
- Same API as SPIFFS

**Cons:**
- Requires newer ESP32 board definitions
- Partition needs to be sized correctly

**Best for:** New projects, high-frequency buffering

**Code:**
```cpp
#include <LittleFS.h>

// Initialize (same as SPIFFS)
LittleFS.begin(true);

// Write/Read same as SPIFFS
```

---

### Option 3: Circular Buffer in RAM + Periodic Flash Save

**Pros:**
- Very fast reads/writes
- Real-time buffering
- Minimal flash wear

**Cons:**
- Data lost if power fails
- Limited by available RAM (typically 520KB free)
- Need periodic flushing to flash

**Best for:** Real-time buffering with periodic persistence

---

### Option 4: EEPROM (Not Recommended)

**Why not:**
- Limited size (only 4KB typically usable)
- Slow
- Limited write cycles (~100,000)

**Only use if:** You need to buffer just 1-2 readings

---

## Recommended Strategy

**Use a hybrid approach:**

1. **RAM Buffer** — Keep recent readings (last 10-100) in memory for fast access
2. **Flash Buffer** — Periodically save important readings to SPIFFS/LittleFS
3. **Cleanup Policy** — Delete old buffered data after successful send or after X days

This balances **speed** (RAM) with **persistence** (Flash).

---

## Implementation Approaches

### Approach 1: Simple JSON File Queue (Easiest)

Store each sensor reading as a JSON line in a file.

**File format:**
```
{"timestamp":1702916400,"ze40_tvoc_ppb":45.2,"zphs01b_pm25":22.5,...}
{"timestamp":1702916460,"ze40_tvoc_ppb":46.1,"zphs01b_pm25":23.1,...}
{"timestamp":1702916520,"ze40_tvoc_ppb":45.8,"zphs01b_pm25":22.8,...}
```

**Advantages:**
- Human-readable
- Easy to debug
- Can be parsed line-by-line
- Good for any file size

**Disadvantages:**
- Slightly slower than binary
- More storage space

---

### Approach 2: Binary Ring Buffer (Most Efficient)

Store sensor data in a fixed-size circular buffer (binary format).

**Advantages:**
- Very fast writes
- Predictable memory usage
- Minimal overhead

**Disadvantages:**
- More complex to implement
- Less readable during debugging

---

### Approach 3: CSV File (Simple & Compatible)

Store as comma-separated values.

**Advantages:**
- Easy to import to spreadsheets
- Compact
- Easy to parse

**Disadvantages:**
- Loses some structure (no nested JSON)

---

## Complete Buffer Manager Implementation

I'll provide a **ready-to-use buffer manager** that uses JSON file queue approach with SPIFFS.

### Files to Create

1. **`buffer_manager.h`** — Header with API
2. **`buffer_manager.cpp`** — Implementation
3. **Integration changes** to `django_client.cpp`

### buffer_manager.h

```cpp
#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <Arduino.h>
#include "shared_data.h"

/**
 * BufferManager
 * 
 * Handles persistent buffering of sensor data to SPIFFS.
 * When network is unavailable, data is saved to flash.
 * When network reconnects, data is retrieved and sent.
 */
class BufferManager {
private:
    static const char* BUFFER_FILE;      // "/data_buffer.jsonl"
    static const size_t MAX_BUFFER_SIZE; // 500KB max
    static const size_t MAX_ENTRIES;     // Max number of entries to buffer
    
    static size_t getCurrentBufferSize();
    
public:
    /**
     * Initialize buffer manager and SPIFFS
     */
    static bool init();
    
    /**
     * Save current sensor data to buffer
     * @return true if saved successfully
     */
    static bool saveData(const SharedSensorData& data, unsigned long timestamp = 0);
    
    /**
     * Save sensor data from JSON string
     * Useful for buffering already-formatted JSON
     */
    static bool saveJSON(const String& jsonData);
    
    /**
     * Get all buffered entries as an array of JSON objects
     * @param maxEntries Maximum entries to retrieve (0 = all)
     * @return Array of JSON strings, or empty if no data
     */
    static String getBufferedEntries(size_t maxEntries = 0);
    
    /**
     * Get number of buffered entries
     */
    static size_t getEntryCount();
    
    /**
     * Get total buffered data size in bytes
     */
    static size_t getBufferSize();
    
    /**
     * Clear all buffered data
     */
    static bool clearBuffer();
    
    /**
     * Remove first N entries from buffer
     * @param count Number of entries to remove
     * @return true if successful
     */
    static bool removeEntries(size_t count);
    
    /**
     * Get buffer status for monitoring
     */
    static String getStatus();
    
    /**
     * Check if buffer has data
     */
    static bool hasData();
    
    /**
     * Check if buffer is nearly full
     * @return true if usage > 80%
     */
    static bool isAlmostFull();
};

#endif
```

### buffer_manager.cpp

```cpp
#include "buffer_manager.h"
#include <SPIFFS.h>
#include "config.h"

const char* BufferManager::BUFFER_FILE = "/data_buffer.jsonl";
const size_t BufferManager::MAX_BUFFER_SIZE = 512000;  // 500KB
const size_t BufferManager::MAX_ENTRIES = 2000;         // Reasonable limit

bool BufferManager::init() {
    if (!SPIFFS.begin(true)) {
        DEBUG_PRINTLN("✗ SPIFFS initialization failed");
        return false;
    }
    
    DEBUG_PRINTLN("✓ SPIFFS initialized");
    
    // Check if buffer file exists, if not create it
    if (!SPIFFS.exists(BUFFER_FILE)) {
        File f = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
        if (f) {
            f.close();
            DEBUG_PRINTLN("✓ Buffer file created");
        } else {
            DEBUG_PRINTLN("✗ Failed to create buffer file");
            return false;
        }
    }
    
    return true;
}

bool BufferManager::saveData(const SharedSensorData& data, unsigned long timestamp) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        DEBUG_PRINTLN("✗ Buffer file not found");
        return false;
    }
    
    // Check if we're at capacity
    if (getEntryCount() >= MAX_ENTRIES) {
        DEBUG_PRINTLN("⚠ Buffer at max entries, not saving");
        return false;
    }
    
    if (getBufferSize() >= MAX_BUFFER_SIZE) {
        DEBUG_PRINTLN("⚠ Buffer is full, not saving");
        return false;
    }
    
    // Build JSON entry (same format as django_client.cpp)
    String json = "{";
    json += "\"timestamp\":" + String(timestamp > 0 ? timestamp : millis() / 1000) + ",";
    
    // ZE40 Data
    json += "\"ze40\":{";
    json += "\"tvoc_ppb\":" + String(data.ze40_tvoc_ppb) + ",";
    json += "\"tvoc_ppm\":" + String(data.ze40_tvoc_ppm, 3) + ",";
    json += "\"dac_voltage\":" + String(data.ze40_dac_voltage, 2) + ",";
    json += "\"dac_ppm\":" + String(data.ze40_dac_ppm, 3) + ",";
    json += "\"uart_data_valid\":" + String(data.ze40_uart_valid ? "true" : "false");
    json += "},";
    
    // ZPHS01B Data
    if (data.zphs01b_valid) {
        json += "\"air_quality\":{";
        json += "\"pm1\":" + String(data.zphs01b_pm1) + ",";
        json += "\"pm25\":" + String(data.zphs01b_pm25) + ",";
        json += "\"pm10\":" + String(data.zphs01b_pm10) + ",";
        json += "\"co2\":" + String(data.zphs01b_co2) + ",";
        json += "\"voc\":" + String(data.zphs01b_voc) + ",";
        json += "\"ch2o\":" + String(data.zphs01b_ch2o) + ",";
        json += "\"co\":" + String(data.zphs01b_co, 1) + ",";
        json += "\"o3\":" + String(data.zphs01b_o3, 2) + ",";
        json += "\"no2\":" + String(data.zphs01b_no2, 3) + ",";
        json += "\"temperature\":" + String(data.zphs01b_temperature, 1) + ",";
        json += "\"humidity\":" + String(data.zphs01b_humidity);
        json += "},";
    } else {
        json += "\"air_quality\":null,";
    }
    
    // MR007 Data
    if (data.mr007_valid) {
        json += "\"mr007\":{";
        json += "\"voltage\":" + String(data.mr007_voltage, 3) + ",";
        json += "\"rawValue\":" + String(data.mr007_raw) + ",";
        json += "\"lel_concentration\":" + String(data.mr007_lel, 1);
        json += "},";
    } else {
        json += "\"mr007\":null,";
    }
    
    // ME4-SO2 Data
    if (data.me4so2_valid) {
        json += "\"me4_so2\":{";
        json += "\"voltage\":" + String(data.me4so2_voltage, 4) + ",";
        json += "\"rawValue\":" + String(data.me4so2_raw) + ",";
        json += "\"current_ua\":" + String(data.me4so2_current, 2) + ",";
        json += "\"so2_concentration\":" + String(data.me4so2_so2, 2);
        json += "},";
    } else {
        json += "\"me4_so2\":null,";
    }
    
    // Network Info
    json += "\"ip_address\":\"" + String(data.ip_address) + "\",";
    json += "\"network_ready\":" + String(data.network_ready ? "true" : "false");
    json += "}";
    
    return saveJSON(json);
}

bool BufferManager::saveJSON(const String& jsonData) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return false;
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_APPEND);
    if (!f) {
        DEBUG_PRINTLN("✗ Failed to open buffer file for writing");
        return false;
    }
    
    f.println(jsonData);  // Add newline to separate entries
    f.close();
    
    DEBUG_PRINT("✓ Buffered: ");
    DEBUG_PRINTLN(jsonData.length());
    DEBUG_PRINTLN(" bytes");
    
    return true;
}

String BufferManager::getBufferedEntries(size_t maxEntries) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return "";
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!f) {
        return "";
    }
    
    String result = "[";
    size_t count = 0;
    bool first = true;
    
    while (f.available() && (maxEntries == 0 || count < maxEntries)) {
        String line = f.readStringUntil('\n');
        line.trim();
        
        if (line.length() > 0) {
            if (!first) result += ",";
            result += line;
            first = false;
            count++;
        }
    }
    
    f.close();
    result += "]";
    
    return result;
}

size_t BufferManager::getEntryCount() {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return 0;
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!f) {
        return 0;
    }
    
    size_t count = 0;
    while (f.available()) {
        String line = f.readStringUntil('\n');
        if (line.length() > 0) {
            count++;
        }
    }
    f.close();
    
    return count;
}

size_t BufferManager::getCurrentBufferSize() {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return 0;
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!f) {
        return 0;
    }
    
    size_t size = f.size();
    f.close();
    
    return size;
}

size_t BufferManager::getBufferSize() {
    return getCurrentBufferSize();
}

bool BufferManager::clearBuffer() {
    if (SPIFFS.remove(BUFFER_FILE)) {
        DEBUG_PRINTLN("✓ Buffer cleared");
        
        // Recreate empty file
        File f = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
        if (f) {
            f.close();
            return true;
        }
    }
    return false;
}

bool BufferManager::removeEntries(size_t count) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return false;
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!f) return false;
    
    String tempContent = "";
    size_t skipped = 0;
    
    while (f.available()) {
        String line = f.readStringUntil('\n');
        
        if (skipped < count) {
            skipped++;
        } else if (line.length() > 0) {
            tempContent += line + "\n";
        }
    }
    f.close();
    
    // Write back filtered content
    f = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
    if (!f) return false;
    
    f.print(tempContent);
    f.close();
    
    DEBUG_PRINT("✓ Removed ");
    DEBUG_PRINT(skipped);
    DEBUG_PRINTLN(" entries from buffer");
    
    return true;
}

String BufferManager::getStatus() {
    size_t entries = getEntryCount();
    size_t bytes = getBufferSize();
    size_t percentFull = (bytes * 100) / MAX_BUFFER_SIZE;
    
    String status = "Buffer Status:\n";
    status += "  Entries: " + String(entries) + "\n";
    status += "  Size: " + String(bytes) + " bytes\n";
    status += "  Usage: " + String(percentFull) + "%\n";
    status += "  Max: " + String(MAX_BUFFER_SIZE) + " bytes";
    
    return status;
}

bool BufferManager::hasData() {
    return getEntryCount() > 0;
}

bool BufferManager::isAlmostFull() {
    size_t bytes = getBufferSize();
    return (bytes * 100 / MAX_BUFFER_SIZE) > 80;
}
```

---

## Integration with Django Client

### Step 1: Update `django_client.h`

Add buffer manager forward declaration and method to send buffered data:

```cpp
#include "buffer_manager.h"

class DjangoClient {
    // ... existing code ...
    
    // Send buffered data from storage
    static void sendBufferedData();
    
    // Save data to buffer if network unavailable
    static bool bufferSensorData(const SharedSensorData& data);
};
```

### Step 2: Update `django_client.cpp`

Add these methods:

```cpp
bool DjangoClient::bufferSensorData(const SharedSensorData& data) {
    if (!networkManager.isEthernetActive() && !networkManager.isWifiActive()) {
        DEBUG_PRINTLN("⚠ Network unavailable - buffering data");
        return BufferManager::saveData(data);
    }
    return false;
}

void DjangoClient::sendBufferedData() {
    if (!BufferManager::hasData()) {
        return;  // No buffered data
    }
    
    if (!networkManager.isEthernetActive() && !networkManager.isWifiActive()) {
        return;  // No network to send
    }
    
    DEBUG_PRINTLN("╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   SENDING BUFFERED DATA TO DJANGO      ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    size_t entryCount = BufferManager::getEntryCount();
    DEBUG_PRINT("Buffered entries: ");
    DEBUG_PRINTLN(entryCount);
    
    // Get all buffered entries
    String bufferedJSON = BufferManager::getBufferedEntries();
    
    if (bufferedJSON == "[]") {
        DEBUG_PRINTLN("⚠ No valid buffered data");
        return;
    }
    
    // Send to Django
    if (sendHTTPPOST(serverURL, bufferedJSON)) {
        DEBUG_PRINT("✓ Sent ");
        DEBUG_PRINT(entryCount);
        DEBUG_PRINTLN(" buffered entries");
        
        // Clear buffer after successful send
        BufferManager::clearBuffer();
    } else {
        DEBUG_PRINTLN("✗ Failed to send buffered data - will retry later");
    }
}
```

### Step 3: Update `sendSensorData()` in `django_client.cpp`

Modify to attempt buffering if send fails:

```cpp
void DjangoClient::sendSensorData() {
    // ... existing checks ...
    
    String payload = buildJSONPayload();
    
    if (payload.length() == 0 || payload == "{}") {
        DEBUG_PRINTLN("⚠ Empty payload - skipping send");
        lastSendTime = millis();
        return;
    }
    
    unsigned long sendStart = millis();
    
    // Try to send to Django
    if (sendHTTPPOST(serverURL, payload)) {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✓ Data successfully sent to Django");
        
        // Also try to send any buffered data
        sendBufferedData();
    } else {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✗ Failed to send data to Django");
        
        // Buffer the data for later
        if (lockData(1000)) {
            bufferSensorData(sharedData);
            unlockData();
        }
        
        // Check buffer status
        if (BufferManager::isAlmostFull()) {
            DEBUG_PRINTLN("⚠ Buffer is nearly full!");
        }
    }
    
    lastSendTime = millis();
}
```

### Step 4: Initialize Buffer Manager in `main.ino`

```cpp
void setup() {
    // ... existing code ...
    
    // Initialize buffer manager for persistent storage
    if (!BufferManager::init()) {
        DEBUG_PRINTLN("⚠ Failed to initialize buffer manager");
    }
    
    // ... rest of setup ...
}
```

### Step 5: Periodically Send Buffered Data in `task_manager.cpp`

```cpp
unsigned long lastBufferedSend = 0;

void TaskManager::sensorTask(void *pvParameters) {
    // ... existing code ...
    
    while (true) {
        // ... existing sensor reading code ...
        
        // Every 30 seconds, try to send buffered data
        if (millis() - lastBufferedSend > 30000) {
            djangoClient.sendBufferedData();
            lastBufferedSend = millis();
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
```

---

## Recovery & Cleanup

### Automatic Cleanup Strategies

#### Strategy 1: Delete After Successful Send

```cpp
void DjangoClient::sendBufferedData() {
    if (sendHTTPPOST(serverURL, bufferedJSON)) {
        DEBUG_PRINTLN("✓ Clearing buffer after successful send");
        BufferManager::clearBuffer();  // Delete all
    }
}
```

#### Strategy 2: Delete Old Entries (Time-based)

```cpp
void DjangoClient::cleanupOldBufferedData() {
    unsigned long now = millis() / 1000;
    unsigned long maxAge = 24 * 3600;  // 24 hours
    
    // Parse buffer and remove entries older than 24 hours
    // (Implementation depends on your timestamp format)
    
    DEBUG_PRINTLN("✓ Cleaned up old buffered data");
}
```

#### Strategy 3: Delete When Full (FIFO)

```cpp
if (BufferManager::isAlmostFull()) {
    DEBUG_PRINTLN("⚠ Buffer full - removing oldest entries");
    BufferManager::removeEntries(100);  // Remove 100 oldest
}
```

---

## Memory Considerations

### Flash Storage

**ESP32 partitions:**
- Default: 1.4MB SPIFFS available
- With our 500KB limit: Plenty of room
- Typical entry size: ~300-500 bytes
- **Can buffer ~1000 entries** before hitting 500KB limit

### RAM Impact

**Buffer manager memory:**
- Minimal: Just file handles and temporary strings
- During write: ~1-2KB temporary buffer
- During read: Depends on getBufferedEntries() - allocates String for entire result

**To minimize RAM:**
```cpp
// Read entries one at a time instead of all at once
void processBufferOneAtATime() {
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    while (f.available()) {
        String line = f.readStringUntil('\n');
        // Process line individually
        djangoClient.sendHTTPPOST(serverURL, line);
    }
    f.close();
}
```

---

## Testing & Verification

### Test 1: Basic Buffering

```cpp
void testBuffering() {
    // Disable network
    // networkManager.disableNetwork();
    
    // Collect some readings
    for (int i = 0; i < 10; i++) {
        if (lockData(1000)) {
            // Simulate sensor data
            sharedData.ze40_tvoc_ppb = random(0, 100);
            sharedData.zphs01b_pm25 = random(0, 50);
            
            // Try to send (will fail, data gets buffered)
            djangoClient.sendSensorData();
            
            unlockData();
        }
        
        delay(2000);  // Wait 2 seconds
    }
    
    // Check buffer status
    Serial.println(BufferManager::getStatus());
}
```

### Test 2: Recovery After Reconnect

```cpp
void testRecovery() {
    // Simulate network loss → buffering → recovery
    
    // 1. Disable network and collect data (gets buffered)
    // 2. Enable network
    // 3. Call djangoClient.sendBufferedData()
    // 4. Verify Django received all buffered entries
    
    Serial.println("Buffer entries before: " + String(BufferManager::getEntryCount()));
    
    // ... enable network ...
    djangoClient.sendBufferedData();
    
    // ... wait a moment ...
    
    Serial.println("Buffer entries after: " + String(BufferManager::getEntryCount()));
}
```

### Test 3: Manual Inspection

**Via Serial Monitor:**
```cpp
// Add these debug commands to web_server.cpp or handle via UART
if (command == "buffer:status") {
    Serial.println(BufferManager::getStatus());
}

if (command == "buffer:show") {
    Serial.println(BufferManager::getBufferedEntries());
}

if (command == "buffer:clear") {
    BufferManager::clearBuffer();
    Serial.println("Buffer cleared");
}
```

**On the filesystem:**
```bash
# Connect via serial and check the buffer file
# Download SPIFFS image and inspect
```

---

## Troubleshooting

### Problem: Buffer Not Saving

**Check:**
1. SPIFFS initialized? → `BufferManager::init()` called in setup()
2. File permissions? → Unlikely on SPIFFS, but check `SPIFFS.exists()`
3. Flash full? → Check available space with `SPIFFS.usedBytes()` / `SPIFFS.totalBytes()`

**Debug:**
```cpp
DEBUG_PRINTLN("SPIFFS info:");
DEBUG_PRINT("  Used: "); DEBUG_PRINTLN(SPIFFS.usedBytes());
DEBUG_PRINT("  Total: "); DEBUG_PRINTLN(SPIFFS.totalBytes());
```

### Problem: Buffer Growing Too Large

**Solutions:**
1. Lower `MAX_ENTRIES` constant
2. Lower `MAX_BUFFER_SIZE` constant
3. Send buffered data more frequently
4. Implement time-based cleanup

### Problem: Buffered Data Format Issues

**Symptoms:** Django 400 error when sending buffered data

**Solution:**
- Ensure each buffered entry is valid JSON
- Test format by sending a single buffered entry manually
- Check that `getBufferedEntries()` wraps entries in valid JSON array

---

## Summary

### Implementation Checklist

- [ ] Create `buffer_manager.h` and `buffer_manager.cpp`
- [ ] Add `#include "buffer_manager.h"` to `django_client.h`
- [ ] Add buffering methods to `django_client.cpp`
- [ ] Call `BufferManager::init()` in `main.ino` setup
- [ ] Update `sendSensorData()` to buffer on send failure
- [ ] Add periodic `sendBufferedData()` call in task manager
- [ ] Test buffering with network disabled
- [ ] Test recovery when network reconnects
- [ ] Monitor buffer size and cleanup policies

### Next Steps

1. **Start simple:** Implement basic file queue buffering first
2. **Test thoroughly:** Verify recovery after power loss/reboot
3. **Monitor storage:** Check SPIFFS usage over time
4. **Adjust limits:** Tune `MAX_BUFFER_SIZE` based on actual data rates
5. **Add cleanup:** Implement time-based or size-based cleanup as needed
6. **Document data:** Add timestamps and metadata for retrieval later

---

## API Reference

```cpp
// Initialize
bool BufferManager::init();

// Save data
bool BufferManager::saveData(const SharedSensorData& data, unsigned long timestamp = 0);
bool BufferManager::saveJSON(const String& jsonData);

// Retrieve data
String BufferManager::getBufferedEntries(size_t maxEntries = 0);
size_t BufferManager::getEntryCount();
size_t BufferManager::getBufferSize();

// Manage buffer
bool BufferManager::clearBuffer();
bool BufferManager::removeEntries(size_t count);
bool BufferManager::hasData();
bool BufferManager::isAlmostFull();

// Status
String BufferManager::getStatus();
```

---

## Additional Resources

- [SPIFFS Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html)
- [LittleFS for ESP32](https://github.com/earlephilhower/LittleFS)
- [FreeRTOS on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos.html)
