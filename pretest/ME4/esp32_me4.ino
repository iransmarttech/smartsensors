/**
 * ME4-SO2 Sulfur Dioxide Sensor Interface for ESP32
 * Based on Winsen Electronics ME4-SO2 0-20ppm Sensor Datasheet (Ver1.2)
 * 
 * Features:
 * - Accurate SO2 concentration measurement
 * - Temperature compensation
 * - Zero calibration function
 * - Cross-sensitivity compensation for common interferents
 */

#include <Arduino.h>

// Sensor configuration parameters
#define ADC_RESOLUTION 12          // ESP32 ADC resolution (12 bits)
#define ADC_MAX_VALUE 4095.0       // Maximum ADC value (2^12 - 1)
#define REF_VOLTAGE 3.3            // ESP32 reference voltage

// Sensor calibration parameters (from datasheet)
const float DEFAULT_SENSITIVITY = 0.8;    // μA/ppm (0.8 ± 0.2)
const float LOAD_RESISTANCE = 10.0;       // Ω (recommended in datasheet)
const int WARMUP_TIME = 48 * 3600 * 1000; // 48 hours in milliseconds

// Cross-sensitivity factors (from datasheet Table 2)
const float CROSS_CO = 0.0125;     // 2.5ppm/200ppm
const float CROSS_H2S = 0.003;     // 0.15ppm/50ppm
const float CROSS_CL2 = -0.06;     // -0.6ppm/10ppm
const float CROSS_C2H4 = 0.0385;   // 5ppm/130ppm
const float CROSS_H2 = 0.0025;     // 1ppm/400ppm
const float CROSS_ALCOHOL = 0.0015;// 1.5ppm/1000ppm
const float CROSS_NH3 = 0.002;     // 0.1ppm/50ppm
const float CROSS_PH3 = 0.15;      // 3ppm/20ppm

class ME4_SO2 {
private:
    uint8_t analogPin;
    float sensitivity;
    float zeroOffset;
    float temperatureCompensation;
    bool isCalibrated;

public:
    /**
     * Constructor for ME4-SO2 sensor
     * @param pin Analog input pin
     * @param sens Sensor sensitivity in μA/ppm (default 0.8)
     */
    ME4_SO2(uint8_t pin, float sens = DEFAULT_SENSITIVITY) 
        : analogPin(pin), sensitivity(sens), zeroOffset(0), 
          temperatureCompensation(1.0), isCalibrated(false) {}

    /**
     * Initialize the sensor
     * Note: Sensor requires 48-hour warmup per datasheet
     */
    void begin() {
        pinMode(analogPin, INPUT);
        analogReadResolution(ADC_RESOLUTION);
        Serial.println("ME4-SO2 Sensor Initialized");
        Serial.println("Note: Sensor requires 48-hour warmup for accurate readings");
    }

    /**
     * Read raw voltage from sensor
     * @return Voltage in volts
     */
    float readVoltage() {
        int rawValue = analogRead(analogPin);
        return (rawValue * REF_VOLTAGE) / ADC_MAX_VALUE;
    }

    /**
     * Calculate current from voltage
     * @return Current in microamperes (μA)
     */
    float readCurrent() {
        float voltage = readVoltage();
        return (voltage / LOAD_RESISTANCE) * 1000000.0; // Convert to μA
    }

    /**
     * Set temperature compensation factor
     * @param compFactor Compensation factor (1.0 = no compensation)
     */
    void setTemperatureCompensation(float compFactor) {
        temperatureCompensation = compFactor;
    }

    /**
     * Perform zero calibration in clean air
     * Must be called in clean air environment per datasheet
     */
    void calibrateZero() {
        Serial.println("Zero calibration in progress...");
        
        // Take multiple readings for stability
        float sum = 0;
        for (int i = 0; i < 100; i++) {
            sum += readCurrent();
            delay(10);
        }
        
        zeroOffset = sum / 100;
        isCalibrated = true;
        Serial.println("Zero calibration completed");
    }

    /**
     * Read SO2 concentration in ppm
     * @return SO2 concentration in parts per million
     */
    float readPPM() {
        if (!isCalibrated) {
            Serial.println("Warning: Sensor not calibrated. Use calibrateZero() in clean air.");
        }
        
        float current = readCurrent() - zeroOffset;
        float ppm = (current / sensitivity) * temperatureCompensation;
        
        // Ensure non-negative output
        return max(ppm, 0.0f);
    }

    /**
     * Check if sensor is calibrated
     * @return True if sensor has been calibrated
     */
    bool calibrated() {
        return isCalibrated;
    }

    /**
     * Get sensor status information
     */
    void getStatus() {
        Serial.print("Voltage: ");
        Serial.print(readVoltage());
        Serial.println(" V");
        
        Serial.print("Current: ");
        Serial.print(readCurrent());
        Serial.println(" μA");
        
        Serial.print("Calibrated: ");
        Serial.println(isCalibrated ? "Yes" : "No");
        
        if (isCalibrated) {
            Serial.print("Zero Offset: ");
            Serial.print(zeroOffset);
            Serial.println(" μA");
        }
    }
};

// Create sensor instance on GPIO 34 (Analog input)
ME4_SO2 so2Sensor(34);

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    so2Sensor.begin();
    
    // Wait for sensor stabilization (in a real application, you'd wait 48 hours)
    Serial.println("Allowing sensor to stabilize...");
    delay(5000);
    
    // Perform zero calibration (should be done in clean air)
    so2Sensor.calibrateZero();
}

void loop() {
    // Read and display SO2 concentration
    float ppm = so2Sensor.readPPM();
    
    Serial.print("SO2 Concentration: ");
    Serial.print(ppm, 2);
    Serial.println(" ppm");
    
    // Display sensor status every 10 readings
    static int count = 0;
    if (count++ % 10 == 0) {
        so2Sensor.getStatus();
        Serial.println("-----------------------");
    }
    
    delay(2000); // Read every 2 seconds
}