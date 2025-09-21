#include <HardwareSerial.h>

// Pin Configuration
const int dacPin = 34;          // ESP32 GPIO34 → Sensor DAC (PIN2)
const int sensorRXPin = 16;     // ESP32 GPIO16 → Sensor TX (PIN6)
const int sensorTXPin = 17;     // ESP32 GPIO17 → Voltage Divider → Sensor RX (PIN5)

// Create hardware serial instance
HardwareSerial sensorSerial(1);  // Use UART1 on ESP32

void setup() {
  Serial.begin(115200);
  
  // Initialize sensor serial communication
  sensorSerial.begin(9600, SERIAL_8N1, sensorRXPin, sensorTXPin);
  
  // Configure ADC for ESP32
  analogReadResolution(12);
  
  Serial.println("ZE40-TVOC Sensor Basic Test");
  Serial.println("===========================");
  Serial.println("Checking sensor communication...");
  
  // Wait a moment for sensor to initialize
  delay(1000);
  
  // Send command to set initiative upload mode
  byte initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
  sensorSerial.write(initCmd, 9);
  
  Serial.println("Sensor initialized. Reading data...");
}

void loop() {
  // Read DAC voltage
  int adcValue = analogRead(dacPin);
  float voltage = adcValue * (3.3 / 4095.0);
  
  // Convert to ppm (0.4-2V → 0-5ppm)
  float ppm = 0;
  if (voltage >= 0.4) {
    ppm = (voltage - 0.4) * (5.0 / 1.6); // 2.0V - 0.4V = 1.6V range
  }
  
  // Print DAC readings
  Serial.print("DAC Voltage: ");
  Serial.print(voltage, 2);
  Serial.print("V → TVOC: ");
  Serial.print(ppm, 3);
  Serial.println(" ppm");
  
  // Check for UART data
  if (sensorSerial.available()) {
    Serial.print("UART Data: ");
    while (sensorSerial.available()) {
      byte data = sensorSerial.read();
      Serial.print("0x");
      if (data < 0x10) Serial.print("0");
      Serial.print(data, HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  // Wait before next reading
  delay(2000);
}