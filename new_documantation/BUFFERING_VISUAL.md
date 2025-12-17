# Data Buffering - Visual Overview

## System Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                     ESP32 BOARD                             │
├─────────────────────────────────────────────────────────────┤
│                                                              │
│  ┌──────────────┐                                            │
│  │   SENSORS    │                                            │
│  │   ZE40       │                                            │
│  │   ZPHS01B    │──────────┐                                │
│  │   MR007      │          │                                │
│  │   ME4-SO2    │          ▼                                │
│  └──────────────┘    ┌──────────────┐                       │
│                      │ shared_data  │                       │
│                      │  (mutex)     │                       │
│                      └──────────────┘                       │
│                            │                                 │
│                ┌───────────┴───────────┐                     │
│                │                       │                     │
│                ▼                       ▼                     │
│        ┌──────────────┐        ┌──────────────┐             │
│        │  Django      │        │   Buffer     │             │
│        │  Client      │        │   Manager    │             │
│        │              │        │              │             │
│        │ Try POST to  │        │ Save to      │             │
│        │ 192.168.1.4  │        │ /data_buffer │             │
│        │              │        │ (SPIFFS)     │             │
│        └──────────────┘        └──────────────┘             │
│                │                       ▲                     │
│    Success?    │ Fail?                │                      │
│         Yes    │ Yes                  │                      │
│         │      │                      │                      │
│         │      └──────► Buffer Data ──┘                      │
│         │                                                    │
│         │         ┌──────────────────┐                      │
│         │         │  Network Status  │                      │
│         │         │  Available?      │                      │
│         │         └──────────────────┘                      │
│         │              │      ▲                             │
│         │              │ Yes  │ No                          │
│         │              ▼      │                             │
│         │         ┌──────────┐│                             │
│         │         │ Send All ││ Wait & Retry              │
│         │         │ Buffered ││ in 30 seconds              │
│         │         │ Data     ││                             │
│         │         └──────────┘│                             │
│         │              │       │                             │
│         └──────────────┼───────┘                             │
│                        │                                     │
│                        ▼                                     │
│              ┌──────────────────┐                           │
│              │  All Data Sent   │                           │
│              │  To Django       │                           │
│              │  & Buffer Clear  │                           │
│              └──────────────────┘                           │
│                        │                                     │
│                        ▼                                     │
│                  ┌──────────────┐                           │
│                  │   Django     │                           │
│                  │   Receives   │                           │
│                  │   All Data   │                           │
│                  └──────────────┘                           │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

---

## Data Flow: Normal Operation (Network Available)

```
Time: 14:00:00

Sensor Reading → Build JSON → Send to Django → Success!
                                    │
                                    ▼
                          Django received
                          ✓ Store in database
                          ✓ Make available to React
                          
React displays the data in dashboard
```

---

## Data Flow: Network Failure (Buffering)

```
Time: 14:00:00 - 14:15:00
(Network down)

Reading 1 → Try POST → FAIL → Buffer to SPIFFS → Continue
Reading 2 → Try POST → FAIL → Buffer to SPIFFS → Continue
Reading 3 → Try POST → FAIL → Buffer to SPIFFS → Continue
...
Reading 15 → Try POST → FAIL → Buffer to SPIFFS → Continue

Buffer now contains 15 entries in /data_buffer.jsonl file
```

---

## Data Flow: Network Recovery (Auto-Recovery)

```
Time: 14:15:00
(Network comes back)

Task Manager detects network available
    ↓
Calls sendBufferedData()
    ↓
Reads all 15 entries from buffer
    ↓
Formats as JSON array: [{...}, {...}, ..., {...}]
    ↓
POSTs to Django with all 15 readings
    ↓
Django receives and processes all data
    ↓
React dashboard updates with all 15 readings
    ↓
Buffer cleared - ready for next cycle

NO DATA LOSS! ✓
```

---

## Storage Format

### File Path
```
/data_buffer.jsonl  (in SPIFFS flash memory)
```

### File Content (JSON Lines Format)
```json
{"timestamp":1702916400,"ze40":{"tvoc_ppb":45.2,"tvoc_ppm":0.045,...},"air_quality":{"pm1":12,"pm25":22.5,...},...}
{"timestamp":1702916460,"ze40":{"tvoc_ppb":46.1,"tvoc_ppm":0.046,...},"air_quality":{"pm1":13,"pm25":23.1,...},...}
{"timestamp":1702916520,"ze40":{"tvoc_ppb":45.8,"tvoc_ppm":0.046,...},"air_quality":{"pm1":12,"pm25":22.8,...},...}
```

### Transmission Format (When Sending)
```json
[
  {"timestamp":1702916400,"ze40":{...},"air_quality":{...},...},
  {"timestamp":1702916460,"ze40":{...},"air_quality":{...},...},
  {"timestamp":1702916520,"ze40":{...},"air_quality":{...},...}
]
```

---

## Capacity & Timing

### Storage Capacity

```
Max Buffer Size: 500 KB
Avg Entry Size: 350 bytes
Max Entries: 2000

┌─────────────────────────────────┐
│ 500,000 bytes ÷ 350 bytes/entry │
│ = ~1,428 entries                │
└─────────────────────────────────┘

Practical capacity: ~1,000 - 1,400 readings
```

### How Long Can Buffer Hold Data?

```
Sampling Rate → Time Buffered

Every minute:    1000 readings = ~16-17 hours
Every 5 min:     1000 readings = ~80+ hours (3+ days)
Every 10 min:    1000 readings = ~160+ hours (6+ days)
Every hour:      1000 readings = ~40+ days

You can collect data for DAYS without sending!
```

---

## API Method Calls

### Initialize (Setup)
```cpp
BufferManager::init()
├─ Initialize SPIFFS
├─ Create buffer file if needed
└─ Return: true if successful
```

### Save Data When Network Unavailable
```cpp
BufferManager::saveData(const SharedSensorData& data)
├─ Check capacity
├─ Convert sensor data to JSON
├─ Write to /data_buffer.jsonl
└─ Return: true if saved

Alternative:
BufferManager::saveJSON(const String& jsonData)
├─ Write raw JSON to buffer
└─ Return: true if saved
```

### Retrieve Data
```cpp
BufferManager::getBufferedEntries()
├─ Read all entries from file
├─ Format as JSON array
└─ Return: "[{...}, {...}, ...]"

BufferManager::getEntryCount()
├─ Count lines in file
└─ Return: size_t (number of entries)

BufferManager::getBufferSize()
├─ Get file size
└─ Return: size_t (bytes)
```

### Send & Cleanup
```cpp
BufferManager::clearBuffer()
├─ Delete /data_buffer.jsonl
├─ Recreate empty file
└─ Return: true if successful

BufferManager::removeEntries(size_t count)
├─ Remove first N entries (oldest)
└─ Return: true if successful
```

### Monitoring
```cpp
BufferManager::getStatus()
├─ Return formatted string with:
│   - Number of entries
│   - Buffer size in bytes
│   - Usage percentage
│   - Warning if nearly full
└─ Return: String

BufferManager::hasData()
├─ Return: true if buffer has entries
└─ Return: bool

BufferManager::isAlmostFull()
├─ Return: true if usage > 80%
└─ Return: bool

BufferManager::getUsagePercent()
├─ Return: 0-100
└─ Return: uint8_t
```

---

## Configuration Constants

```cpp
// In buffer_manager.cpp:

const char* BUFFER_FILE = "/data_buffer.jsonl"
  ↳ Path to buffer file in SPIFFS

const size_t MAX_BUFFER_SIZE = 512000  // 500 KB
  ↳ Stop buffering when this size is reached
  ↳ Can increase up to ~1 MB if needed

const size_t MAX_ENTRIES = 2000
  ↳ Maximum number of JSON entries to buffer
  ↳ Prevents infinite growth
  ↳ Can increase if you have free flash space
```

---

## Integration Points

### Point 1: Initialize Buffer
```cpp
// In main.ino setup()
BufferManager::init()
```

### Point 2: Try to Buffer on Send Failure
```cpp
// In django_client.cpp sendSensorData()
if (sendHTTPPOST fails) {
    bufferSensorData(sharedData);
}
```

### Point 3: Send Buffered Data
```cpp
// In task_manager.cpp sensorTask()
Every 30 seconds:
    sendBufferedData();
```

---

## Error Handling & Recovery

```
Try to Send Data
    │
    ├─ Success → Clear buffer, move on ✓
    │
    └─ Fail
        │
        ├─ No network → Buffer for later ✓
        ├─ Server error → Buffer for retry ✓
        ├─ Connection timeout → Buffer for retry ✓
        └─ Buffer full → Discard new data ⚠

Later...

Buffer has data + Network available
    │
    ├─ Send all buffered entries
    │
    ├─ Success → Clear buffer ✓
    │
    └─ Fail → Keep trying in 30 seconds
```

---

## Memory Impact

### Flash Storage
```
SPIFFS available: 1.4 MB (typical)
Buffer allocation: 500 KB (configurable)
Used by app: ~200-300 KB
Free space: ~400-700 KB

✓ Plenty of room, no issues
```

### RAM Usage
```
Buffer manager objects: ~2 KB
File handles: ~1 KB
Temporary strings: ~5 KB

Total: ~8 KB (minimal impact)
```

---

## Monitoring & Debugging

### Check Status in Real-Time
```cpp
Serial.println(BufferManager::getStatus());

Output:
Buffer Status:
  Entries: 42 / 2000
  Size: 15234 / 512000 bytes
  Usage: 3%
```

### View All Buffered Data
```cpp
String data = BufferManager::getBufferedEntries();
Serial.println(data);

Output:
[{"timestamp":..., "ze40":{...}}, {...}, ...]
```

### Check Individual Values
```cpp
if (BufferManager::hasData()) {
    // Data is buffered
}

if (BufferManager::isAlmostFull()) {
    // Buffer is 80%+ full, take action
}

uint8_t percent = BufferManager::getUsagePercent();
// 0-100
```

---

## Troubleshooting Decision Tree

```
Data not being buffered?
├─ BufferManager::init() called? ❌
│  └─ Add to setup()
├─ Network actually unavailable? ❌
│  └─ Check network status
└─ Buffer disk full? ❌
   └─ Call removeEntries() or clearBuffer()

Buffer growing too large?
├─ MAX_BUFFER_SIZE too high? ❌
│  └─ Lower the constant in buffer_manager.cpp
├─ Data not being sent? ❌
│  └─ Check network and sendBufferedData()
└─ Too frequent readings? ❌
   └─ Increase SEND_INTERVAL

Buffered data not sending?
├─ Network available? ❌
│  └─ Check networkManager status
├─ sendBufferedData() called? ❌
│  └─ Add to task_manager periodic loop
└─ Django endpoint responding? ❌
   └─ Test with curl first
```

---

## Timeline Example: 7-Day Offline Scenario

```
Day 1 - 7: Device offline but collecting data
  ↓
 Every 10 minutes: 1 reading added to buffer
  ↓
 Total after 7 days: ~1,008 readings (10,080 minutes ÷ 10)
  ↓
 Total file size: ~350 KB (within 500 KB limit)

Day 8: Network comes back online
  ↓
 sendBufferedData() is called
  ↓
 All 1,008 readings sent as one JSON array
  ↓
 Django receives and stores all readings
  ↓
 React dashboard updates with all 7 days of data
  ↓
 User can see: Temperature changes, trends, patterns over 7 days
  ↓
 NO DATA LOSS ✓
```

---

## Files Created

```
smartsensors/
├── main/
│   ├── buffer_manager.h           ← NEW
│   ├── buffer_manager.cpp         ← NEW
│   └── ... other files ...
│
├── documentation/
│   └── DATA_BUFFERING_GUIDE.md    ← NEW (detailed guide)
│
├── BUFFERING_QUICKSTART.md         ← NEW (5-step setup)
├── BUFFERING_INTEGRATION.md        ← NEW (copy-paste code)
└── ... other files ...
```

---

## Implementation Checklist

```
Phase 1: Setup
  ☐ Copy buffer_manager.h to main/
  ☐ Copy buffer_manager.cpp to main/
  ☐ Add #include "buffer_manager.h" to main files

Phase 2: Initialize
  ☐ Add BufferManager::init() to main.ino setup()
  ☐ Test compilation

Phase 3: Integration
  ☐ Add bufferSensorData() to django_client.cpp
  ☐ Add sendBufferedData() to django_client.cpp
  ☐ Modify sendSensorData() to buffer on failure
  ☐ Add periodic sendBufferedData() to task_manager.cpp
  ☐ Test compilation

Phase 4: Testing
  ☐ Disable network, verify data gets buffered
  ☐ Enable network, verify buffered data is sent
  ☐ Check buffer status during operation
  ☐ Verify no data loss after reboot

Phase 5: Monitoring
  ☐ Add buffer status to web server endpoint
  ☐ Monitor buffer size over time
  ☐ Adjust MAX_BUFFER_SIZE if needed
```

---

## Next Steps

1. **Read BUFFERING_QUICKSTART.md** - Overview and quick setup
2. **Read BUFFERING_INTEGRATION.md** - Copy ready-to-use code snippets
3. **Read documentation/DATA_BUFFERING_GUIDE.md** - Deep dive into options
4. **Copy code files** - buffer_manager.h and buffer_manager.cpp
5. **Integrate** - Follow the 5-step checklist
6. **Test** - Verify with network disabled
7. **Deploy** - Compile and upload to board
