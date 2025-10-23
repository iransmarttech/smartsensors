/*
 * Simple MR007 Sensor Test for ESP32
 * Basic functionality check to verify sensor health
 * Output pin: GPIO36 (ADC1_CH0)
 */

#include <Arduino.h>

// Pin definitions
#define SENSOR_OUTPUT_PIN 36   // GPIO36 for sensor data reading

void setup() {
  Serial.begin(115200);
  delay(1000); // Wait for serial connection
  
  Serial.println("====================================");
  Serial.println("MR007 Sensor Basic Test");
  Serial.println("====================================");
  Serial.println("Testing sensor on GPIO36...");
  
  // Configure ADC
  analogReadResolution(12); // Set to 12-bit resolution
  analogSetAttenuation(ADC_11db); // Set attenuation to 0-3.3V range
  
  Serial.println("Sensor test initialized");
  Serial.println("Waiting for sensor to stabilize...");
  delay(3000); // Allow sensor to stabilize
}

void loop() {
  // Read raw ADC value
  int rawValue = analogRead(SENSOR_OUTPUT_PIN);
  
  // Convert to voltage (ESP32 reference voltage is 3.3V)
  float voltage = (rawValue * 3.3) / 4095.0;
  
  // Display results
  Serial.print("Raw ADC: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V | Millivolts: ");
  Serial.print(voltage * 1000, 1);
  Serial.println("mV");
  
  // Basic sensor health check
  if (voltage < 0.1) {
    Serial.println("WARNING: Very low voltage - check connections");
  } else if (voltage > 3.2) {
    Serial.println("WARNING: Voltage near maximum - check circuit");
  } else {
    Serial.println("Sensor reading appears normal");
  }
  
  Serial.println("------------------------------------");
  delay(2000); // Wait 2 seconds between readings
}