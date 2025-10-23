/*
 * MR007 Flammable Gas Sensor Interface for ESP32
 * Based on Winsen MR007 datasheet version 1.1 (2023-07-13)
 * Output pin: GPIO36 (ADC1_CH0)
 */

#include <Arduino.h>
#include <driver/adc.h>

// Sensor constants from datasheet
#define ADC_REFERENCE_VOLTAGE 3.3    // ESP32 reference voltage
#define ADC_RESOLUTION 4095.0        // 12-bit ADC resolution
#define SENSOR_WORKING_VOLTAGE 2.5   // MR007 required operating voltage

// Pin definitions - according to MR007 datasheet pinout
#define SENSOR_POWER_PIN 12    // Pin 1: Positive supply (2.5V required)
#define SENSOR_GND_PIN 13      // Pin 3: Ground
#define SENSOR_OUTPUT_PIN 36   // Pins 2+4: Connected together as output (GPIO36)

class MR007 {
  private:
    int outputPin;
    float sensitivityMin;  // Minimum sensitivity voltage at 20% LEL (V)
    float sensitivityMax;  // Maximum sensitivity voltage at 20% LEL (V)
    float lelThreshold;    // LEL percentage for warning
    String gasName;        // Name of detected gas
    float zeroPoint;       // Calibrated zero point voltage
    
    // Apply temperature compensation based on datasheet recommendations
    float temperatureCompensation(float voltage, float temperature = 25.0) {
      // Typical compensation factor (~0.1% per °C)
      return voltage * (1.0 + 0.001 * (temperature - 25.0));
    }
    
  public:
    // Constructor with parameters from datasheet
    MR007(int pin, float minSensitivity, float maxSensitivity, float threshold, String name) 
      : outputPin(pin), sensitivityMin(minSensitivity/1000.0), // Convert mV to V
        sensitivityMax(maxSensitivity/1000.0), lelThreshold(threshold), 
        gasName(name), zeroPoint(0.0) {}
    
    // Initialize sensor power pins
    void begin() {
      pinMode(SENSOR_POWER_PIN, OUTPUT);
      pinMode(SENSOR_GND_PIN, OUTPUT);
      digitalWrite(SENSOR_GND_PIN, LOW);
      
      // Provide precise 2.5V using PWM (external filter circuit recommended)
      // For accurate measurements, use external 2.5V regulator
      dacWrite(SENSOR_POWER_PIN, 205); // Approximate 2.5V output (205/255 * 3.3V ≈ 2.65V)
      
      // Configure ADC for accurate reading
      adc1_config_width(ADC_WIDTH_BIT_12);
      adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11); // 0-3.1V range
      
      // Warm-up time as recommended in datasheet
      delay(2000);
    }
    
    // Read sensor output voltage
    float readVoltage() {
      int rawValue = adc1_get_raw(ADC1_CHANNEL_0);
      return (rawValue * ADC_REFERENCE_VOLTAGE) / ADC_RESOLUTION;
    }
    
    // Calculate LEL percentage based on sensitivity curve
    float calculateLEL(float voltage, float temperature = 25.0) {
      // Apply temperature compensation
      float compVoltage = temperatureCompensation(voltage, temperature);
      
      // Subtract zero point offset
      float adjustedVoltage = compVoltage - zeroPoint;
      
      // Calculate LEL based on sensitivity characteristics
      // Using linear approximation from datasheet curve
      if (adjustedVoltage <= 0) return 0.0;
      
      float lel = (adjustedVoltage / ((sensitivityMax + sensitivityMin) / 2)) * 20.0;
      return constrain(lel, 0.0, 100.0);
    }
    
    // Check if LEL exceeds safety threshold
    void checkSafety(float lel) {
      if (lel >= lelThreshold) {
        Serial.print("DANGER: ");
        Serial.print(gasName);
        Serial.println(" concentration at explosive level!");
        // Add emergency actions here (alarm, ventilation, etc.)
      } else if (lel >= lelThreshold * 0.5) {
        Serial.print("WARNING: ");
        Serial.print(gasName);
        Serial.println(" concentration approaching dangerous level!");
      }
    }
    
    // Calibrate zero point (should be done in clean air)
    void calibrateZero() {
      Serial.println("Calibrating zero point in clean air...");
      Serial.println("Ensure sensor is in clean environment and wait 30 seconds");
      delay(30000);
      
      // Read multiple samples for stability
      float sum = 0;
      for(int i = 0; i < 20; i++) {
        sum += readVoltage();
        delay(100);
      }
      zeroPoint = sum / 20.0;
      
      Serial.print("Zero point calibration complete: ");
      Serial.print(zeroPoint * 1000, 1);
      Serial.println("mV");
    }
    
    // Display current sensor status
    void displayStatus() {
      float voltage = readVoltage();
      float lel = calculateLEL(voltage);
      
      Serial.print(gasName);
      Serial.print(" - Raw Voltage: ");
      Serial.print(voltage * 1000, 1); // Display in mV
      Serial.print("mV, Adjusted: ");
      Serial.print((voltage - zeroPoint) * 1000, 1);
      Serial.print("mV, LEL: ");
      Serial.print(lel, 1);
      Serial.println("%");
      
      checkSafety(lel);
    }
    
    // Get current LEL value
    float getLEL() {
      return calculateLEL(readVoltage());
    }
};

// Create sensor instances with parameters from datasheet
// MR007(meterPin, minSensitivity(mV), maxSensitivity(mV), alarmThreshold(%), gasName)
MR007 methaneSensor(SENSOR_OUTPUT_PIN, 12.0, 45.0, 20.0, "Methane");

void setup() {
  Serial.begin(115200);
  Serial.println("MR007 Flammable Gas Sensor Initializing...");
  
  // Initialize sensor power supply
  methaneSensor.begin();
  
  // Display sensor info from datasheet
  Serial.println("==========================================");
  Serial.println("MR007 Hot-wire Gas Sensor Characteristics");
  Serial.println("==========================================");
  Serial.println("Operating Voltage: 2.5V ± 0.1V");
  Serial.println("Operating Current: 150mA ± 10mA");
  Serial.println("Response Time (T90): ≤10s");
  Serial.println("Recovery Time (T90): ≤30s");
  Serial.println("Measurement Range: 0-100% LEL");
  Serial.println("==========================================");
  
  // Calibrate zero point (perform in clean air)
  methaneSensor.calibrateZero();
}

void loop() {
  // Read and display sensor data
  methaneSensor.displayStatus();
  
  // Add non-blocking delay for stability
  static unsigned long lastRead = 0;
  if (millis() - lastRead >= 2000) {
    lastRead = millis();
  }
  
  delay(100); // Short delay for stability
}