# Data Buffering Integration Snippets

This file contains ready-to-copy code snippets for integrating the BufferManager into your existing project.

---

## 1. Update main.ino (Setup Function)

**Add this to your `setup()` function:**

```cpp
void setup() {
    // ... your existing setup code ...
    
    // Initialize buffer manager for persistent data storage
    DEBUG_PRINTLN("\n╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   INITIALIZING BUFFER MANAGER          ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    if (!BufferManager::init()) {
        DEBUG_PRINTLN("⚠ Warning: Buffer manager failed to initialize");
        DEBUG_PRINTLN("  Sensor data will not be buffered if network fails");
    } else {
        DEBUG_PRINTLN("✓ Buffer manager ready");
        DEBUG_PRINTLN(BufferManager::getStatus());
    }
    
    DEBUG_PRINTLN("");
}
```

---

## 2. Update django_client.h (Add Declarations)

**Add these includes at the top (if not already there):**

```cpp
#include "buffer_manager.h"
#include "shared_data.h"
#include "network_manager.h"
```

**Add these method declarations to the `DjangoClient` class:**

```cpp
class DjangoClient {
    // ... existing methods ...
    
    /**
     * Buffer current sensor data to persistent storage
     * Called when network is unavailable or send fails
     */
    static bool bufferSensorData(const SharedSensorData& data);
    
    /**
     * Attempt to send all buffered data to Django
     * Called periodically to retry buffered data
     */
    static void sendBufferedData();
};
```

---

## 3. Update django_client.cpp (Add Implementations)

**Add these two new methods to the `DjangoClient` class:**

```cpp
bool DjangoClient::bufferSensorData(const SharedSensorData& data) {
    if (!networkManager.isEthernetActive() && !networkManager.isWifiActive()) {
        DEBUG_PRINTLN("⚠ Network unavailable - buffering sensor data");
        
        if (BufferManager::saveData(data)) {
            DEBUG_PRINT("  Buffer status: ");
            DEBUG_PRINT(BufferManager::getEntryCount());
            DEBUG_PRINT(" entries, ");
            DEBUG_PRINT(BufferManager::getBufferSize());
            DEBUG_PRINTLN(" bytes");
            
            if (BufferManager::isAlmostFull()) {
                DEBUG_PRINTLN("  ⚠ WARNING: Buffer is nearly full!");
            }
            
            return true;
        } else {
            DEBUG_PRINTLN("  ✗ Failed to buffer data");
            return false;
        }
    }
    return false;
}

void DjangoClient::sendBufferedData() {
    if (!BufferManager::hasData()) {
        return;  // No buffered data to send
    }
    
    if (!networkManager.isEthernetActive() && !networkManager.isWifiActive()) {
        return;  // No network to send data
    }
    
    size_t entryCount = BufferManager::getEntryCount();
    
    DEBUG_PRINTLN("╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   SENDING BUFFERED DATA TO DJANGO      ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    DEBUG_PRINT("Buffered entries: ");
    DEBUG_PRINTLN(entryCount);
    
    // Get all buffered entries as a JSON array
    String bufferedJSON = BufferManager::getBufferedEntries();
    
    if (bufferedJSON == "[]") {
        DEBUG_PRINTLN("⚠ No valid buffered data to send");
        return;
    }
    
    DEBUG_PRINT("Payload size: ");
    DEBUG_PRINT(bufferedJSON.length());
    DEBUG_PRINTLN(" bytes");
    
    unsigned long sendStart = millis();
    
    // Send to Django
    if (sendHTTPPOST(serverURL, bufferedJSON)) {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINT("✓ Successfully sent ");
        DEBUG_PRINT(entryCount);
        DEBUG_PRINT(" buffered entries in ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms");
        
        // Clear buffer after successful send
        if (BufferManager::clearBuffer()) {
            DEBUG_PRINTLN("✓ Buffer cleared");
        } else {
            DEBUG_PRINTLN("⚠ Failed to clear buffer");
        }
    } else {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINT("✗ Failed to send buffered data (took ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms)");
        DEBUG_PRINTLN("  Will retry when network is more stable");
    }
    
    DEBUG_PRINTLN("═══════════════════════════════════════════\n");
}
```

---

## 4. Update sendSensorData() in django_client.cpp

**Find the `sendSensorData()` method and modify it:**

**Before (current code):**
```cpp
void DjangoClient::sendSensorData() {
    // ... existing checks ...
    
    unsigned long sendStart = millis();
    
    // Use native socket-based POST (avoids HTTPClient mutex conflicts)
    if (sendHTTPPOST(serverURL, payload)) {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✓ Data successfully sent to Django");
        // ...
    } else {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✗ Failed to send data to Django");
        // ...
    }
    
    lastSendTime = millis();
}
```

**After (with buffering):**
```cpp
void DjangoClient::sendSensorData() {
    // Check if enough time has passed since last send
    if (millis() - lastSendTime < SEND_INTERVAL) {
        return;
    }
    
    // Check if server URL is set
    if (serverURL.length() == 0) {
        DEBUG_PRINTLN("⚠ Django server URL not set");
        return;
    }
    
    // Check if network is available
    if (!networkManager.isEthernetActive() && !networkManager.isWifiActive()) {
        DEBUG_PRINTLN("⚠ No network connection - attempting to buffer data");
        
        if (lockData(1000)) {
            bufferSensorData(sharedData);
            unlockData();
        }
        return;
    }
    
    // Add delay to allow Ethernet operations to complete
    vTaskDelay(pdMS_TO_TICKS(100));
    
    DEBUG_PRINTLN("╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   SENDING DATA TO DJANGO BACKEND       ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    String payload = buildJSONPayload();
    
    if (payload.length() == 0 || payload == "{}") {
        DEBUG_PRINTLN("⚠ Empty payload - skipping send");
        lastSendTime = millis();
        return;
    }
    
    DEBUG_PRINTLN("→ Target URL: " + serverURL);
    DEBUG_PRINTLN("→ Payload size: " + String(payload.length()) + " bytes");
    DEBUG_PRINTLN("");
    
    unsigned long sendStart = millis();
    
    // Use native socket-based POST (avoids HTTPClient mutex conflicts)
    if (sendHTTPPOST(serverURL, payload)) {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✓ Data successfully sent to Django");
        DEBUG_PRINT("  Send Time: ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms");
        
        // *** NEW: Try to send any buffered data as well ***
        DEBUG_PRINTLN("");
        sendBufferedData();
        
    } else {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✗ Failed to send data to Django");
        DEBUG_PRINT("  Attempted for: ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms");
        
        // *** NEW: Buffer the data for later ***
        DEBUG_PRINTLN("→ Attempting to buffer data for later transmission...");
        if (lockData(1000)) {
            if (bufferSensorData(sharedData)) {
                DEBUG_PRINTLN("✓ Data buffered successfully");
            } else {
                DEBUG_PRINTLN("✗ Failed to buffer data");
            }
            unlockData();
        }
    }
    
    // Add delay after HTTP operation to let stack recover
    vTaskDelay(pdMS_TO_TICKS(100));
    
    lastSendTime = millis();
    
    DEBUG_PRINTLN("═══════════════════════════════════════════\n");
}
```

---

## 5. Update task_manager.cpp (Add Periodic Buffered Send)

**Add this variable at the top of the file (before the TaskManager functions):**

```cpp
// Track when we last attempted to send buffered data
static unsigned long lastBufferedSendAttempt = 0;
#define BUFFERED_SEND_INTERVAL 30000  // Try every 30 seconds
```

**Find the `sensorTask()` function and add this code in the main loop:**

```cpp
void TaskManager::sensorTask(void *pvParameters) {
    // ... existing code ...
    
    unsigned long lastReadings[4] = {0};
    bool buttonPressed = false;
    
    while (true) {
        unsigned long currentTime = millis();
        
        readSensors(currentTime);
        
        #ifdef BUTTON_LED_ENABLED
        handleButtonAndRelay(currentTime, buttonPressed);
        #endif
        
        #ifdef ETHERNET_ENABLED
        if (!networkManager.isEthernetActive()) {
            webServer.handleWiFiClient();
        }
        #endif
        
        // *** NEW: Periodically attempt to send buffered data ***
        if (currentTime - lastBufferedSendAttempt >= BUFFERED_SEND_INTERVAL) {
            if (BufferManager::hasData()) {
                DEBUG_PRINTLN("\n→ Attempting to send buffered data...");
                djangoClient.sendBufferedData();
            }
            lastBufferedSendAttempt = currentTime;
        }
        
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
```

---

## 6. (Optional) Add Monitoring Command to Web Server

**In `web_server.cpp`, add this to your HTTP request handler to provide a `/buffer/status` endpoint:**

```cpp
// Add this method to the WebServer class or to your request handler
void handleBufferStatus() {
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Connection: close\r\n\r\n";
    
    // Build JSON response with buffer status
    response += "{";
    response += "\"has_data\":" + String(BufferManager::hasData() ? "true" : "false") + ",";
    response += "\"entries\":" + String(BufferManager::getEntryCount()) + ",";
    response += "\"size_bytes\":" + String(BufferManager::getBufferSize()) + ",";
    response += "\"usage_percent\":" + String(BufferManager::getUsagePercent()) + ",";
    response += "\"is_nearly_full\":" + String(BufferManager::isAlmostFull() ? "true" : "false");
    response += "}";
    
    // Send response (adapt to your web server implementation)
}

// Then handle requests to /buffer/status in your HTTP handler:
// if (request.startsWith("GET /buffer/status")) {
//     handleBufferStatus();
// }
```

---

## Testing the Implementation

### Test 1: Verify Buffering Works

```cpp
// Add this test function to main.ino
void testBuffering() {
    DEBUG_PRINTLN("\n╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   TESTING DATA BUFFERING               ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    // Simulate network failure
    DEBUG_PRINTLN("Simulating network failure...");
    
    // Collect some readings with network disabled
    for (int i = 0; i < 5; i++) {
        if (lockData(1000)) {
            // Simulate sensor data
            sharedData.ze40_tvoc_ppb = random(0, 100);
            sharedData.zphs01b_pm25 = random(0, 50);
            sharedData.zphs01b_valid = true;
            
            // Attempt to send (will fail and buffer)
            djangoClient.sendSensorData();
            
            unlockData();
        }
        
        delay(2000);
    }
    
    // Check buffer
    DEBUG_PRINTLN("\nBuffer Status After Collection:");
    DEBUG_PRINTLN(BufferManager::getStatus());
    
    // Show buffered entries
    DEBUG_PRINTLN("\nBuffered Entries:");
    DEBUG_PRINTLN(BufferManager::getBufferedEntries());
}

// Call this from setup() to test: testBuffering();
```

### Test 2: Verify Recovery

```cpp
void testRecovery() {
    DEBUG_PRINTLN("\n╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   TESTING RECOVERY                     ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    size_t entriesBefore = BufferManager::getEntryCount();
    DEBUG_PRINT("Buffered entries before: ");
    DEBUG_PRINTLN(entriesBefore);
    
    if (networkManager.isEthernetActive() || networkManager.isWifiActive()) {
        DEBUG_PRINTLN("Network is available - attempting to send buffered data...");
        djangoClient.sendBufferedData();
        
        delay(2000);
        
        size_t entriesAfter = BufferManager::getEntryCount();
        DEBUG_PRINT("Buffered entries after: ");
        DEBUG_PRINTLN(entriesAfter);
        
        if (entriesAfter == 0 && entriesBefore > 0) {
            DEBUG_PRINTLN("✓ Recovery test PASSED - buffer was sent and cleared");
        } else if (entriesAfter < entriesBefore) {
            DEBUG_PRINTLN("✓ Partial success - some entries were sent");
        } else {
            DEBUG_PRINTLN("✗ Recovery test FAILED - buffer still has data");
        }
    } else {
        DEBUG_PRINTLN("✗ Network not available - cannot test recovery");
    }
}
```

---

## Summary of Changes

| File | Changes |
|------|---------|
| `main.ino` | Add `BufferManager::init()` in setup |
| `main/django_client.h` | Add `#include "buffer_manager.h"` and 2 method declarations |
| `main/django_client.cpp` | Add `bufferSensorData()` and `sendBufferedData()` methods; modify `sendSensorData()` |
| `main/task_manager.cpp` | Add periodic call to `sendBufferedData()` in sensor task |
| `main/web_server.cpp` | (Optional) Add `/buffer/status` endpoint |
| `main/buffer_manager.h` | NEW FILE |
| `main/buffer_manager.cpp` | NEW FILE |

---

## Verification Checklist

After implementing, verify:

- [ ] Code compiles without errors
- [ ] `BufferManager::init()` is called and succeeds
- [ ] When network is disabled, `sendSensorData()` calls `bufferSensorData()`
- [ ] Data is being saved to buffer (check serial output: `✓ Buffered data`)
- [ ] `BufferManager::getStatus()` shows entries increasing
- [ ] When network is restored, buffered data is sent
- [ ] After successful send, buffer is cleared
- [ ] Buffer status shows 0 entries after recovery

---

## Troubleshooting

### Compilation Errors

**Error: `'BufferManager' has no member named 'init'`**
- Make sure both `buffer_manager.h` and `buffer_manager.cpp` are in the `main/` folder
- Make sure `#include "buffer_manager.h"` is in your `.ino` file or relevant header

**Error: `SPIFFS.begin` not found**
- Add `#include <SPIFFS.h>` to `buffer_manager.cpp`
- Ensure SPIFFS is supported in your board definition

### Runtime Issues

**No data being buffered**
1. Check that `networkManager.isEthernetActive()` returns false
2. Verify `BufferManager::init()` returned true
3. Check serial output for buffer-related messages

**Buffered data not sending**
1. Verify network is actually available when recovery is attempted
2. Check that `sendBufferedData()` is being called (add debug lines)
3. Verify Django endpoint is accessible and responding

---

## Next Steps

1. Copy the code snippets into your project files
2. Compile and verify no errors
3. Test with network disabled
4. Test recovery when network is restored
5. Monitor buffer status over time
6. Adjust `MAX_BUFFER_SIZE` or `MAX_ENTRIES` if needed

See `BUFFERING_QUICKSTART.md` and `documentation/DATA_BUFFERING_GUIDE.md` for more details!
