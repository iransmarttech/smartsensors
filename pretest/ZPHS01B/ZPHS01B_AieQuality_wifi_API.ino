/*
   ZPHS01B Air Quality Sensor with WiFi API
   Reads sensor data and sends to server via HTTP POST
   Wiring:
    ZPHS01B -> ESP32
      Vcc ->  5V
      GND ->  GND
      TxD ->  RX2 (GPIO 16)
      RxD ->  TX2 (GPIO 17)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

//  WiFi Configuration 
const char* WIFI_SSID = "Morteza";
const char* WIFI_PASSWORD = "miopmiow";
const char* SERVER_URL = "http://your-server.com/api/airquality";

//  Sensor Configuration 
#define SENSOR_BAUD_RATE     9600
#define RESPONSE_LENGTH      26
#define READ_INTERVAL_MS     30000  // 30 seconds
#define WARMUP_DELAY_MS      180000 // 3 minutes
#define RX2_PIN              7
#define TX2_PIN              8

// Sensor commands
const uint8_t REQUEST_DATA_CMD[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const uint8_t CALIBRATE_CO2_CMD[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

// Air quality level indicators
enum AirQualityLevel { LOW, MEDIUM, HIGH, ERROR };
const char* LEVEL_STRINGS[] = {"Low", "Medium", "High", "Error"};

//  Data Structures 
struct AirQualityData {
  // Particulate Matter
  uint16_t pm1;
  String pm1Level;
  
  uint16_t pm25;
  String pm25Level;
  
  uint16_t pm10;
  String pm10Level;
  
  // Gases
  uint16_t co2;
  String co2Level;
  
  uint8_t voc;
  String vocLevel;
  
  uint16_t ch2o;
  String ch2oLevel;
  
  double co;
  String coLevel;
  
  double o3;
  String o3Level;
  
  double no2;
  String no2Level;
  
  // Environmental
  double temperature;
  uint16_t humidity;
  String humidityLevel;
};

//  Global Variables 
AirQualityData sensorData;
unsigned long lastReadTime = 0;
bool isCalibrated = false;

//  Function Prototypes 
bool connectToWiFi();
bool validateChecksum(const uint8_t* response, size_t length);
void processSensorData(const uint8_t* response, AirQualityData* output);
void calibrateCO2();
String assessPM1(uint16_t value);
String assessPM25(uint16_t value);
String assessPM10(uint16_t value);
String assessCO2(uint16_t value);
String assessVOC(uint8_t value);
String assessCH2O(uint16_t value);
String assessCO(double value);
String assessO3(double value);
String assessNO2(double value);
String assessHumidity(uint16_t value);
bool sendDataToServer(const AirQualityData* data);

//  Main Functions 
void setup() {
  Serial.begin(115200);
  Serial2.begin(SENSOR_BAUD_RATE, SERIAL_8N1, RX2_PIN, TX2_PIN);

  // Connect to WiFi
  if (!connectToWiFi()) {
    Serial.println("Failed to connect to WiFi. Restarting...");
    ESP.restart();
  }

  Serial.println("\n===== ZPHS01B Sensor Initialization =====");
  Serial.println("Sensor warm-up (3 minutes required)...");
  delay(WARMUP_DELAY_MS);
  
  Serial.println("Send 'c' to calibrate CO2 sensor");
  Serial.println("===== Sensor Ready =====");
}

void loop() {
  // Handle calibration command
  if (Serial.available() && Serial.read() == 'c') {
    calibrateCO2();
  }

  // Periodic sensor reading and data transmission
  if (millis() - lastReadTime >= READ_INTERVAL_MS) {
    lastReadTime = millis();
    
    // Request data
    Serial2.write(REQUEST_DATA_CMD, sizeof(REQUEST_DATA_CMD));
    delay(200);  // Allow sensor processing time

    uint8_t rawData[RESPONSE_LENGTH] = {0};
    size_t bytesRead = Serial2.readBytes(rawData, RESPONSE_LENGTH);

    if (bytesRead == RESPONSE_LENGTH && validateChecksum(rawData, bytesRead)) {
      processSensorData(rawData, &sensorData);
      
      // Send data to server
      if (sendDataToServer(&sensorData)) {
        Serial.println("Data sent to server successfully");
      } else {
        Serial.println("Failed to send data to server");
      }
    } else {
      Serial.println("Error reading sensor data");
      while (Serial2.available()) Serial2.read();  // Clear buffer
    }
  }
}

//  Core Functions 
bool connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi");
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());
    return true;
  }
  return false;
}

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

bool sendDataToServer(const AirQualityData* data) {
  if (WiFi.status() != WL_CONNECTED) {
    if (!connectToWiFi()) return false;
  }

  HTTPClient http;
  http.begin(SERVER_URL);
  http.addHeader("Content-Type", "application/json");
  
  // Create JSON payload
  DynamicJsonDocument doc(1024);
  
  // Add sensor data
  doc["sensor"] = "ZPHS01B";
  doc["device_id"] = WiFi.macAddress();
  
  // Particulate Matter
  JsonObject pm = doc.createNestedObject("pm");
  pm["pm1_0"] = data->pm1;
  pm["pm1_0_level"] = data->pm1Level;
  pm["pm2_5"] = data->pm25;
  pm["pm2_5_level"] = data->pm25Level;
  pm["pm10"] = data->pm10;
  pm["pm10_level"] = data->pm10Level;
  
  // Gases
  JsonObject gases = doc.createNestedObject("gases");
  gases["co2"] = data->co2;
  gases["co2_level"] = data->co2Level;
  gases["tvoc"] = data->voc;
  gases["tvoc_level"] = data->vocLevel;
  gases["ch2o"] = data->ch2o;
  gases["ch2o_level"] = data->ch2oLevel;
  gases["co"] = data->co;
  gases["co_level"] = data->coLevel;
  gases["o3"] = data->o3;
  gases["o3_level"] = data->o3Level;
  gases["no2"] = data->no2;
  gases["no2_level"] = data->no2Level;
  
  // Environmental
  JsonObject env = doc.createNestedObject("environment");
  env["temperature"] = data->temperature;
  env["humidity"] = data->humidity;
  env["humidity_level"] = data->humidityLevel;
  
  // System info
  doc["calibrated"] = isCalibrated;
  doc["rssi"] = WiFi.RSSI();
  
  // Serialize JSON to string
  String jsonPayload;
  serializeJson(doc, jsonPayload);
  
  // Send HTTP POST
  int httpCode = http.POST(jsonPayload);
  http.end();
  
  return (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED);
}

//  Quality Assessment Functions 
String assessPM1(uint16_t value) {
  return (value <= 10) ? LEVEL_STRINGS[LOW] : 
         (value <= 25) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessPM25(uint16_t value) {
  return (value < 14) ? LEVEL_STRINGS[LOW] : 
         (value < 25) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessPM10(uint16_t value) {
  return (value < 20) ? LEVEL_STRINGS[LOW] : 
         (value < 50) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessCO2(uint16_t value) {
  return (value <= 700) ? LEVEL_STRINGS[LOW] : 
         (value <= 1200) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessVOC(uint8_t value) {
  return (value == 0) ? LEVEL_STRINGS[LOW] : 
         (value == 1) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessCH2O(uint16_t value) {
  return (value < 10) ? LEVEL_STRINGS[LOW] : 
         (value < 36) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessCO(double value) {
  return (value < 9) ? LEVEL_STRINGS[LOW] : 
         (value < 25) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessO3(double value) {
  return (value < 0.021) ? LEVEL_STRINGS[LOW] : 
         (value < 0.055) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessNO2(double value) {
  return (value < 0.051) ? LEVEL_STRINGS[LOW] : 
         (value < 0.100) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}

String assessHumidity(uint16_t value) {
  return (value < 30) ? LEVEL_STRINGS[LOW] : 
         (value <= 68) ? LEVEL_STRINGS[MEDIUM] : 
         LEVEL_STRINGS[HIGH];
}
