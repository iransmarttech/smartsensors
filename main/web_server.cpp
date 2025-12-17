#include "web_server.h"
#include "config.h"
#include "credentials.h"
#include "shared_data.h"
#include <Arduino.h>
#include <mbedtls/base64.h>

#ifdef WEB_SERVER_ENABLED

// HTTP Headers for caching control
const char HTTP_CACHE_HEADER[] PROGMEM = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Cache-Control: public, max-age=3600\r\n"  // Cache HTML for 1 hour
    "X-Frame-Options: DENY\r\n"  // Security: prevent clickjacking
    "X-Content-Type-Options: nosniff\r\n"  // Security: prevent MIME sniffing
    "Connection: close\r\n\r\n";

const char HTTP_NO_CACHE_HEADER[] PROGMEM = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: http://localhost\r\n"  // Security: restrict CORS
    "Cache-Control: no-cache, no-store, must-revalidate\r\n"  // Never cache JSON
    "Pragma: no-cache\r\n"
    "Expires: 0\r\n"
    "X-Content-Type-Options: nosniff\r\n"  // Security header
    "Connection: close\r\n\r\n";

const char HTTP_UNAUTHORIZED[] PROGMEM = 
    "HTTP/1.1 401 Unauthorized\r\n"
    "WWW-Authenticate: Basic realm=\"Smart Sensor System\"\r\n"
    "Content-Type: text/html\r\n"
    "Cache-Control: no-cache\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html><html><head><title>401 Unauthorized</title></head>"
    "<body><h1>401 Unauthorized</h1><p>Authentication required.</p></body></html>";

const char HTTP_FORBIDDEN[] PROGMEM = 
    "HTTP/1.1 403 Forbidden\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n\r\n"
    "<!DOCTYPE html><html><head><title>403 Forbidden</title></head>"
    "<body><h1>403 Forbidden</h1><p>Rate limit exceeded.</p></body></html>";

// Static HTML content (without HTTP headers - we'll add them separately)
const char MAIN_PAGE[] PROGMEM = R"=====(<!DOCTYPE html>
<html lang="fa">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Air Quality Monitoring System</title>
    <link href="https://fonts.googleapis.com/css2?family=Inter:wght@300;400;500;600&display=swap" rel="stylesheet">
    <link href="https://cdn.jsdelivr.net/gh/rastikerdar/vazir-font@v30.1.0/dist/font-face.css" rel="stylesheet">
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>
        :root {
            --primary: #2E7D32;
            --primary-light: #4CAF50;
            --primary-dark: #1B5E20;
            --secondary: #0277BD;
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
            font-family: 'Vazir', 'Inter', sans-serif;
            font-weight: 600;
            color: var(--primary-dark);
            margin-bottom: 10px;
            font-size: 2.2rem;
        }
        
        .status {
            padding: 10px 20px;
            border-radius: 20px;
            background: linear-gradient(135deg, var(--primary), var(--primary-light));
            color: white;
            display: inline-block;
            margin-bottom: 10px;
            font-weight: 500;
        }
        
        .sensor-grid {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
            gap: 25px;
            margin-bottom: 30px;
        }
        
        .sensor-card {
            background: var(--card-bg);
            padding: 25px;
            border-radius: 12px;
            box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
            transition: transform 0.3s ease, box-shadow 0.3s ease;
            border-top: 4px solid var(--primary);
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
        }
        
        .data-value {
            font-weight: 600;
            color: var(--primary-dark);
            font-size: 1.1rem;
        }
        
        .footer {
            text-align: center;
            margin-top: 30px;
            padding: 20px;
            color: var(--text-secondary);
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
            <h1><i class="fas fa-microchip"></i> سیستم مانیتورینگ کیفیت هوا</h1>
            <div class="status" id="networkStatus">Connecting...</div>
        </div>
        
        <div class="sensor-grid">
            <!-- ZE40 TVOC Sensor -->
            <div class="sensor-card">
                <div class="sensor-header">
                    <i class="fas fa-wind"></i> ZE40 TVOC Sensor
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-bolt"></i> DAC Voltage:
                    </span>
                    <span class="data-value" id="dacVoltage">-- V</span>
                </div>
                <div class="data-row">
                    <span class="data-label">
                        <i class="fas fa-tachometer-alt"></i> TVOC PPM:
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
            </div>
        </div>
        
        <div class="footer">
            <div>IP Address: <span id="ipAddress">--</span></div>
            <div>Last Update: <span id="lastUpdate">--</span></div>
        </div>
    </div>

    <script>
        function updateSensorData() {
            fetch('/data')
                .then(response => {
                    if (!response.ok) throw new Error('Network error');
                    return response.json();
                })
                .then(data => {
                    // Update network info
                    document.getElementById('ipAddress').textContent = data.ip_address;
                    document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
                    
                    const statusElem = document.getElementById('networkStatus');
                    statusElem.textContent = 'Network: ' + data.network_mode.toUpperCase();
                    
                    // ZE40 Data
                    document.getElementById('dacVoltage').textContent = data.dac_voltage.toFixed(2) + ' V';
                    document.getElementById('dacPPM').textContent = data.dac_ppm.toFixed(3) + ' ppm';
                    
                    // ZPHS01B Data
                    if (data.air_quality) {
                        document.getElementById('pm25Value').textContent = data.air_quality.pm25 + ' μg/m³';
                        document.getElementById('pm10Value').textContent = data.air_quality.pm10 + ' μg/m³';
                        document.getElementById('co2Value').textContent = data.air_quality.co2 + ' ppm';
                        document.getElementById('tempValue').textContent = data.air_quality.temperature.toFixed(1) + ' °C';
                        document.getElementById('humidityValue').textContent = data.air_quality.humidity + ' %';
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
                    }
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                    document.getElementById('networkStatus').textContent = 'Connection Error';
                });
        }

        // Initial update and set interval
        updateSensorData();
        setInterval(updateSensorData, 2000);
    </script>
</body>
</html>
)=====";

SensorWebServer webServer;

void SensorWebServer::init() {
    DEBUG_PRINTLN("Initializing web server...");
    
    // Initialize authentication manager
    WebAuthManager::init();
    lastRateLimitCleanup = millis();
    
    #ifdef ETHERNET_ENABLED
    // Allocate on heap with placement new to control construction timing
    void* serverMem = malloc(sizeof(EthernetServer));
    if (serverMem != nullptr) {
        // Delay to ensure SPI is stable
        vTaskDelay(pdMS_TO_TICKS(500));
        
        // Use placement new - construct object at specific memory location
        ethServer = new (serverMem) EthernetServer(80);
        
        // Another delay before calling begin()
        vTaskDelay(pdMS_TO_TICKS(500));
        
        ethServer->begin();
        
        DEBUG_PRINTLN("✓ Ethernet HTTP server started on port 80");
        DEBUG_PRINTLN("✓ Web authentication enabled");
        
        // Final stabilization delay
        vTaskDelay(pdMS_TO_TICKS(500));
    } else {
        DEBUG_PRINTLN("✗ ERROR: Failed to allocate server memory");
    }
    #endif
}

void SensorWebServer::handleEthernetClient() {
    #ifdef ETHERNET_ENABLED
    if (ethServer == nullptr) {
        DEBUG_PRINTLN("ERROR: ethServer is nullptr!");
        return;
    }
    
    if (activeClients >= MAX_CONCURRENT_CONNECTIONS) {
        DEBUG_PRINTLN("Max clients reached");
        return;
    }
    
    EthernetClient client = ethServer->available();
    
    if (client) {
        DEBUG_PRINTLN("→ Client connected!");
        IPAddress ip = client.remoteIP();
        String clientIP = String(ip[0]) + "." + String(ip[1]) + "." + 
                         String(ip[2]) + "." + String(ip[3]);
        DEBUG_PRINTF("  Client IP: %s\n", clientIP.c_str());
        
        activeClients++;
        handleHTTPRequest(client, clientIP);
        client.stop();
        activeClients--;
        DEBUG_PRINTLN("← Client disconnected");
    }
    #endif
}

void SensorWebServer::handleWiFiClient() {
    #ifdef WIFI_FALLBACK_ENABLED
    if (wifiServer == nullptr) {
        return;
    }
    
    if (activeClients >= MAX_CONCURRENT_CONNECTIONS) {
        return;
    }
    
    WiFiClient client = wifiServer->available();
    
    if (client) {
        IPAddress ip = client.remoteIP();
        String clientIP = String(ip[0]) + "." + String(ip[1]) + "." + 
                         String(ip[2]) + "." + String(ip[3]);
        
        activeClients++;
        handleHTTPRequest(client, clientIP);
        client.stop();
        activeClients--;
    }
    #endif
}

void SensorWebServer::sendUnauthorized(Client &client) {
    client.print(FPSTR(HTTP_UNAUTHORIZED));
    DEBUG_PRINTLN("Sent 401 Unauthorized");
}

void SensorWebServer::sendForbidden(Client &client) {
    client.print(FPSTR(HTTP_FORBIDDEN));
    DEBUG_PRINTLN("Sent 403 Forbidden (Rate Limited)");
}

bool SensorWebServer::checkAuthentication(const String& authHeader) {
    if (authHeader.length() == 0) {
        return false;
    }

    // Check if it's Basic authentication
    if (!authHeader.startsWith("Basic ")) {
        return false;
    }

    // Extract Base64 encoded credentials
    String encodedCreds = authHeader.substring(6); // Remove "Basic "
    
    // Decode Base64 using mbedtls
    size_t outputLen;
    
    // First call to get required buffer size
    mbedtls_base64_decode(NULL, 0, &outputLen,
                          (const unsigned char*)encodedCreds.c_str(), encodedCreds.length());
    
    // Allocate buffer and decode
    unsigned char decoded[outputLen + 1];
    int ret = mbedtls_base64_decode(decoded, outputLen, &outputLen,
                                     (const unsigned char*)encodedCreds.c_str(), encodedCreds.length());
    
    if (ret != 0) {
        return false;  // Decode failed
    }
    
    decoded[outputLen] = '\0';
    String decodedCreds = String((char*)decoded);

    // Expected format: username:password
    int separatorIndex = decodedCreds.indexOf(':');
    if (separatorIndex == -1) {
        return false;
    }

    String username = decodedCreds.substring(0, separatorIndex);
    String password = decodedCreds.substring(separatorIndex + 1);

    // Constant-time comparison to prevent timing attacks
    bool usernameMatch = (username == WEB_ADMIN_USERNAME);
    bool passwordMatch = (password == WEB_ADMIN_PASSWORD);

    return (usernameMatch && passwordMatch);
}

bool SensorWebServer::checkAPIToken(const String& tokenHeader) {
    if (tokenHeader.length() == 0) {
        return false;
    }
    
    // Constant-time comparison
    return (tokenHeader == API_ACCESS_TOKEN);
}

void SensorWebServer::handleHTTPRequest(Client &client, const String& clientIP) {
    if (xPortInIsrContext()) {
        client.stop();
        return;
    }
    
    DEBUG_PRINTLN("Reading HTTP request...");
    
    unsigned long startTime = millis();
    String request = "";
    String authHeader = "";
    String apiTokenHeader = "";
    
    while (client.connected() && (millis() - startTime < 2000)) {
        if (client.available()) {
            char c = client.read();
            request += c;
            if (request.endsWith("\r\n\r\n")) break;
        }
        vTaskDelay(1);
    }

    if (request.length() == 0) {
        DEBUG_PRINTLN("Empty request");
        client.stop();
        return;
    }
    
    DEBUG_PRINTF("Request: %s\n", request.substring(0, 50).c_str());

    // Rate limiting check
    if (clientIP.length() > 0) {
        // Periodically clean old rate limit records
        if (millis() - lastRateLimitCleanup > 60000) {
            WebAuthManager::clearRateLimitRecords();
            lastRateLimitCleanup = millis();
        }
        
        if (!WebAuthManager::checkRateLimit(clientIP)) {
            DEBUG_PRINTF("Rate limit exceeded for IP: %s\n", clientIP.c_str());
            sendForbidden(client);
            client.stop();
            return;
        }
    }

    // Extract Authorization header
    int authStart = request.indexOf("Authorization: ");
    if (authStart != -1) {
        int authEnd = request.indexOf("\r\n", authStart);
        authHeader = request.substring(authStart + 15, authEnd);
    }

    // Extract X-API-Token header
    int tokenStart = request.indexOf("X-API-Token: ");
    if (tokenStart != -1) {
        int tokenEnd = request.indexOf("\r\n", tokenStart);
        apiTokenHeader = request.substring(tokenStart + 13, tokenEnd);
    }

    // Check if this is a data endpoint (requires API token or basic auth)
    bool isDataEndpoint = (request.indexOf("GET /data") != -1);
    bool isMainPage = (request.indexOf("GET / ") != -1 || request.indexOf("GET /index") != -1);
    
    // Authentication logic
    bool authenticated = false;
    
    if (isDataEndpoint) {
        // Data endpoint: Accept either API token or Basic Auth
        authenticated = checkAPIToken(apiTokenHeader) || checkAuthentication(authHeader);
    } else if (isMainPage) {
        // Main page: Require Basic Auth
        authenticated = checkAuthentication(authHeader);
    }

    // Handle endpoints
    if (isMainPage) {
        if (!authenticated) {
            DEBUG_PRINTLN("Unauthorized access to main page");
            sendUnauthorized(client);
        } else {
            DEBUG_PRINTLN("Sending main page (authenticated)");
            sendMainPage(client, true);
        }
    }
    else if (isDataEndpoint) {
        if (!authenticated) {
            DEBUG_PRINTLN("Unauthorized access to data endpoint");
            sendUnauthorized(client);
        } else {
            DEBUG_PRINTLN("Sending JSON data (authenticated)");
            sendJSONData(client, true);
        }
    }
    else {
        DEBUG_PRINTLN("404 Not Found");
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("404 Not Found");
    }
}

void SensorWebServer::sendMainPage(Client &client, bool authenticated) {
    if (!authenticated) {
        sendUnauthorized(client);
        return;
    }
    
    // Send HTTP header with cache control FIRST
    client.print(FPSTR(HTTP_CACHE_HEADER));
    
    // Then send large HTML in chunks to avoid buffer overflow
    const char* htmlPtr = MAIN_PAGE;
    size_t totalLen = strlen_P(htmlPtr);
    size_t chunkSize = 512;  // Send 512 bytes at a time
    
    DEBUG_PRINTF("Sending cached HTML page (%d bytes) in chunks...\n", totalLen);
    
    for (size_t i = 0; i < totalLen; i += chunkSize) {
        size_t remaining = totalLen - i;
        size_t bytesToSend = (remaining < chunkSize) ? remaining : chunkSize;
        
        char buffer[chunkSize + 1];
        memcpy_P(buffer, htmlPtr + i, bytesToSend);
        buffer[bytesToSend] = '\0';
        
        client.write((const uint8_t*)buffer, bytesToSend);
        client.flush();  // Ensure data is sent before continuing
        
        vTaskDelay(1);  // Small delay to allow TCP stack to process
    }
    
    DEBUG_PRINTLN("✓ HTML page sent successfully (will be cached by browser)");
}

void SensorWebServer::sendJSONData(Client &client, bool authenticated) {
    vTaskDelay(1);
    
    if (!authenticated) {
        sendUnauthorized(client);
        return;
    }
    
    if (!lockData(1000)) {
        client.print(FPSTR(HTTP_NO_CACHE_HEADER));
        client.println("{\"error\":\"Data temporarily unavailable\"}");
        return;
    }

    SharedSensorData localData = sharedData;
    unlockData();

    // Send JSON response header with NO CACHE directive
    client.print(FPSTR(HTTP_NO_CACHE_HEADER));
    
    // Build and send JSON body
    client.print("{");
    
    // ZE40 Data
    client.print("\"dac_voltage\":");
    client.print(localData.ze40_dac_voltage, 2);
    client.print(",\"dac_ppm\":");
    client.print(localData.ze40_dac_ppm, 3);
    client.print(",\"tvoc_ppb\":");
    client.print(localData.ze40_tvoc_ppb);
    client.print(",\"tvoc_ppm\":");
    client.print(localData.ze40_tvoc_ppm, 3);
    
    // ZPHS01B Data
    if (localData.zphs01b_valid) {
        client.print(",\"air_quality\":{");
        client.print("\"pm25\":");
        client.print(localData.zphs01b_pm25);
        client.print(",\"pm10\":");
        client.print(localData.zphs01b_pm10);
        client.print(",\"co2\":");
        client.print(localData.zphs01b_co2);
        client.print(",\"temperature\":");
        client.print(localData.zphs01b_temperature, 1);
        client.print(",\"humidity\":");
        client.print(localData.zphs01b_humidity);
        client.print("}");
    } else {
        client.print(",\"air_quality\":null");
    }
    
    // MR007 Data
    if (localData.mr007_valid) {
        client.print(",\"mr007\":{");
        client.print("\"voltage\":");
        client.print(localData.mr007_voltage, 3);
        client.print(",\"lel_concentration\":");
        client.print(localData.mr007_lel, 1);
        client.print("}");
    } else {
        client.print(",\"mr007\":null");
    }
    
    // ME4-SO2 Data
    if (localData.me4so2_valid) {
        client.print(",\"me4_so2\":{");
        client.print("\"voltage\":");
        client.print(localData.me4so2_voltage, 4);
        client.print(",\"current_ua\":");
        client.print(localData.me4so2_current, 2);
        client.print(",\"so2_concentration\":");
        client.print(localData.me4so2_so2, 2);
        client.print("}");
    } else {
        client.print(",\"me4_so2\":null");
    }
    
    // Network info
    client.print(",\"ip_address\":\"");
    client.print(localData.ip_address);
    client.print("\",\"network_mode\":\"");
    
    // These method calls should now work with SensorNetworkManager
    if (networkManager.isEthernetActive()) client.print("eth");
    else if (networkManager.isWifiActive()) client.print("wifi");
    else if (networkManager.isAPActive()) client.print("ap");
    else client.print("unknown");
    
    client.println("\"}");
}

#endif