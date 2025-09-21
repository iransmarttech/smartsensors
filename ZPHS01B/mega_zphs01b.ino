/*
   ZPHS01B Sensor Reader for Arduino Mega
   Reads air quality data and outputs to Serial Monitor
   Wiring:
     ZPHS01B Vc  -> Arduino 5V
     ZPHS01B GND -> Arduino GND
     ZPHS01B RxD -> Arduino TX1 (Pin 18)
     ZPHS01B TxD -> Arduino RX1 (Pin 19)
*/

#include <Arduino.h>

// Constants
#define RESPONSE_LENGTH     26
#define READ_DATA_PAUSE_MS  5000
#define RESULT_MESSAGE_SIZE 600  // Increased buffer size
#define CALIBRATION_DELAY   18  // 3 minutes warm-up

// Sensor commands
const uint8_t ZPHS01B_DATA_REQUEST[9] = {0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
const uint8_t CALIBRATE_CO2_CMD[9] = {0xFF, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78};

// Air quality levels
typedef enum { LO = 0, ME = 1, HI = 2, ER = 3 } lvl_e;
const char *lvls[] = {"Low", "Med.", "High", "Error"};

// Sensor data structure
struct AirData {
  uint16_t pm1_0;     // μg/m³
  lvl_e pm1_0_lvl;
  uint16_t pm2_5;     // μg/m³
  lvl_e pm2_5_lvl;
  uint16_t pm10;      // μg/m³
  lvl_e pm10_lvl;
  uint16_t co2;       // ppm
  lvl_e co2_lvl;
  uint8_t voc;        // 0-3 grade
  lvl_e voc_lvl;
  uint16_t ch2o;      // μg/m³
  lvl_e ch2o_lvl;
  double co;          // ppm
  lvl_e co_lvl;
  double o3;          // ppm
  lvl_e o3_lvl;
  double no2;         // ppm
  lvl_e no2_lvl;
  double temp;        // °C
  uint16_t humidity;  // %RH
  lvl_e humidity_lvl;
};

// Function prototypes
uint8_t validate_response(uint8_t *response, int response_length);
void process_response(const uint8_t *response, struct AirData *output);
int generate_output(const struct AirData *d, char *output_message);
void calibrateCO2();
lvl_e get_pm1_0_lvl(uint16_t value);
lvl_e get_pm2_5_lvl(uint16_t value);
lvl_e get_pm10_lvl(uint16_t value);
lvl_e get_co2_lvl(uint16_t value);
lvl_e get_voc_lvl(uint8_t value);
lvl_e get_ch2o_lvl(uint16_t value);
lvl_e get_co_lvl(double value);
lvl_e get_o3_lvl(double value);
lvl_e get_no2_lvl(double value);
lvl_e get_humidity_lvl(uint16_t value);

// Global variables
struct AirData air_data;
unsigned long lastReadTime = 0;
bool calibrated = false;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);  // RX1=19, TX1=18
  
  while (!Serial);
  
  Serial.println(F("ZPHS01B Initializing..."));
  Serial.println(F("Warming up (3 minutes required)..."));
  delay(CALIBRATION_DELAY);
  
  Serial.println(F("Send 'c' to calibrate CO2 in fresh air (400ppm)"));
  Serial.println(F("Sensor Ready"));
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'c') calibrateCO2();
  }

  if (millis() - lastReadTime >= READ_DATA_PAUSE_MS) {
    lastReadTime = millis();
    
    Serial1.write(ZPHS01B_DATA_REQUEST, 9);
    delay(200);
    
    uint8_t data[RESPONSE_LENGTH] = {0};
    int bytesRead = Serial1.readBytes(data, RESPONSE_LENGTH);
    
    if (bytesRead == RESPONSE_LENGTH) {
      if (!validate_response(data, bytesRead)) {
        process_response(data, &air_data);
        
        char output[RESULT_MESSAGE_SIZE];
        if (generate_output(&air_data, output) > 0) {
          Serial.println(output);
        }
      } else {
        Serial.println(F("Error: Response checksum failed"));
      }
    } else {
      Serial.print(F("Error: Incomplete data ("));
      Serial.print(bytesRead);
      Serial.println(F(" bytes)"));
      while (Serial1.available()) Serial1.read();
    }
  }
}

void calibrateCO2() {
  Serial.println(F("Starting CO2 calibration..."));
  Serial1.write(CALIBRATE_CO2_CMD, 9);
  delay(500);
  Serial.println(F("Calibration command sent. Sensor needs 20 minutes in fresh air."));
  calibrated = true;
}

uint8_t validate_response(uint8_t *response, int response_length) {
  if (response_length != RESPONSE_LENGTH) return 1;

  uint16_t sum = 0;
  for (int i = 1; i < 25; i++) {
    sum += response[i];
  }
  uint8_t checksum = (~sum) + 1;

  return (checksum != response[25]) ? 1 : 0;
}

void process_response(const uint8_t *response, struct AirData *output) {
  // PM values (μg/m³)
  output->pm1_0 = (response[2] << 8) | response[3];
  output->pm1_0_lvl = get_pm1_0_lvl(output->pm1_0);

  output->pm2_5 = (response[4] << 8) | response[5];
  output->pm2_5_lvl = get_pm2_5_lvl(output->pm2_5);

  output->pm10 = (response[6] << 8) | response[7];
  output->pm10_lvl = get_pm10_lvl(output->pm10);

  // CO₂ (ppm)
  output->co2 = (response[8] << 8) | response[9];
  output->co2_lvl = get_co2_lvl(output->co2);

  // VOC (0-3 grade)
  output->voc = response[10];
  output->voc_lvl = get_voc_lvl(output->voc);

  // Temperature (°C) and Humidity (%RH)
  output->temp = (((response[11] << 8) | response[12]) - 437) * 0.1;
  output->humidity = (response[13] << 8) | response[14];
  output->humidity_lvl = get_humidity_lvl(output->humidity);

  // CH₂O (μg/m³)
  output->ch2o = (response[15] << 8) | response[16];
  output->ch2o_lvl = get_ch2o_lvl(output->ch2o);

  // CO (ppm)
  output->co = ((response[17] << 8) | response[18]) * 0.1;
  output->co_lvl = get_co_lvl(output->co);

  // O₃ (ppm)
  output->o3 = ((response[19] << 8) | response[20]) * 0.01;
  output->o3_lvl = get_o3_lvl(output->o3);

  // NO₂ (ppm)
  output->no2 = ((response[21] << 8) | response[22]) * 0.01;
  output->no2_lvl = get_no2_lvl(output->no2);
}

int generate_output(const struct AirData *d, char *output) {
  char temp_str[10], co_str[10], o3_str[10], no2_str[10];
  
  // Convert floats to strings
  dtostrf(d->temp, 4, 1, temp_str);
  dtostrf(d->co, 4, 1, co_str);
  dtostrf(d->o3, 4, 2, o3_str);
  dtostrf(d->no2, 4, 2, no2_str);

  return snprintf_P(output, RESULT_MESSAGE_SIZE, PSTR(
    "----- Air Quality Report -----\n"
    "PM1.0: %d μg/m³ (%s)\n"
    "PM2.5: %d μg/m³ (%s)\n"
    "PM10: %d μg/m³ (%s)\n"
    "CO₂: %d ppm (%s)\n"
    "TVOC: %d level (%s)\n"
    "CH₂O: %d μg/m³ (%s)\n"
    "CO: %s ppm (%s)\n"
    "O₃: %s ppm (%s)\n"
    "NO₂: %s ppm (%s)\n"
    "Temp: %s°C\n"
    "Humidity: %d%% (%s)\n"
    "-----------------------------"),
    d->pm1_0, lvls[d->pm1_0_lvl],
    d->pm2_5, lvls[d->pm2_5_lvl],
    d->pm10, lvls[d->pm10_lvl],
    d->co2, lvls[d->co2_lvl],
    d->voc, lvls[d->voc_lvl],
    d->ch2o, lvls[d->ch2o_lvl],
    co_str, lvls[d->co_lvl],
    o3_str, lvls[d->o3_lvl],
    no2_str, lvls[d->no2_lvl],
    temp_str,
    d->humidity, lvls[d->humidity_lvl]
  );
}

// Air quality level thresholds
lvl_e get_pm1_0_lvl(uint16_t pm1_0) {
    if (pm1_0 <= 10) return LO;
    if (pm1_0 <= 25) return ME;
    return HI;
}

lvl_e get_pm2_5_lvl(uint16_t pm2_5) {
    if (pm2_5 < 14) return LO;
    if (pm2_5 < 25) return ME;
    return HI;
}

lvl_e get_pm10_lvl(uint16_t pm10) {
    if (pm10 < 20) return LO;
    if (pm10 < 50) return ME;
    return HI;
}

lvl_e get_co2_lvl(uint16_t co2) {
    if (co2 <= 700) return LO;
    if (co2 <= 1200) return ME;
    return HI;
}

lvl_e get_voc_lvl(uint8_t voc) {
    if (voc == 0) return LO;
    if (voc == 1) return ME;
    return HI;
}

lvl_e get_ch2o_lvl(uint16_t ch2o) {
    if (ch2o < 10) return LO;
    if (ch2o < 36) return ME;
    return HI;
}

lvl_e get_co_lvl(double co) {
    if (co < 9) return LO;
    if (co < 25) return ME;
    return HI;
}

lvl_e get_o3_lvl(double o3) {
    if (o3 < 0.021) return LO;  // 21 ppb
    if (o3 < 0.055) return ME;  // 55 ppb
    return HI;
}

lvl_e get_no2_lvl(double no2) {
    if (no2 < 0.051) return LO;  // 51 ppb
    if (no2 < 0.100) return ME;  // 100 ppb
    return HI;
}

lvl_e get_humidity_lvl(uint16_t rh) {
    if (rh < 30) return LO;
    if (rh <= 68) return ME;
    return HI;
}