#include "django_client.h"
#include "shared_data.h"

String DjangoClient::serverURL = "";
unsigned long DjangoClient::lastSendTime = 0;

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
    
    DEBUG_PRINTLN("╔════════════════════════════════════════╗");
    DEBUG_PRINTLN("║   SENDING DATA TO DJANGO BACKEND      ║");
    DEBUG_PRINTLN("╚════════════════════════════════════════╝");
    
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(5000);  // 5 second timeout
    
    String payload = buildJSONPayload();
    
    DEBUG_PRINTLN("→ Target URL: " + serverURL);
    DEBUG_PRINTLN("→ Payload size: " + String(payload.length()) + " bytes");
    DEBUG_PRINTLN("→ Timestamp: " + String(millis() / 1000) + "s");
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("Payload:");
    DEBUG_PRINTLN(payload);
    DEBUG_PRINTLN("");
    
    unsigned long sendStart = millis();
    int httpResponseCode = http.POST(payload);
    unsigned long sendDuration = millis() - sendStart;
    
    if (httpResponseCode > 0) {
        String response = http.getString();
        
        DEBUG_PRINTLN("✓ Django Response Received:");
        DEBUG_PRINT("  Status Code: ");
        DEBUG_PRINTLN(httpResponseCode);
        DEBUG_PRINT("  Response Time: ");
        DEBUG_PRINT(sendDuration);
        DEBUG_PRINTLN(" ms");
        DEBUG_PRINT("  Response: ");
        DEBUG_PRINTLN(response);
        
        if (httpResponseCode == 200 || httpResponseCode == 207) {
            DEBUG_PRINTLN("✓ Data successfully stored in Django");
        } else {
            DEBUG_PRINTLN("⚠ Django returned non-success code");
        }
    } else {
        DEBUG_PRINTLN("✗ Django Communication Failed:");
        DEBUG_PRINT("  Error Code: ");
        DEBUG_PRINTLN(httpResponseCode);
        DEBUG_PRINT("  Error: ");
        DEBUG_PRINTLN(http.errorToString(httpResponseCode));
        DEBUG_PRINTLN("  Possible reasons:");
        DEBUG_PRINTLN("  - Django server not running");
        DEBUG_PRINTLN("  - Wrong URL configured");
        DEBUG_PRINTLN("  - Network connectivity issue");
        DEBUG_PRINTLN("  - Firewall blocking connection");
    }
    
    http.end();
    lastSendTime = millis();
    
    DEBUG_PRINTLN("═══════════════════════════════════════════");
    DEBUG_PRINTLN("");
}
