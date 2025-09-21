
#include <SoftwareSerial.h>

// Pins Configuration
const int sensorRX = 2;        // Arduino RX → Sensor TX (PIN6)
const int sensorTX = 3;        // Arduino TX → Voltage Divider → Sensor RX (PIN5)
const int dacPin = A0;         // Arduino A0 → Sensor DAC (PIN2)
SoftwareSerial sensorSerial(sensorRX, sensorTX);

// Frame buffer
unsigned char dataFrame[9];
byte frameIndex = 0;
unsigned long lastByteTime = 0;
const unsigned int FRAME_TIMEOUT = 150;  // ms

// Preheating requirements
unsigned long powerOnTime = 0;
const unsigned long INITIAL_PREHEAT = 864;  // 24 hours (manual requirement)86400000
const unsigned long DAILY_PREHEAT = 180000;      // 3 minutes

// DAC calibration
const float DAC_ZERO_VOLTAGE = 0.4;    // 0 ppm = 0.4V
const float DAC_FULLSCALE_VOLTAGE = 2.0; // 5 ppm = 2.0V
const float DAC_PPM_RANGE = 5.0;        // 0-5 ppm range

void setup() {
  Serial.begin(115200);
  sensorSerial.begin(9600);
  analogReference(DEFAULT);  // 5V reference for Arduino
  
  Serial.println(F("\nZE40-TVOC Sensor with UART & DAC"));
  Serial.println(F("================================"));
  Serial.println(F("Pin Connections:"));
  Serial.println(F("UART:"));
  Serial.println(F("  Sensor TX(PIN6) -> Arduino D2"));
  Serial.println(F("  Sensor RX(PIN5) -> Voltage Divider -> Arduino D3"));
  Serial.println(F("DAC:"));
  Serial.println(F("  Sensor DAC(PIN2) -> Arduino A0"));
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
  // Read analog voltage (10-bit ADC)
  int adcValue = analogRead(dacPin);
  
  // Convert to voltage (assuming 5V Arduino)
  float voltage = adcValue * (5.0 / 1023.0);
  
  // Convert to ppm (0.4-2V → 0-5ppm)
  if (voltage < DAC_ZERO_VOLTAGE) return 0.0;
  return (voltage - DAC_ZERO_VOLTAGE) * (DAC_PPM_RANGE / (DAC_FULLSCALE_VOLTAGE - DAC_ZERO_VOLTAGE));
}

void loop() {
  // Handle preheating requirements
  unsigned long elapsed = millis() - powerOnTime;
  if (elapsed < INITIAL_PREHEAT) {
    if (elapsed % 60000 == 0) {  // Print every minute
      Serial.print(F("Preheating: "));
      Serial.print(elapsed / 3600000.0, 1);
      Serial.println(F(" hours elapsed (24-48h required)"));
    }
    return;
  }
  else if (elapsed - INITIAL_PREHEAT < DAILY_PREHEAT) {
    if (elapsed % 10000 == 0) {
      Serial.print(F("Daily preheating: "));
      Serial.print((DAILY_PREHEAT - (elapsed - INITIAL_PREHEAT)) / 1000);
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
    Serial.print(analogRead(dacPin) * (5.0 / 1023.0), 2);
    Serial.print(F("V → TVOC: "));
    Serial.print(dacPPM, 3);
    Serial.println(F(" ppm"));
    
    lastDACRead = millis();
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
  
  if (dataFrame[1] == 0x17) {  // TVOC identifier
    unsigned int ppb = (dataFrame[4] << 8) | dataFrame[5];
    
    Serial.print(F("\n[UART] TVOC: "));
    Serial.print(ppb);
    Serial.print(F(" ppb ("));
    Serial.print(ppb / 1000.0, 3);
    Serial.println(F(" ppm)"));
  }
}