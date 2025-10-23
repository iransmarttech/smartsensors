#include "ze40_sensor.h"
#include "config.h"
#include <Arduino.h>

ZE40Sensor ze40Sensor;

// Preheating configuration - REDUCED FOR TESTING
const unsigned long INITIAL_PREHEAT = 0;        // 0 hours for testing (original: 86400000 = 24h)
const unsigned long DAILY_PREHEAT = 180000;     // 3 minutes in ms

void ZE40Sensor::init() {
    dataMutex = xSemaphoreCreateMutex();
    ze40State.serial = new HardwareSerial(1);
    ze40State.serial->begin(9600, SERIAL_8N1, ZE40_RX_PIN, ZE40_TX_PIN);

    analogReadResolution(12);
    
    ze40State.powerOnTime = millis();

    // Start in initiative upload mode
    uint8_t initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
    ze40State.serial->write(initCmd, 9);

    DEBUG_PRINTLN("ZE40 Sensor Initialized - UART + Analog");
    DEBUG_PRINTLN("Preheating started...");
}

void ZE40Sensor::processData() {
    // Handle preheating requirements
    unsigned long elapsed = millis() - ze40State.powerOnTime;
    
    if (!ze40State.preheatComplete) {
        if (elapsed < INITIAL_PREHEAT) {
            // Still in initial preheating
            if (elapsed % 60000 == 0) {
                DEBUG_PRINTF("ZE40 Preheating: %.1f hours elapsed\n", elapsed / 3600000.0);
            }
            return;
        } else {
            ze40State.preheatComplete = true;
            DEBUG_PRINTLN("ZE40 Initial preheating complete!");
        }
    }
    
    // Daily preheating (3 minutes)
    if (elapsed % 86400000 < DAILY_PREHEAT) {
        if (elapsed % 10000 == 0) {
            DEBUG_PRINTF("ZE40 Daily preheating: %ld seconds remaining\n", 
                        (DAILY_PREHEAT - (elapsed % 86400000)) / 1000);
        }
        return;
    }

    // Process incoming UART data
    while (ze40State.serial->available()) {
        processByte(ze40State.serial->read());
    }
    
    // Handle frame timeout
    if (ze40State.frameIndex > 0 && (millis() - ze40State.lastByteTime > FRAME_TIMEOUT)) {
        DEBUG_PRINTLN("ZE40 Frame timeout - resetting buffer");
        ze40State.frameIndex = 0;
    }
}

void ZE40Sensor::processByte(uint8_t byte) {
    ze40State.lastByteTime = millis();
    
    // Reset on start byte
    if (byte == 0xFF) {
        ze40State.frameIndex = 0;
    }
    
    // Store byte
    if (ze40State.frameIndex < 9) {
        ze40State.frame[ze40State.frameIndex++] = byte;
    }
    
    // Process complete frame
    if (ze40State.frameIndex == 9) {
        #ifdef DEBUG_SERIAL_ENABLED
        if (Serial) {
            Serial.print("ZE40 Frame received:");
            for (int i = 0; i < 9; i++) {
                Serial.print(" 0x");
                if (ze40State.frame[i] < 0x10) Serial.print('0');
                Serial.print(ze40State.frame[i], HEX);
            }
            Serial.println();
        }
        #endif
        
        if (validateChecksum()) {
            parseDataFrame();
        } else {
            DEBUG_PRINTLN("ZE40 Invalid checksum");
        }
        ze40State.frameIndex = 0;
    }
}

void ZE40Sensor::switchToQAMode() {
    if (!ze40State.preheatComplete) return;
    
    uint8_t qaCmd[9] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
    ze40State.serial->write(qaCmd, 9);
    ze40State.inQAMode = true;
    DEBUG_PRINTLN("ZE40 Switched to Q&A mode");
}

void ZE40Sensor::switchToInitiativeMode() {
    uint8_t initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
    ze40State.serial->write(initCmd, 9);
    ze40State.inQAMode = false;
    DEBUG_PRINTLN("ZE40 Switched to initiative mode");
}

void ZE40Sensor::requestReading() {
    if (!ze40State.preheatComplete) return;

    // Switch to Q&A mode and request reading
    switchToQAMode();
    delay(100);
    
    uint8_t requestCmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    ze40State.serial->write(requestCmd, 9);
    DEBUG_PRINTLN("ZE40 Reading requested in Q&A mode");
    
    delay(100);
    switchToInitiativeMode();
}

float ZE40Sensor::readDACVoltage() {
    int adcValue = analogRead(ZE40_DAC_PIN);
    return adcValue * (3.3 / 4095.0);
}

float ZE40Sensor::readDACPPM(float voltage) {
    if (voltage < DAC_ZERO_VOLTAGE) return 0.0;
    return (voltage - DAC_ZERO_VOLTAGE) * (DAC_PPM_RANGE / (DAC_FULLSCALE_VOLTAGE - DAC_ZERO_VOLTAGE));
}

SensorData ZE40Sensor::getSensorData() {
    SensorData data;
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        data = sensorData;
        xSemaphoreGive(dataMutex);
    }
    return data;
}

void ZE40Sensor::setSensorData(const SensorData& data) {
    if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE) {
        sensorData = data;
        xSemaphoreGive(dataMutex);
    }
}

bool ZE40Sensor::isPreheatComplete() {
    return ze40State.preheatComplete;
}

bool ZE40Sensor::isUARTDataValid() {
    return ze40State.uartDataReceived && (millis() - ze40State.lastByteTime < 10000);
}

bool ZE40Sensor::isAnalogDataValid() {
    return sensorData.analog_data_valid;
}

bool ZE40Sensor::validateChecksum() {
    uint8_t sum = 0;
    for (uint8_t i = 1; i <= 7; i++) {
        sum += ze40State.frame[i];
    }
    return ((~sum) + 1) == ze40State.frame[8];
}

void ZE40Sensor::parseDataFrame() {
    if (ze40State.frame[0] != 0xFF) return;
    
    if (ze40State.frame[1] == 0x17) {
        // Initiative upload mode - TVOC data
        uint16_t ppb = (ze40State.frame[4] << 8) | ze40State.frame[5];
        uint16_t fullScale = (ze40State.frame[6] << 8) | ze40State.frame[7];

        SensorData newData = getSensorData();
        newData.tvoc_ppb = ppb;
        newData.tvoc_ppm = ppb / 1000.0;
        newData.uart_data_valid = true;
        setSensorData(newData);

        #ifdef DEBUG_SERIAL_ENABLED
        if (Serial) {
            Serial.print("ZE40 UART - TVOC: ");
            Serial.print(ppb);
            Serial.print(" ppb (");
            Serial.print(ppb / 1000.0, 3);
            Serial.print(" ppm), Full Scale: ");
            Serial.print(fullScale);
            Serial.println(" ppb");
        }
        #endif
        
        ze40State.uartDataReceived = true;
    }
    else if (ze40State.frame[1] == 0x86) {
        // Q&A mode response
        uint16_t ppb = (ze40State.frame[6] << 8) | ze40State.frame[7];

        SensorData newData = getSensorData();
        newData.tvoc_ppb = ppb;
        newData.tvoc_ppm = ppb / 1000.0;
        newData.uart_data_valid = true;
        setSensorData(newData);

        DEBUG_PRINTF("ZE40 Q&A - TVOC: %d ppb\n", ppb);
        
        ze40State.uartDataReceived = true;
    }
}