# ESP32-WROOM to ESP32-S3 Porting Guide

## Critical Differences

### 1. Global Object Construction ⚠️ MOST IMPORTANT

**ESP32-WROOM:** Tolerates global objects with internal mutexes  
**ESP32-S3:** CRASHES if RTOS objects created before scheduler starts

#### ❌ DON'T DO THIS:
```cpp
// Global construction - happens before main()
EthernetServer server(80);
WiFiServer wifiServer(80);
SemaphoreHandle_t mySemaphore = xSemaphoreCreateMutex(); // CRASH!

void setup() {
    server.begin(); // Too late, internal mutex already corrupted
}
```

#### ✅ DO THIS INSTEAD:
```cpp
// Use pointers, no construction
EthernetServer* server = nullptr;
WiFiServer* wifiServer = nullptr;

void setup() {
    // Now FreeRTOS is running, safe to create RTOS objects
    server = new EthernetServer(80);
    server->begin();
}
```

### 2. Mutex/Semaphore Allocation

**ESP32-WROOM:** Dynamic allocation usually works  
**ESP32-S3:** Prefer static allocation

#### ❌ Risky on ESP32-S3:
```cpp
SemaphoreHandle_t mutex = xSemaphoreCreateMutex();
```

#### ✅ Better for ESP32-S3:
```cpp
static StaticSemaphore_t mutexBuffer;
SemaphoreHandle_t mutex = xSemaphoreCreateMutexStatic(&mutexBuffer);
```

### 3. ISR Context Protection

**ESP32-S3** has stricter validation of ISR vs Task context

#### ✅ Always check before using mutexes:
```cpp
bool lockData() {
    if (xPortInIsrContext()) {
        return false; // Can't use mutex from ISR!
    }
    return xSemaphoreTake(mutex, timeout) == pdTRUE;
}
```

### 4. SPI Timing

**ESP32-S3** requires explicit delays for bus stabilization

```cpp
// Initialize SPI
SPI.begin(SCK, MISO, MOSI, CS);
delay(100); // ✅ Add delay for ESP32-S3

Ethernet.init(CS);
delay(100); // ✅ Add another delay before DHCP
```

### 5. Task Stack Sizes

**ESP32-S3** may require larger stack sizes

```cpp
// ESP32-WROOM
#define ETH_TASK_STACK_SIZE 16384

// ESP32-S3
#define ETH_TASK_STACK_SIZE 20480  // ✅ Increase by 25%
```

### 6. Memory Management

**ESP32-S3** has different PSRAM/SPIRAM configuration

- Be more careful with heap fragmentation
- Use static allocation when possible
- Monitor free heap regularly

### 7. Client Object Handling

**Never store pointers to stack-allocated Client objects!**

#### ❌ DANGEROUS:
```cpp
EthernetClient client = server.available();
myStruct.clientPtr = &client; // ⚠️ client goes out of scope!
// Later access to clientPtr = CRASH
```

#### ✅ SAFE:
```cpp
EthernetClient client = server.available();
// Process immediately, don't store pointers
handleRequest(client);
client.stop();
// Done, no dangling pointers
```

## Quick Checklist for Porting

- [ ] Move all Server objects to pointers with delayed construction
- [ ] Use static mutex allocation
- [ ] Add ISR context checks to all mutex operations
- [ ] Add SPI initialization delays
- [ ] Increase task stack sizes by 25%
- [ ] Remove any stored pointers to stack Client objects
- [ ] Test thoroughly with Ethernet first (most problematic)
- [ ] Monitor serial output for assertion failures
- [ ] Check for memory leaks over extended runtime

## Common Error Messages

### `assert failed: xQueueSemaphoreTake queue.c:1709`
**Cause:** Mutex/semaphore accessed before initialization or from ISR  
**Fix:** Use static allocation, add ISR checks, delay server construction

### `Guru Meditation Error: Core 0 panic'ed (LoadProhibited)`
**Cause:** Accessing invalid memory (often dangling pointers)  
**Fix:** Remove stored pointers to stack variables

### Stack overflow in task
**Cause:** Insufficient stack size  
**Fix:** Increase task stack size in config

## Debugging Tips

1. **Enable verbose FreeRTOS errors:**
   - In Arduino IDE: Tools → Core Debug Level → Verbose
   
2. **Monitor stack usage:**
```cpp
UBaseType_t stackRemaining = uxTaskGetStackHighWaterMark(NULL);
Serial.printf("Stack remaining: %d\n", stackRemaining);
```

3. **Check heap:**
```cpp
Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
```

4. **Backtrace decoder:**
   - Use ESP32 Exception Decoder tool
   - Helps identify exact crash location

## Summary

The key to successful ESP32-S3 porting is understanding that it has:
- **Stricter RTOS initialization requirements**
- **Better error detection** (catches bugs that WROOM missed)
- **Different timing requirements** for peripherals
- **Less tolerance for heap fragmentation**

These are actually **improvements** - the ESP32-S3 catches bugs that silently corrupted memory on ESP32-WROOM!

---
**Remember:** If it crashes on ESP32-S3 but worked on WROOM, the code probably had a bug that WROOM just didn't catch.
