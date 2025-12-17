#include "buffer_manager.h"
#include <SPIFFS.h>
#include "config.h"

const char* BufferManager::BUFFER_FILE = "/data_buffer.jsonl";
const size_t BufferManager::MAX_BUFFER_SIZE = 512000;  // 500KB max
const size_t BufferManager::MAX_ENTRIES = 2000;         // Reasonable limit

bool BufferManager::init() {
    DEBUG_PRINTLN("Initializing buffer manager...");
    
    if (!SPIFFS.begin(true)) {
        DEBUG_PRINTLN("✗ SPIFFS initialization failed");
        return false;
    }
    
    DEBUG_PRINTLN("✓ SPIFFS initialized");
    
    // Check total and used space
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    DEBUG_PRINT("  Total: ");
    DEBUG_PRINT(totalBytes);
    DEBUG_PRINT(" bytes, Used: ");
    DEBUG_PRINT(usedBytes);
    DEBUG_PRINTLN(" bytes");
    
    // Check if buffer file exists, if not create it
    if (!SPIFFS.exists(BUFFER_FILE)) {
        File f = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
        if (f) {
            f.close();
            DEBUG_PRINTLN("✓ Buffer file created: " + String(BUFFER_FILE));
        } else {
            DEBUG_PRINTLN("✗ Failed to create buffer file");
            return false;
        }
    } else {
        DEBUG_PRINTLN("✓ Buffer file exists: " + String(BUFFER_FILE));
    }
    
    return true;
}

bool BufferManager::saveData(const SharedSensorData& data, unsigned long timestamp) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        DEBUG_PRINTLN("✗ Buffer file not found");
        return false;
    }
    
    // Check if we're at capacity
    size_t entries = getEntryCount();
    if (entries >= MAX_ENTRIES) {
        DEBUG_PRINTLN("⚠ Buffer at max entries (" + String(MAX_ENTRIES) + "), not saving");
        return false;
    }
    
    size_t currentSize = getBufferSize();
    if (currentSize >= MAX_BUFFER_SIZE) {
        DEBUG_PRINTLN("⚠ Buffer is full (" + String(MAX_BUFFER_SIZE) + " bytes), not saving");
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
        DEBUG_PRINTLN("✗ Buffer file does not exist");
        return false;
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_APPEND);
    if (!f) {
        DEBUG_PRINTLN("✗ Failed to open buffer file for writing");
        return false;
    }
    
    size_t written = f.println(jsonData);  // Add newline to separate entries
    f.close();
    
    DEBUG_PRINT("✓ Buffered data: ");
    DEBUG_PRINT(jsonData.length());
    DEBUG_PRINT(" bytes, Total entries: ");
    DEBUG_PRINTLN(getEntryCount());
    
    return written > 0;
}

String BufferManager::getBufferedEntries(size_t maxEntries) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return "[]";
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!f) {
        return "[]";
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
    } else {
        DEBUG_PRINTLN("✗ Failed to clear buffer");
        return false;
    }
    return false;
}

bool BufferManager::removeEntries(size_t count) {
    if (!SPIFFS.exists(BUFFER_FILE)) {
        return false;
    }
    
    File f = SPIFFS.open(BUFFER_FILE, FILE_READ);
    if (!f) {
        DEBUG_PRINTLN("✗ Failed to open buffer file");
        return false;
    }
    
    String tempContent = "";
    size_t skipped = 0;
    size_t totalLines = 0;
    
    while (f.available()) {
        String line = f.readStringUntil('\n');
        totalLines++;
        
        if (skipped < count) {
            skipped++;
        } else if (line.length() > 0) {
            tempContent += line + "\n";
        }
    }
    f.close();
    
    // Write back filtered content
    f = SPIFFS.open(BUFFER_FILE, FILE_WRITE);
    if (!f) {
        DEBUG_PRINTLN("✗ Failed to open buffer file for writing");
        return false;
    }
    
    f.print(tempContent);
    f.close();
    
    DEBUG_PRINT("✓ Removed ");
    DEBUG_PRINT(skipped);
    DEBUG_PRINT(" entries (kept ");
    DEBUG_PRINT(totalLines - skipped);
    DEBUG_PRINTLN(")");
    
    return true;
}

String BufferManager::getStatus() {
    size_t entries = getEntryCount();
    size_t bytes = getBufferSize();
    uint8_t percentFull = getUsagePercent();
    
    String status = "Buffer Status:\n";
    status += "  Entries: " + String(entries) + " / " + String(MAX_ENTRIES) + "\n";
    status += "  Size: " + String(bytes) + " / " + String(MAX_BUFFER_SIZE) + " bytes\n";
    status += "  Usage: " + String(percentFull) + "%\n";
    
    if (percentFull > 80) {
        status += "  ⚠ WARNING: Buffer nearly full!\n";
    }
    
    return status;
}

bool BufferManager::hasData() {
    return getEntryCount() > 0;
}

bool BufferManager::isAlmostFull() {
    size_t bytes = getBufferSize();
    return (bytes * 100 / MAX_BUFFER_SIZE) > 80;
}

uint8_t BufferManager::getUsagePercent() {
    size_t bytes = getBufferSize();
    return (bytes * 100) / MAX_BUFFER_SIZE;
}
