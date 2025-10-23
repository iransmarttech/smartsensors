/*
   ZPHS01B Air Quality Sensor Interface for ESP32
   Reads multi-parameter air quality data and outputs via Serial Monitor
   Wiring:
     ZPHS01B Vcc -> ESP32 5V
     ZPHS01B GND -> ESP32 GND
     ZPHS01B TxD -> ESP32 RX2 (GPIO 16)
     ZPHS01B RxD -> ESP32 TX2 (GPIO 17)
   Features:
     - Auto-calibration command
     - Comprehensive air quality levels
     - Error-checked communication
*/

#include <Arduino.h>

// =============== Configuration ===============
#define SERIAL_BAUD_RATE     115200
#define SENSOR_BAUD_RATE     9600
#define RESPONSE_LENGTH      26
#define READ_INTERVAL_MS     5000
#define OUTPUT_BUFFER_SIZE   600
#define WARMUP_DELAY_MS      180000  // 3 minutes
#define RX2_PIN              7      // HardwareSerial2 RX
#define TX2_PIN              8      // HardwareSerial2 TX

// Sensor command packets
const uint8_t REQUEST_DATA_CMD[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const uint8_t CALIBRATE_CO2_CMD[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

// Air quality level indicators (renamed to avoid conflicts)
enum AirQualityLevel { AQI_LOW, AQI_MEDIUM, AQI_HIGH, AQI_ERROR };
const char* LEVEL_STRINGS[] = {"Low", "Medium", "High", "Error"};

// =============== Data Structures ===============
struct AirQualityData {
  // Particulate Matter (μg/m³)
  uint16_t pm1;
  AirQualityLevel pm1Level;
  
  uint16_t pm25;
  AirQualityLevel pm25Level;
  
  uint16_t pm10;
  AirQualityLevel pm10Level;
  
  // Gases
  uint16_t co2;       // ppm
  AirQualityLevel co2Level;
  
  uint8_t voc;        // 0-3 scale
  AirQualityLevel vocLevel;
  
  uint16_t ch2o;      // μg/m³
  AirQualityLevel ch2oLevel;
  
  double co;          // ppm
  AirQualityLevel coLevel;
  
  double o3;          // ppm
  AirQualityLevel o3Level;
  
  double no2;         // ppm
  AirQualityLevel no2Level;
  
  // Environmental
  double temperature; // °C
  uint16_t humidity;  // %RH
  AirQualityLevel humidityLevel;
};

// =============== Global Variables ===============
AirQualityData sensorData;
unsigned long lastReadTime = 0;
bool isCalibrated = false;

// =============== Function Prototypes ===============
bool validateChecksum(const uint8_t* response, size_t length);
void processSensorData(const uint8_t* response, AirQualityData* output);
void generateReport(const AirQualityData* data, char* buffer);
void calibrateCO2();
AirQualityLevel assessPM1(uint16_t value);
AirQualityLevel assessPM25(uint16_t value);
AirQualityLevel assessPM10(uint16_t value);
AirQualityLevel assessCO2(uint16_t value);
AirQualityLevel assessVOC(uint8_t value);
AirQualityLevel assessCH2O(uint16_t value);
AirQualityLevel assessCO(double value);
AirQualityLevel assessO3(double value);
AirQualityLevel assessNO2(double value);
AirQualityLevel assessHumidity(uint16_t value);

// =============== Main Functions ===============
void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial2.begin(SENSOR_BAUD_RATE, SERIAL_8N1, RX2_PIN, TX2_PIN);

  Serial.println("\n===== ZPHS01B Sensor Initialization =====");
  Serial.println("Sensor warm-up (3 minutes required)...");
  delay(WARMUP_DELAY_MS);
  
  Serial.println("Send 'c' to calibrate CO2 sensor (requires fresh air)");
  Serial.println("===== Sensor Ready =====");
}

void loop() {
  // Handle calibration command
  if (Serial.available() && Serial.read() == 'c') {
    calibrateCO2();
  }

  // Periodic sensor reading
  if (millis() - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = millis();
    
    // Request data
    Serial2.write(REQUEST_DATA_CMD, sizeof(REQUEST_DATA_CMD));
    delay(200);  // Allow sensor processing time

    uint8_t rawData[RESPONSE_LENGTH] = {0};
    size_t bytesRead = Serial2.readBytes(rawData, RESPONSE_LENGTH);

    if (bytesRead == RESPONSE_LENGTH) {
      if (validateChecksum(rawData, bytesRead)) {
        processSensorData(rawData, &sensorData);
        char report[OUTPUT_BUFFER_SIZE];
        generateReport(&sensorData, report);
        Serial.println(report);
      } else {
        Serial.println("Error: Invalid checksum");
      }
    } else {
      Serial.printf("Error: Expected %d bytes, received %d\n", RESPONSE_LENGTH, bytesRead);
      while (Serial2.available()) Serial2.read();  // Clear buffer
    }
  }
}

// =============== Core Functions ===============
void calibrateCO2() {
  Serial.println("Initiating CO2 calibration...");
  Serial2.write(CALIBRATE_CO2_CMD, sizeof(CALIBRATE_CO2_CMD));
  delay(500);
  Serial.println("Calibration command sent. Keep sensor in fresh air for 20 minutes.");
  isCalibrated = true;
}

bool validateChecksum(const uint8_t* response, size_t length) {
  if (length != RESPONSE_LENGTH) return false;

  uint16_t calculatedSum = 0;
  for (uint8_t i = 1; i < 25; i++) {
    calculatedSum += response[i];
  }
  uint8_t checksum = (~static_cast<uint8_t>(calculatedSum)) + 1;
  
  return (checksum == response[25]);
}

void processSensorData(const uint8_t* data, AirQualityData* output) {
  // Process particulate matter
  output->pm1 = (data[2] << 8) | data[3];
  output->pm1Level = assessPM1(output->pm1);
  
  output->pm25 = (data[4] << 8) | data[5];
  output->pm25Level = assessPM25(output->pm25);
  
  output->pm10 = (data[6] << 8) | data[7];
  output->pm10Level = assessPM10(output->pm10);

  // Process gas concentrations
  output->co2 = (data[8] << 8) | data[9];
  output->co2Level = assessCO2(output->co2);
  
  output->voc = data[10];
  output->vocLevel = assessVOC(output->voc);
  
  // Process environmental data
  output->temperature = ((data[11] << 8 | data[12]) - 500) * 0.1;
  output->humidity = (data[13] << 8) | data[14];
  output->humidityLevel = assessHumidity(output->humidity);
  
  // Process additional gases
  output->ch2o = (data[15] << 8) | data[16];
  output->ch2oLevel = assessCH2O(output->ch2o);
  
  output->co = (data[17] << 8 | data[18]) * 0.1;
  output->coLevel = assessCO(output->co);
  
  output->o3 = (data[19] << 8 | data[20]) * 0.01;
  output->o3Level = assessO3(output->o3);
  
  output->no2 = (data[21] << 8 | data[22]) * 0.01;
  output->no2Level = assessNO2(output->no2);
}

void generateReport(const AirQualityData* data, char* buffer) {
  char tempStr[8], coStr[8], o3Str[8], no2Str[8];
  
  dtostrf(data->temperature, 5, 1, tempStr);
  dtostrf(data->co, 5, 1, coStr);
  dtostrf(data->o3, 5, 2, o3Str);
  dtostrf(data->no2, 5, 2, no2Str);

  snprintf(buffer, OUTPUT_BUFFER_SIZE,
    "----- AIR QUALITY REPORT -----\n"
    "PM1.0: %4d μg/m³ [%s]\n"
    "PM2.5: %4d μg/m³ [%s]\n"
    "PM10:  %4d μg/m³ [%s]\n"
    "CO₂:   %4d ppm   [%s]\n"
    "TVOC:  %4d       [%s]\n"
    "CH₂O:  %4d μg/m³ [%s]\n"
    "CO:    %5s ppm   [%s]\n"
    "O₃:    %5s ppm   [%s]\n"
    "NO₂:   %5s ppm   [%s]\n"
    "Temp:  %5s °C\n"
    "Humid: %3d%%     [%s]\n"
    "-----------------------------",
    data->pm1, LEVEL_STRINGS[data->pm1Level],
    data->pm25, LEVEL_STRINGS[data->pm25Level],
    data->pm10, LEVEL_STRINGS[data->pm10Level],
    data->co2, LEVEL_STRINGS[data->co2Level],
    data->voc, LEVEL_STRINGS[data->vocLevel],
    data->ch2o, LEVEL_STRINGS[data->ch2oLevel],
    coStr, LEVEL_STRINGS[data->coLevel],
    o3Str, LEVEL_STRINGS[data->o3Level],
    no2Str, LEVEL_STRINGS[data->no2Level],
    tempStr,
    data->humidity, LEVEL_STRINGS[data->humidityLevel]
  );
}

// =============== Quality Assessment Functions ===============
AirQualityLevel assessPM1(uint16_t value) {
  return (value <= 10) ? AQI_LOW : (value <= 25) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessPM25(uint16_t value) {
  return (value < 14) ? AQI_LOW : (value < 25) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessPM10(uint16_t value) {
  return (value < 20) ? AQI_LOW : (value < 50) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessCO2(uint16_t value) {
  return (value <= 700) ? AQI_LOW : (value <= 1200) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessVOC(uint8_t value) {
  return (value == 0) ? AQI_LOW : (value == 1) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessCH2O(uint16_t value) {
  return (value < 10) ? AQI_LOW : (value < 36) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessCO(double value) {
  return (value < 9) ? AQI_LOW : (value < 25) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessO3(double value) {
  return (value < 0.021) ? AQI_LOW : (value < 0.055) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessNO2(double value) {
  return (value < 0.051) ? AQI_LOW : (value < 0.100) ? AQI_MEDIUM : AQI_HIGH;
}

AirQualityLevel assessHumidity(uint16_t value) {
  return (value < 30) ? AQI_LOW : (value <= 68) ? AQI_MEDIUM : AQI_HIGH;
}