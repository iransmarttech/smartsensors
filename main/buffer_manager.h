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
 * 
 * Usage:
 *   - Call init() in setup()
 *   - Call saveData() when network unavailable
 *   - Call getBufferedEntries() to retrieve buffered data
 *   - Call clearBuffer() after successful send
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
     * Call this once in setup()
     * @return true if initialization successful
     */
    static bool init();
    
    /**
     * Save current sensor data to buffer
     * Converts SharedSensorData to JSON and saves to flash
     * @param data Sensor data to buffer
     * @param timestamp Unix timestamp (0 = use current millis)
     * @return true if saved successfully
     */
    static bool saveData(const SharedSensorData& data, unsigned long timestamp = 0);
    
    /**
     * Save sensor data from JSON string
     * Useful for buffering already-formatted JSON
     * @param jsonData JSON string to save (will add newline)
     * @return true if saved successfully
     */
    static bool saveJSON(const String& jsonData);
    
    /**
     * Get all buffered entries as a JSON array
     * Format: [{"timestamp":..., "ze40":{...}}, {...}, ...]
     * @param maxEntries Maximum entries to retrieve (0 = all)
     * @return JSON array string, or "[]" if no data
     */
    static String getBufferedEntries(size_t maxEntries = 0);
    
    /**
     * Get number of buffered entries (lines in file)
     * @return Number of JSON entries currently buffered
     */
    static size_t getEntryCount();
    
    /**
     * Get total buffered data size in bytes
     * @return Size of buffer file
     */
    static size_t getBufferSize();
    
    /**
     * Clear all buffered data
     * Deletes and recreates the buffer file
     * @return true if successful
     */
    static bool clearBuffer();
    
    /**
     * Remove first N entries from buffer
     * Useful for partial cleanup
     * @param count Number of entries to remove
     * @return true if successful
     */
    static bool removeEntries(size_t count);
    
    /**
     * Get buffer status for monitoring
     * Returns formatted string with usage info
     * @return Status report
     */
    static String getStatus();
    
    /**
     * Check if buffer has any data
     * @return true if there are buffered entries
     */
    static bool hasData();
    
    /**
     * Check if buffer is nearly full
     * Useful for warnings or aggressive cleanup
     * @return true if usage > 80%
     */
    static bool isAlmostFull();
    
    /**
     * Get percentage of buffer used
     * @return 0-100
     */
    static uint8_t getUsagePercent();
};

#endif
