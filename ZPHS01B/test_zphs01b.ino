/*
 * ZPHS01B Sensor Basic Test
 * Checks if sensor is responding properly and displays raw data
 * 
 * Wiring:
 * ZPHS01B VCC -> ESP32 5V
 * ZPHS01B GND -> ESP32 GND
 * ZPHS01B TX  -> ESP32 GPIO 16 (RX2)
 * ZPHS01B RX  -> ESP32 GPIO 17 (TX2)
 */

#include <Arduino.h>

// Configuration
#define SENSOR_BAUD_RATE   9600
#define RESPONSE_LENGTH    26
#define READ_INTERVAL_MS   5000
#define RX2_PIN            16
#define TX2_PIN            17

// Sensor command to request data
const uint8_t REQUEST_DATA_CMD[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  Serial2.begin(SENSOR_BAUD_RATE, SERIAL_8N1, RX2_PIN, TX2_PIN);
  
  // Wait for serial port to connect (for ESP32)
  delay(1000);
}

void loop() {
  static unsigned long lastReadTime = 0;
  
  // Read sensor every 5 seconds
  if (millis() - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = millis();
    
    // Send request command to sensor
    Serial2.write(REQUEST_DATA_CMD, sizeof(REQUEST_DATA_CMD));
    delay(200);  // Allow sensor processing time

    // Read response from sensor
    uint8_t rawData[RESPONSE_LENGTH] = {0};
    size_t bytesRead = Serial2.readBytes(rawData, RESPONSE_LENGTH);

    // Display results
    if (bytesRead == RESPONSE_LENGTH) {
      Serial.println("Sensor responded successfully!");
      Serial.print("Raw data (HEX): ");
      
      // Print raw hexadecimal data
      for (int i = 0; i < RESPONSE_LENGTH; i++) {
        if (rawData[i] < 0x10) Serial.print("0");
        Serial.print(rawData[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      
      // Display a few key values for quick verification
      uint16_t pm25 = (rawData[4] << 8) | rawData[5];
      uint16_t co2 = (rawData[8] << 8) | rawData[9];
      double temperature = ((rawData[11] << 8 | rawData[12]) - 500) * 0.1;
      
      Serial.print("PM2.5: "); Serial.print(pm25); Serial.println(" μg/m³");
      Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
      Serial.print("Temp: "); Serial.print(temperature, 1); Serial.println(" °C");
      Serial.println("------------------------------------");
    } else {
      Serial.println("Error: No response from sensor or incomplete data");
      Serial.print("Expected 26 bytes, received: "); Serial.println(bytesRead);
      
      // Clear serial buffer
      while (Serial2.available()) Serial2.read();
    }
  }
  
  // Check for user input to send test command
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 't' || cmd == 'T') {
      Serial.println("Manual test command sent to sensor");
      Serial2.write(REQUEST_DATA_CMD, sizeof(REQUEST_DATA_CMD));
    }
  }
}