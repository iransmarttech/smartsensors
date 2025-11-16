#include "ze40_sensor.h"
#include "config.h"
#include "shared_data.h"
#include <Arduino.h>

ZE40Sensor ze40Sensor;

// Preheating configuration
const unsigned long INITIAL_PREHEAT = 0;
const unsigned long DAILY_PREHEAT = 180000;

void ZE40Sensor::init() {
    ze40State.serial = new HardwareSerial(1);
    ze40State.serial->begin(9600, SERIAL_8N1, ZE40_RX_PIN, ZE40_TX_PIN);
    analogReadResolution(12);
    
    ze40State.powerOnTime = millis();

    // Start in initiative upload mode
    uint8_t initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
    ze40State.serial->write(initCmd, 9);

    DEBUG_PRINTLN("✓ ZE40 TVOC Sensor initialized");
}

void ZE40Sensor::processData() {
    unsigned long elapsed = millis() - ze40State.powerOnTime;
    
    // Handle preheating
    if (!ze40State.preheatComplete) {
        if (elapsed >= INITIAL_PREHEAT) {
            ze40State.preheatComplete = true;
            DEBUG_PRINTLN("✓ ZE40 preheating complete");
        } else {
            return;
        }
    }
    
    // Skip during daily preheating
    if (elapsed % 86400000 < DAILY_PREHEAT) {
        return;
    }

    // Process incoming UART data
    while (ze40State.serial->available()) {
        processByte(ze40State.serial->read());
    }
    
    // Handle frame timeout
    if (ze40State.frameIndex > 0 && (millis() - ze40State.lastByteTime > FRAME_TIMEOUT)) {
        ze40State.frameIndex = 0;
    }
}

void ZE40Sensor::processByte(uint8_t byte) {
    ze40State.lastByteTime = millis();
    
    if (byte == 0xFF) {
        ze40State.frameIndex = 0;
    }
    
    if (ze40State.frameIndex < 9) {
        ze40State.frame[ze40State.frameIndex++] = byte;
    }
    
    if (ze40State.frameIndex == 9 && validateChecksum()) {
        parseDataFrame();
        ze40State.frameIndex = 0;
    }
}

void ZE40Sensor::switchToQAMode() {
    if (!ze40State.preheatComplete) return;
    
    uint8_t qaCmd[9] = {0xFF, 0x01, 0x78, 0x41, 0x00, 0x00, 0x00, 0x00, 0x46};
    ze40State.serial->write(qaCmd, 9);
    ze40State.inQAMode = true;
}

void ZE40Sensor::switchToInitiativeMode() {
    uint8_t initCmd[9] = {0xFF, 0x01, 0x78, 0x40, 0x00, 0x00, 0x00, 0x00, 0x47};
    ze40State.serial->write(initCmd, 9);
    ze40State.inQAMode = false;
}

void ZE40Sensor::requestReading() {
    if (!ze40State.preheatComplete) return;

    switchToQAMode();
    delay(100);
    
    uint8_t requestCmd[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    ze40State.serial->write(requestCmd, 9);
    
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

bool ZE40Sensor::isPreheatComplete() {
    return ze40State.preheatComplete;
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
    
    if (ze40State.frame[1] == 0x17 || ze40State.frame[1] == 0x86) {
        uint16_t ppb = 0;
        
        if (ze40State.frame[1] == 0x17) {
            ppb = (ze40State.frame[4] << 8) | ze40State.frame[5];
        } else {
            ppb = (ze40State.frame[6] << 8) | ze40State.frame[7];
        }

        // Update shared data directly
        if (lockData()) {
            sharedData.ze40_tvoc_ppb = ppb;
            sharedData.ze40_tvoc_ppm = ppb / 1000.0;
            sharedData.ze40_uart_valid = true;
            sharedData.last_update = millis();
            unlockData();
        }
        
        DEBUG_PRINTF("ZE40 UART - TVOC: %d ppb (%.3f ppm)\n", ppb, ppb / 1000.0);
        ze40State.uartDataReceived = true;
    }
}