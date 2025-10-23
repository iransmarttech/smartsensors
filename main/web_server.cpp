#include "web_server.h"
#include "config.h"
#include <Arduino.h>

#ifdef WEB_SERVER_ENABLED

#ifdef ETHERNET_ENABLED
EthernetServer ethServer(80);
#endif

#ifdef WIFI_FALLBACK_ENABLED
WiFiServer wifiServer(80);
#endif

WebServer webServer;

const char MAIN_PAGE[] PROGMEM = R"=====(
HTTP/1.1 200 OK
Content-Type: text/html
Connection: close

<!DOCTYPE html>
<html lang="fa">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>شرکت هوشمندفناوران برتر ایرانیان</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600&display=swap" rel="stylesheet">
    <link href="https://cdn.jsdelivr.net/gh/rastikerdar/vazir-font@v30.1.0/dist/font-face.css" rel="stylesheet" type="text/css" />
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>
        :root {
            --primary: #2E7D32;
            --primary-light: #4CAF50;
            --primary-dark: #1B5E20;
            --secondary: #0277BD;
            --secondary-light: #03A9F4;
            --accent: #FF9800;
            --background: #F5F7FA;
            --card-bg: #FFFFFF;
            --text-primary: #263238;
            --text-secondary: #546E7A;
            --border: #E0E0E0;
            --success: #4CAF50;
            --warning: #FF9800;
            --danger: #F44336;
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Inter', 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            margin: 0;
            padding: 20px;
            background: var(--background);
            color: var(--text-primary);
            line-height: 1.6;
            direction: ltr;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
        }
        
        .header {
            text-align: center;
            margin-bottom: 30px;
            padding: 20px;
        }
        
        .header h1 {
            font-family: 'Vazir', 'Inter', 'Segoe UI', Tahoma, sans-serif;
            font-weight: 600;
            color: var(--primary-dark);
            margin-bottom: 10px;
            font-size: 2.2rem;
            direction: rtl;
        }
        
        .status {
            padding: 10px 20px;
            border-radius: 20px;
            background: linear-gradient(135deg, var(--primary), var(--primary-light));
            color: white;
            display: inline-block;
            margin-bottom: 10px;
            font-weight: 500;
            box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
        }
        
        .sensor-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 25px;
        }
        
        .sensor-card {
            background: var(--card-bg);
            padding: 25px;
            border-radius: 12px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
            border-top: 4px solid var(--primary);
            position: relative;
            overflow: hidden;
        }
        
        .sensor-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 8px 16px rgba(0, 0, 0, 0.12);
        }
        
        .sensor-header {
            font-size: 1.2rem;
            font-weight: 600;
            margin-bottom: 20px;
            color: var(--primary-dark);
            display: flex;
            align-items: center;
            padding-bottom: 12px;
            border-bottom: 2px solid var(--border);
        }
        
        .sensor-header i {
            margin-right: 10px;
            color: var(--primary);
            font-size: 1.4rem;
        }
        
        .data-row {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin: 12px 0;
            padding: 12px 15px;
            background: rgba(76, 175, 80, 0.05);
            border-radius: 8px;
            border-left: 3px solid var(--primary-light);
        }
        
        .data-label {
            font-weight: 500;
            color: var(--text-primary);
            display: flex;
            align-items: center;
        }
        
        .data-label i {
            margin-right: 8px;
            color: var(--secondary);
            font-size: 0.9rem;
        }
        
        .data-value {
            font-weight: 600;
            color: var(--primary-dark);
            font-size: 1.1rem;
        }
        
        .data-source {
            font-size: 0.75rem;
            color: var(--text-secondary);
            margin-left: 5px;
            font-style: italic;
        }
        
        .footer {
            text-align: center;
            margin-top: 30px;
            padding: 20px;
            color: var(--text-secondary);
            font-size: 0.9rem;
            border-top: 1px solid var(--border);
        }
        
        .footer div {
            margin: 5px 0;
        }
        
        @media (max-width: 768px) {
            .sensor-grid {
                grid-template-columns: 1fr;
            }
            
            .header h1 {
                font-size: 1.8rem;
            }
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1><i class="fas fa-microchip"></i> هوشمند فناوران برتر ایرانیان</h1>
            <div class="status" id="networkStatus">Connecting...</div>
        </div>
        
        <div class="sensor-grid">
            <!-- ZE40 TVOC Sensor -->
            <div class="sensor-card">
                <div class="sensor-header">
                    <i class="fas fa-wind"></i> ZE40 TVOC Sensor
                </div>
                
                <!-- UART Data -->
                
                
                <!-- Analog Data -->
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-bolt"></i> DAC Voltage (Analog):
                    </span>
                    <span class="data-value" id="dacVoltage">-- V</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-tachometer-alt"></i> TVOC PPM (Analog):
                    </span>
                    <span class="data-value" id="dacPPM">-- ppm</span>
                </div>
            </div>
            
            <!-- ZPHS01B Air Quality -->
            <div class="sensor-card">
                <div class="sensor-header">
                    <i class="fas fa-cloud"></i> ZPHS01B Air Quality
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-smog"></i> PM2.5:
                    </span>
                    <span class="data-value" id="pm25Value">-- μg/m³</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-smog"></i> PM10:
                    </span>
                    <span class="data-value" id="pm10Value">-- μg/m³</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-industry"></i> CO2:
                    </span>
                    <span class="data-value" id="co2Value">-- ppm</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-thermometer-half"></i> Temperature:
                    </span>
                    <span class="data-value" id="tempValue">-- °C</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-tint"></i> Humidity:
                    </span>
                    <span class="data-value" id="humidityValue">-- %</span>
                </div>
                    <div class="data-row">
                        <span class="data-label">
                            <i class="fas fa-smog"></i> PM1.0:
                        </span>
                        <span class="data-value" id="pm1Value">-- μg/m³</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">
                            <i class="fas fa-flask"></i> VOC:
                        </span>
                        <span class="data-value" id="vocValue">--</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">
                            <i class="fas fa-vial"></i> CH2O:
                        </span>
                        <span class="data-value" id="ch2oValue">--</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">
                            <i class="fas fa-burn"></i> CO:
                        </span>
                        <span class="data-value" id="coValue">--</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">
                            <i class="fas fa-biohazard"></i> O3:
                        </span>
                        <span class="data-value" id="o3Value">--</span>
                    </div>
                    <div class="data-row">
                        <span class="data-label">
                            <i class="fas fa-biohazard"></i> NO2:
                        </span>
                        <span class="data-value" id="no2Value">--</span>
                    </div>
            </div>
            
            <!-- MR007 Combustible Gas -->
            <div class="sensor-card">
                <div class="sensor-header">
                    <i class="fas fa-fire"></i> MR007 Combustible Gas
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-exclamation-triangle"></i> LEL:
                    </span>
                    <span class="data-value" id="mr007LEL">-- %</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-bolt"></i> Voltage:
                    </span>
                    <span class="data-value" id="mr007Voltage">-- V</span>
                </div>
            </div>
            
            <!-- ME4-SO2 Sulfur Dioxide -->
            <div class="sensor-card">
                <div class="sensor-header">
                    <i class="fas fa-skull-crossbones"></i> ME4-SO2 Sulfur Dioxide
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-vial"></i> SO2 Concentration:
                    </span>
                    <span class="data-value" id="so2Concentration">-- ppm</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-bolt"></i> Current:
                    </span>
                    <span class="data-value" id="so2Current">-- μA</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-bolt"></i> Voltage:
                    </span>
                    <span class="data-value" id="so2Voltage">-- V</span>
                </div>
            </div>
        </div>
        
        <div class="footer">
            <div>IP: <span id="ipAddress">--</span></div>
            <div>Last Update: <span id="lastUpdate">--</span></div>
        </div>
    </div>

    <script>
        function updateSensorData() {
            fetch('/data')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('ipAddress').textContent = data.ip_address;
                    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
                    
                    const statusElem = document.getElementById('networkStatus');
                    statusElem.textContent = 'Network: ' + data.network_mode.toUpperCase();
                    
                    // ZE40 Data - Both UART and Analog
                    // document.getElementById('tvocUARTValue').textContent = data.tvoc_ppb.toFixed(0) + ' ppb';
                    // document.getElementById('tvocUARTPPM').textContent = data.tvoc_ppm.toFixed(3) + ' ppm';
                    document.getElementById('dacVoltage').textContent = data.dac_voltage.toFixed(2) + ' V';
                    document.getElementById('dacPPM').textContent = data.dac_ppm.toFixed(3) + ' ppm';
                    
                    // ZPHS01B Data
                    if (data.air_quality) {
                        document.getElementById('pm25Value').textContent = data.air_quality.pm25 + ' μg/m³';
                        document.getElementById('pm10Value').textContent = data.air_quality.pm10 + ' μg/m³';
                        document.getElementById('co2Value').textContent = data.air_quality.co2 + ' ppm';
                        document.getElementById('tempValue').textContent = data.air_quality.temperature.toFixed(1) + ' °C';
                        document.getElementById('humidityValue').textContent = data.air_quality.humidity + ' %';
                        document.getElementById('pm1Value').textContent = data.air_quality.pm1 + ' μg/m³';
                        document.getElementById('vocValue').textContent = data.air_quality.voc;
                        document.getElementById('ch2oValue').textContent = data.air_quality.ch2o;
                        document.getElementById('coValue').textContent = data.air_quality.co.toFixed(2);
                        document.getElementById('o3Value').textContent = data.air_quality.o3.toFixed(3);
                        document.getElementById('no2Value').textContent = data.air_quality.no2.toFixed(3);
                    }
                    
                    // MR007 Data
                    if (data.mr007) {
                        document.getElementById('mr007LEL').textContent = data.mr007.lel_concentration.toFixed(1) + ' %';
                        document.getElementById('mr007Voltage').textContent = data.mr007.voltage.toFixed(3) + ' V';
                    }
                    
                    // ME4-SO2 Data
                    if (data.me4_so2) {
                        document.getElementById('so2Concentration').textContent = data.me4_so2.so2_concentration.toFixed(2) + ' ppm';
                        document.getElementById('so2Current').textContent = data.me4_so2.current_ua.toFixed(2) + ' μA';
                        document.getElementById('so2Voltage').textContent = data.me4_so2.voltage.toFixed(4) + ' V';
                    }
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                });
        }

        updateSensorData();
        setInterval(updateSensorData, 2000);
    </script>
</body>
</html>
)=====";

void WebServer::init() {
    initConnections();

    #ifdef ETHERNET_ENABLED
    ethServer.begin();
    DEBUG_PRINTLN("Ethernet server started");
    #endif

    #ifdef WIFI_FALLBACK_ENABLED
    wifiServer.begin();
    DEBUG_PRINTLN("WiFi server started");
    #endif
}

void WebServer::initConnections() {
    for (int i = 0; i < MAX_CONCURRENT_CONNECTIONS; i++) {
        connections[i].client = nullptr;
        connections[i].inUse = false;
        connections[i].lastActivity = millis();
    }
}

void WebServer::cleanupConnections() {
    unsigned long currentTime = millis();
    for (int i = 0; i < MAX_CONCURRENT_CONNECTIONS; i++) {
        if (connections[i].inUse && 
            (currentTime - connections[i].lastActivity > CONNECTION_TIMEOUT_MS)) {
            if (connections[i].client && connections[i].client->connected()) {
                connections[i].client->stop();
            }
            connections[i].inUse = false;
            connections[i].client = nullptr;
        }
    }
}

int WebServer::getFreeConnectionSlot() {
    for (int i = 0; i < MAX_CONCURRENT_CONNECTIONS; i++) {
        if (!connections[i].inUse) {
            connections[i].inUse = true;
            connections[i].lastActivity = millis();
            return i;
        }
    }
    return -1;
}

void WebServer::handleEthernetClient() {
    #ifdef ETHERNET_ENABLED
    cleanupConnections();

    EthernetClient client = ethServer.available();
    if (client) {
        int slot = getFreeConnectionSlot();
        if (slot >= 0) {
            connections[slot].client = &client;
            connections[slot].lastActivity = millis();

            handleHTTPRequest(client);

            connections[slot].inUse = false;
            connections[slot].client = nullptr;
        } else {
            client.println("HTTP/1.1 503 Service Unavailable");
            client.println("Retry-After: 5");
            client.println("Connection: close");
            client.println();
            client.println("Server busy. Please try again later.");
            client.stop();
        }
    }
    #endif
}

void WebServer::handleWiFiClient() {
    #ifdef WIFI_FALLBACK_ENABLED
    cleanupConnections();

    WiFiClient client = wifiServer.available();
    if (client) {
        int slot = getFreeConnectionSlot();
        if (slot >= 0) {
            connections[slot].client = &client;
            connections[slot].lastActivity = millis();

            handleHTTPRequest(client);

            connections[slot].inUse = false;
            connections[slot].client = nullptr;
        } else {
            client.println("HTTP/1.1 503 Service Unavailable");
            client.println("Retry-After: 5");
            client.println("Connection: close");
            client.println();
            client.println("Server busy. Please try again later.");
            client.stop();
        }
    }
    #endif
}

void WebServer::handleHTTPRequest(Client &client) {
    String request = client.readStringUntil('\r');

    if (request.indexOf("GET / ") != -1 || request.indexOf("GET /index") != -1) {
        sendMainPage(client);
    }
    else if (request.indexOf("GET /data") != -1) {
        sendJSONData(client);
    }
    else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("404 Not Found");
    }
}

void WebServer::sendMainPage(Client &client) {
    client.print(FPSTR(MAIN_PAGE));
}

void WebServer::sendJSONData(Client &client) {
    SensorData ze40Data = ze40Sensor.getSensorData();
    AirQualityData airQualityData = zphs01bSensor.getSensorData();
    MR007Data mr007Data = mr007Sensor.getSensorData();
    ME4SO2Data me4so2Data = me4so2Sensor.getSensorData();
    
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Access-Control-Allow-Origin: *");
    client.println("Connection: close");
    client.println();
    
    client.print("{");
    
    // ZE40 Data - Both UART and Analog
    client.print("\"tvoc_ppb\":");
    client.print(ze40Data.tvoc_ppb);
    client.print(",\"tvoc_ppm\":");
    client.print(ze40Data.tvoc_ppm, 3);
    client.print(",\"dac_voltage\":");
    client.print(ze40Data.dac_voltage, 2);
    client.print(",\"dac_ppm\":");
    client.print(ze40Data.dac_ppm, 3);
    client.print(",\"uart_data_valid\":");
    client.print(ze40Data.uart_data_valid ? "true" : "false");
    client.print(",\"analog_data_valid\":");
    client.print(ze40Data.analog_data_valid ? "true" : "false");
    
    // ZPHS01B Data
    if (zphs01bSensor.isDataValid()) {
        client.print(",\"air_quality\":{");
        client.print("\"pm1\":");
        client.print(airQualityData.pm1);
        client.print(",\"pm25\":");
        client.print(airQualityData.pm25);
        client.print(",\"pm10\":");
        client.print(airQualityData.pm10);
        client.print(",\"co2\":");
        client.print(airQualityData.co2);
        client.print(",\"voc\":");
        client.print(airQualityData.voc);
        client.print(",\"ch2o\":");
        client.print(airQualityData.ch2o);
        client.print(",\"co\":");
        client.print(airQualityData.co, 2);
        client.print(",\"o3\":");
        client.print(airQualityData.o3, 3);
        client.print(",\"no2\":");
        client.print(airQualityData.no2, 3);
        client.print(",\"temperature\":");
        client.print(airQualityData.temperature, 1);
        client.print(",\"humidity\":");
        client.print(airQualityData.humidity);
        client.print("}");
    } else {
        client.print(",\"air_quality\":null");
    }
    
    // MR007 Data
    if (mr007Sensor.isDataValid()) {
        client.print(",\"mr007\":{");
        client.print("\"voltage\":");
        client.print(mr007Data.voltage, 3);
        client.print(",\"rawValue\":");
        client.print(mr007Data.rawValue);
        client.print(",\"lel_concentration\":");
        client.print(mr007Data.lel_concentration, 1);
        client.print("}");
    } else {
        client.print(",\"mr007\":null");
    }
    
    // ME4-SO2 Data
    if (me4so2Sensor.isDataValid()) {
        client.print(",\"me4_so2\":{");
        client.print("\"voltage\":");
        client.print(me4so2Data.voltage, 4);
        client.print(",\"rawValue\":");
        client.print(me4so2Data.rawValue);
        client.print(",\"current_ua\":");
        client.print(me4so2Data.current_ua, 2);
        client.print(",\"so2_concentration\":");
        client.print(me4so2Data.so2_concentration, 2);
        client.print("}");
    } else {
        client.print(",\"me4_so2\":null");
    }
    
    // Network info
    client.print(",\"ip_address\":\"");
    client.print(ze40Data.ipAddress);
    client.print("\",\"network_mode\":\"");
    
    if (networkManager.isEthernetActive()) client.print("eth");
    else if (networkManager.isWifiActive()) client.print("wifi");
    else if (networkManager.isAPActive()) client.print("ap");
    else client.print("unknown");
    
    client.println("\"}");
}

#endif

// <!-- UART Data -->
//                 <div class="data-row">
//                     <span class="data-label">
//                         <i class="fas fa-signal"></i> TVOC (UART):
//                     </span>
//                     <span class="data-value" id="tvocUARTValue">-- ppb</span>
//                 </div>
//                 <div class="data-row">
//                     <span class="data-label">
//                         <i class="fas fa-chart-line"></i> TVOC PPM (UART):
//                     </span>
//                     <span class="data-value" id="tvocUARTPPM">-- ppm</span>
//                 </div>