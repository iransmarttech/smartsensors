#include "django_client.h"
#include "shared_data.h"
#include "network_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <Ethernet.h>
#include <EthernetClient.h>

String DjangoClient::serverURL = "";
unsigned long DjangoClient::lastSendTime = 0;

// Native socket-based HTTP POST to avoid HTTPClient mutex conflicts
bool DjangoClient::sendHTTPPOST(const String& url, const String& payload) {
    // Parse URL to extract host, port, and path
    String host = "";
    String path = "/api/sensors";
    int port = 80;
    
    // Remove http:// prefix if present
    String cleanURL = url;
    if (cleanURL.startsWith("http://")) {
        cleanURL = cleanURL.substring(7);
    }
    
    // Extract path
    int pathIndex = cleanURL.indexOf('/');
    if (pathIndex != -1) {
        path = cleanURL.substring(pathIndex);
        host = cleanURL.substring(0, pathIndex);
    } else {
        host = cleanURL;
    }
    
    // Extract port if specified
    int portIndex = host.indexOf(':');
    if (portIndex != -1) {
        port = host.substring(portIndex + 1).toInt();
        host = host.substring(0, portIndex);
    }
    
    // Parse host string to IP address
    // For W5500, we need to handle IP address directly or use the WIZnet DNS
    IPAddress serverIP;
    if (!serverIP.fromString(host)) {
        // If it's not a simple IP address, log and fail
        // (W5500 doesn't have simple DNS resolution)
        DEBUG_PRINTLN("✗ Host must be an IP address (DNS not available for W5500): " + host);
        DEBUG_PRINTLN("  Edit credentials.h and set DJANGO_SERVER_URL to your server's IP");
        return false;
    }
    
    DEBUG_PRINT("✓ Connecting to ");
    DEBUG_PRINT(serverIP);
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(port);
    
    // Create socket and connect
    EthernetClient client;
    unsigned long connectStart = millis();
    
    if (!client.connect(serverIP, port)) {
        DEBUG_PRINTLN("✗ Failed to connect to server");
        return false;
    }
    
    unsigned long connectTime = millis() - connectStart;
    DEBUG_PRINT("✓ Connected in ");
    DEBUG_PRINT(connectTime);
    DEBUG_PRINTLN("ms");
    
    // Build HTTP POST request
    String httpRequest = "POST " + path + " HTTP/1.1\r\n";
    httpRequest += "Host: " + host + "\r\n";
    httpRequest += "Content-Type: application/json\r\n";
    httpRequest += "Content-Length: " + String(payload.length()) + "\r\n";
    httpRequest += "Connection: close\r\n";
    httpRequest += "\r\n";
    httpRequest += payload;
    
    // Send request
    client.print(httpRequest);
    client.flush();
    
    DEBUG_PRINTLN("✓ Request sent, waiting for response...");
    
    // Read response
    unsigned long responseStart = millis();
    String response = "";
    int httpStatusCode = 0;
    bool headersParsed = false;
    
    unsigned long timeout = 10000;  // 10 second timeout
    while (client.connected() && (millis() - responseStart < timeout)) {
        if (client.available()) {
            char c = client.read();
            response += c;
            
            // Parse status line: "HTTP/1.1 200 OK\r\n"
            if (!headersParsed && response.indexOf("\r\n\r\n") != -1) {
                headersParsed = true;
                
                // Extract status code
                int statusStart = response.indexOf(' ') + 1;
                int statusEnd = response.indexOf(' ', statusStart);
                String statusStr = response.substring(statusStart, statusEnd);
                httpStatusCode = statusStr.toInt();
                
                DEBUG_PRINT("✓ HTTP Status: ");
                DEBUG_PRINTLN(httpStatusCode);
                
                // For success codes, we can return early
                if (httpStatusCode >= 200 && httpStatusCode < 300) {
                    client.stop();
                    return true;
                }
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
    
    client.stop();
    
    if (httpStatusCode == 0) {
        DEBUG_PRINTLN("✗ No valid HTTP response received");
        return false;
    }
    
    return (httpStatusCode >= 200 && httpStatusCode < 300);
}

void DjangoClient::init() {
    DEBUG_PRINTLN("Django Client initialized");
}

void DjangoClient::setServerURL(const char* url) {
    serverURL = String(url);
    DEBUG_PRINT("Django server URL set to: ");
    DEBUG_PRINTLN(serverURL);
}

String DjangoClient::buildJSONPayload() {
    if (!lockData(1000)) {
        DEBUG_PRINTLN("Failed to lock data for Django client");
        return "{}";
    }
    
    SharedSensorData localData = sharedData;
    unlockData();
    
    String json = "{";
    
    // ZE40 Data
    json += "\"ze40\":{";
    json += "\"tvoc_ppb\":" + String(localData.ze40_tvoc_ppb) + ",";
    json += "\"tvoc_ppm\":" + String(localData.ze40_tvoc_ppm, 3) + ",";
    json += "\"dac_voltage\":" + String(localData.ze40_dac_voltage, 2) + ",";
    json += "\"dac_ppm\":" + String(localData.ze40_dac_ppm, 3) + ",";
    json += "\"uart_data_valid\":" + String(localData.ze40_uart_valid ? "true" : "false") + ",";
    json += "\"analog_data_valid\":true";
    json += "},";
    
    // ZPHS01B Air Quality Data
    if (localData.zphs01b_valid) {
        json += "\"air_quality\":{";
        json += "\"pm1\":" + String(localData.zphs01b_pm1) + ",";
        json += "\"pm25\":" + String(localData.zphs01b_pm25) + ",";
        json += "\"pm10\":" + String(localData.zphs01b_pm10) + ",";
        json += "\"co2\":" + String(localData.zphs01b_co2) + ",";
        json += "\"voc\":" + String(localData.zphs01b_voc) + ",";
        json += "\"ch2o\":" + String(localData.zphs01b_ch2o) + ",";
        json += "\"co\":" + String(localData.zphs01b_co, 1) + ",";
        json += "\"o3\":" + String(localData.zphs01b_o3, 2) + ",";
        json += "\"no2\":" + String(localData.zphs01b_no2, 3) + ",";
        json += "\"temperature\":" + String(localData.zphs01b_temperature, 1) + ",";
        json += "\"humidity\":" + String(localData.zphs01b_humidity);
        json += "},";
    } else {
        json += "\"air_quality\":null,";
    }
    
    // MR007 Data
    if (localData.mr007_valid) {
        json += "\"mr007\":{";
        json += "\"voltage\":" + String(localData.mr007_voltage, 3) + ",";
        json += "\"rawValue\":" + String(localData.mr007_raw) + ",";
        json += "\"lel_concentration\":" + String(localData.mr007_lel, 1);
        json += "},";
    } else {
        json += "\"mr007\":null,";
    }
    
    // ME4-SO2 Data
    if (localData.me4so2_valid) {
        json += "\"me4_so2\":{";
        json += "\"voltage\":" + String(localData.me4so2_voltage, 4) + ",";
        json += "\"rawValue\":" + String(localData.me4so2_raw) + ",";
        json += "\"current_ua\":" + String(localData.me4so2_current, 2) + ",";
        json += "\"so2_concentration\":" + String(localData.me4so2_so2, 2);
        json += "},";
    } else {
        json += "\"me4_so2\":null,";
    }
    
    // Network Info
    json += "\"ip_address\":\"" + String(localData.ip_address) + "\",";
    
    // Network mode
    json += "\"network_mode\":\"";
    if (networkManager.isEthernetActive()) json += "eth";
    else if (networkManager.isWifiActive()) json += "wifi";
    else if (networkManager.isAPActive()) json += "ap";
    else json += "unknown";
    json += "\"";
    
    json += "}";
    
    return json;
}

void DjangoClient::sendSensorData() {
    // Check if enough time has passed since last send
    if (millis() - lastSendTime < SEND_INTERVAL) {
        return;
    }
    
    // Check if server URL is set
    if (serverURL.length() == 0) {
        DEBUG_PRINTLN("⚠ Django server URL not set");
        return;
    }
    
    // Check if network is available
    if (!networkManager.isEthernetActive() && !networkManager.isWifiActive()) {
        DEBUG_PRINTLN("⚠ No network connection available for Django upload");
        return;
    }
    
    // Add delay to allow Ethernet operations to complete
    vTaskDelay(pdMS_TO_TICKS(100));
    
    DEBUG_PRINTLN("╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   SENDING DATA TO DJANGO BACKEND       ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    String payload = buildJSONPayload();
    
    if (payload.length() == 0 || payload == "{}") {
        DEBUG_PRINTLN("⚠ Empty payload - skipping send");
        lastSendTime = millis();
        return;
    }
    
    DEBUG_PRINTLN("→ Target URL: " + serverURL);
    DEBUG_PRINTLN("→ Payload size: " + String(payload.length()) + " bytes");
    DEBUG_PRINTLN("→ Timestamp: " + String(millis() / 1000) + "s");
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("Payload:");
    DEBUG_PRINTLN(payload);
    DEBUG_PRINTLN("");
    
    unsigned long sendStart = millis();
    
    // Use native socket-based POST (avoids HTTPClient mutex conflicts)
    if (sendHTTPPOST(serverURL, payload)) {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✓ Data successfully sent to Django");
        DEBUG_PRINT("  Send Time: ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms");
    } else {
        unsigned long sendDuration = millis() - sendStart;
        DEBUG_PRINTLN("✗ Failed to send data to Django");
        DEBUG_PRINT("  Attempted for: ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms");
        DEBUG_PRINTLN("  Possible reasons:");
        DEBUG_PRINTLN("  - Django server not running");
        DEBUG_PRINTLN("  - Wrong URL configured");
        DEBUG_PRINTLN("  - Network connectivity issue");
        DEBUG_PRINTLN("  - Firewall blocking connection");
    }
    
    // Add delay after HTTP operation to let stack recover
    vTaskDelay(pdMS_TO_TICKS(100));
    
    lastSendTime = millis();
    
    DEBUG_PRINTLN("═══════════════════════════════════════════");
    DEBUG_PRINTLN("");
}
