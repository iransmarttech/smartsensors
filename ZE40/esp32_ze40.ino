#include <HardwareSerial.h>

// Pin Configuration
const int dacPin = 34;          // ESP32 GPIO34 (ADC1_CH6) → Sensor DAC (PIN2)
const int sensorRXPin = 16;     // ESP32 GPIO16 → Sensor TX (PIN6)
const int sensorTXPin = 17;     // ESP32 GPIO17 → Voltage Divider → Sensor RX (PIN5)

// Create hardware serial instance
HardwareSerial sensorSerial(1);  // Use UART1 on ESP32

// Frame buffer
unsigned char dataFrame[9];
byte frameIndex = 0;
unsigned long lastByteTime = 0;
const unsigned int FRAME_TIMEOUT = 150;  // ms

// Preheating requirements
unsigned long powerOnTime = 0;
const unsigned long INITIAL_PREHEAT = 86400000;  // 24 hours in ms
const unsigned long DAILY_PREHEAT = 180000;      // 3 minutes in ms
bool initialPreheatComplete = false;

// DAC calibration
const float DAC_ZERO_VOLTAGE = 0.4;    // 0 ppm = 0.4V
const float DAC_FULLSCALE_VOLTAGE = 2.0; // 5 ppm = 2.0V
const float DAC_PPM_RANGE = 5.0;        // 0-5 ppm range

void setup() {
  Serial.begin(115200);
  
  // Initialize sensor serial communication
  sensorSerial.begin(9600, SERIAL_8N1, sensorRXPin, sensorTXPin);
  
  // Configure ADC for ESP32 (12-bit resolution)
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);  // 0-3.3V range
  
  Serial.println(F("\nZE40-TVOC Sensor with ESP32"));
  Serial.println(F("==========================="));
  Serial.println(F("Pin Connections:"));
  Serial.println(F("UART:"));
  Serial.println(F("  Sensor TX(PIN6) -> ESP32 GPIO16"));
  Serial.println(F("  Sensor RX(PIN5) -> Voltage Divider -> ESP32 GPIO17"));
  Serial.println(F("DAC:"));
  Serial.println(F("  Sensor DAC(PIN2) -> ESP32 GPIO34"));
  Serial.println(F("Power:"));
  Serial.println(F("  Sensor VIN(PIN4) -> 5V"));
  Serial.println(F("  Sensor GND(PIN3) -> GND"));
  Serial.println(F("\nPreheating started (24-48h required)..."));
  
  powerOnTime = millis();
  
  // Set to initiative upload mode
  byte initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
  sensorSerial.write(initCmd, 9);
}

float readDAC() {
  // Read analog voltage (12-bit ADC on ESP32)
  int adcValue = analogRead(dacPin);
  
  // Convert to voltage (ESP32 ADC with 3.3V reference)
  float voltage = adcValue * (3.3 / 4095.0);
  
  // Convert to ppm (0.4-2V → 0-5ppm)
  if (voltage < DAC_ZERO_VOLTAGE) return 0.0;
  return (voltage - DAC_ZERO_VOLTAGE) * (DAC_PPM_RANGE / (DAC_FULLSCALE_VOLTAGE - DAC_ZERO_VOLTAGE));
}

void switchToQAMode() {
  // Command to switch to Q&A mode
  byte qaCmd[9] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
  sensorSerial.write(qaCmd, 9);
  Serial.println(F("Switched to Q&A mode"));
}

void requestConcentration() {
  // Command to request concentration in Q&A mode
  byte requestCmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
  sensorSerial.write(requestCmd, 9);
}

void switchToInitiativeMode() {
  // Command to switch back to initiative upload mode
  byte initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
  sensorSerial.write(initCmd, 9);
  Serial.println(F("Switched to initiative upload mode"));
}

void loop() {
  // Handle preheating requirements
  unsigned long elapsed = millis() - powerOnTime;
  
  if (!initialPreheatComplete) {
    if (elapsed < INITIAL_PREHEAT) {
      if (elapsed % 60000 == 0) {  // Print every minute
        Serial.print(F("Initial preheating: "));
        Serial.print(elapsed / 3600000.0, 1);
        Serial.println(F(" hours elapsed (24-48h required)"));
      }
      return;
    } else {
      initialPreheatComplete = true;
      Serial.println(F("Initial preheating complete!"));
    }
  }
  
  // Daily preheating (3 minutes)
  if (elapsed % 86400000 < DAILY_PREHEAT) {  // Check first 3 minutes of each day
    if (elapsed % 10000 == 0) {
      Serial.print(F("Daily preheating: "));
      Serial.print((DAILY_PREHEAT - (elapsed % 86400000)) / 1000);
      Serial.println(F(" seconds remaining"));
    }
    return;
  }

  // Process incoming UART data
  while (sensorSerial.available()) {
    processByte(sensorSerial.read());
  }
  
  // Handle frame timeout
  if (frameIndex > 0 && millis() - lastByteTime > FRAME_TIMEOUT) {
    Serial.println(F("\n! Frame timeout - resetting buffer"));
    frameIndex = 0;
  }
  
  // Read DAC every 2 seconds
  static unsigned long lastDACRead = 0;
  if (millis() - lastDACRead >= 2000) {
    float dacPPM = readDAC();
    
    Serial.print(F("\n[DAC] Voltage: "));
    Serial.print(analogRead(dacPin) * (3.3 / 4095.0), 2);
    Serial.print(F("V → TVOC: "));
    Serial.print(dacPPM, 3);
    Serial.println(F(" ppm"));
    
    lastDACRead = millis();
  }
  
  // Example: Switch to Q&A mode and request reading every 30 seconds
  static unsigned long lastModeSwitch = 0;
  if (millis() - lastModeSwitch >= 30000) {
    switchToQAMode();
    delay(100);
    requestConcentration();
    delay(100);
    switchToInitiativeMode();  // Switch back after reading
    lastModeSwitch = millis();
  }
}

void processByte(byte b) {
  lastByteTime = millis();
  
  // Reset on start byte
  if (b == 0xFF) {
    frameIndex = 0;
  }
  
  // Store byte
  if (frameIndex < 9) {
    dataFrame[frameIndex++] = b;
  }
  
  // Process complete frame
  if (frameIndex == 9) {
    Serial.print(F("\n[UART] Frame:"));
    for (int i = 0; i < 9; i++) {
      Serial.print(i == 0 ? "\n" : " ");
      Serial.print(F("0x"));
      if (dataFrame[i] < 0x10) Serial.print('0');
      Serial.print(dataFrame[i], HEX);
    }
    
    if (validateChecksum()) {
      parseDataFrame();
    } else {
      Serial.println(F("\n! Invalid checksum"));
    }
    frameIndex = 0;
  }
}

bool validateChecksum() {
  unsigned char sum = 0;
  for (int i = 1; i <= 7; i++) {
    sum += dataFrame[i];
  }
  unsigned char checksum = (~sum) + 1;
  return (checksum == dataFrame[8]);
}

void parseDataFrame() {
  if (dataFrame[0] != 0xFF) return;
  
  if (dataFrame[1] == 0x17) {  // TVOC identifier in initiative mode
    unsigned int ppb = (dataFrame[4] << 8) | dataFrame[5];
    unsigned int fullScale = (dataFrame[6] << 8) | dataFrame[7];
    
    Serial.print(F("\n[UART] TVOC: "));
    Serial.print(ppb);
    Serial.print(F(" ppb ("));
    Serial.print(ppb / 1000.0, 3);
    Serial.print(F(" ppm), Full Scale: "));
    Serial.print(fullScale);
    Serial.println(F(" ppb"));
  }
  else if (dataFrame[1] == 0x86) {  // Response to Q&A mode
    unsigned int ppb = (dataFrame[6] << 8) | dataFrame[7];
    Serial.print(F("\n[Q&A Mode] TVOC: "));
    Serial.print(ppb);
    Serial.println(F(" ppb"));
  }
}