/**
 * Simple Test Code for ME4-SO2 Sulfur Dioxide Sensor
 * ESP32 with ME4-SO2 Sensor Test
 * Connect sensor output to GPIO 36
 */

// Pin definitions
#define SENSOR_PIN 36

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);  // Set ADC to 12-bit resolution
  delay(2000);
  
  Serial.println("ME4-SO2 Sensor Test");
  Serial.println("-------------------");
}

void loop() {
  // Read raw analog value
  int rawValue = analogRead(SENSOR_PIN);
  
  // Convert to voltage (ESP32 uses 3.3V reference)
  float voltage = (rawValue * 3.3) / 4095.0;
  
  // Calculate current (using 10Ω load resistor as per datasheet)
  float current = (voltage / 10.0) * 1000000.0; // Convert to μA
  
  // Estimate SO2 concentration (using typical sensitivity 0.8 μA/ppm)
  float ppm = current / 0.8;
  
  // Display results
  Serial.print("Raw ADC: ");
  Serial.print(rawValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 3);
  Serial.print("V | Current: ");
  Serial.print(current, 1);
  Serial.print("μA | SO2: ");
  Serial.print(ppm, 2);
  Serial.println(" ppm");
  
  // Simple sensor health check
  if (voltage < 0.1) {
    Serial.println("Warning: Very low voltage - check connections");
  } else if (voltage > 3.2) {
    Serial.println("Warning: Voltage near maximum - sensor may be saturated");
  } else if (abs(current) < 1.0) {
    Serial.println("Sensor appears to be working normally in clean air");
  }
  
  delay(2000); // Wait 2 seconds between readings
}