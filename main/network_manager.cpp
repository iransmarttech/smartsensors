#include "network_manager.h"
#include "config.h"
#include <Arduino.h> 

#ifdef ETHERNET_ENABLED
#include <SPI.h>
#include <Ethernet.h>
uint8_t mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
#endif

#ifdef WIFI_FALLBACK_ENABLED
#include <WiFi.h>
#endif

#ifdef MDNS_ENABLED
#include <ESPmDNS.h>
#endif

// Updated to SensorNetworkManager
SensorNetworkManager networkManager;

#ifdef ETHERNET_ENABLED
bool SensorNetworkManager::initEthernet() {
    DEBUG_PRINTLN("Initializing Ethernet...");

    // CRITICAL: Disable interrupts during SPI initialization to prevent conflicts
    portMUX_TYPE ethMux = portMUX_INITIALIZER_UNLOCKED;
    portENTER_CRITICAL(&ethMux);
    
    // Initialize SPI with proper configuration for ESP32-S3
    SPI.begin(ETH_SCK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN, ETH_CS_PIN);
    
    portEXIT_CRITICAL(&ethMux);
    
    // Delay for SPI stabilization
    vTaskDelay(pdMS_TO_TICKS(200));
    
    DEBUG_PRINTLN("Initializing W5500 chip...");
    Ethernet.init(ETH_CS_PIN);
    
    // Additional delay before DHCP
    vTaskDelay(pdMS_TO_TICKS(200));

    DEBUG_PRINTLN("Starting DHCP...");
    if (Ethernet.begin(mac, 10000)) {
        DEBUG_PRINT("Ethernet connected. IP: ");
        DEBUG_PRINTLN(Ethernet.localIP());
        ethActive = true;

        #ifdef WIFI_FALLBACK_ENABLED
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        DEBUG_PRINTLN("WiFi disabled - Ethernet active");
        #endif

        #ifdef MDNS_ENABLED
        DEBUG_PRINTLN("Note: mDNS not supported over Ethernet on ESP32");
        DEBUG_PRINTLN("      Use IP address or configure router DNS");
        #endif

        return true;
    }

    DEBUG_PRINTLN("Ethernet connection failed");
    return false;
}
#endif

#ifdef WIFI_FALLBACK_ENABLED
bool SensorNetworkManager::initWiFi() {
    DEBUG_PRINTLN("Initializing WiFi...");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    uint32_t startTime = millis();
    while (WiFi.status() != WL_CONNECTED && (millis() - startTime < 15000)) {
        delay(500);
        DEBUG_PRINT(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        DEBUG_PRINT("\nWiFi connected. IP: ");
        DEBUG_PRINTLN(WiFi.localIP());
        wifiActive = true;

        #ifdef MDNS_ENABLED
        initmDNS();
        #endif

        return true;
    }

    DEBUG_PRINTLN("\nWiFi connection failed");
    return false;
}

void SensorNetworkManager::startAccessPoint() {
    DEBUG_PRINTLN("Starting Access Point...");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);

    // Generate unique SSID using MAC address
    String apSSID = generateAPSSID();
    
    DEBUG_PRINT("AP SSID: ");
    DEBUG_PRINTLN(apSSID);
    
    if (WiFi.softAP(apSSID.c_str(), AP_PASS)) {
        DEBUG_PRINT("AP started. IP: ");
        DEBUG_PRINTLN(WiFi.softAPIP());
        DEBUG_PRINT("AP Password: ");
        DEBUG_PRINTLN(AP_PASS);
        DEBUG_PRINTLN("SECURITY: Change default AP password in credentials.h!");
        apActive = true;

        #ifdef MDNS_ENABLED
        initmDNS();
        #endif
    }
}

String SensorNetworkManager::generateAPSSID() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char ssid[32];
    snprintf(ssid, sizeof(ssid), "%s%02X%02X%02X", 
             AP_SSID_PREFIX, mac[3], mac[4], mac[5]);
    return String(ssid);
}
#endif

String SensorNetworkManager::getIPAddress() {
    if (ethActive) {
        #ifdef ETHERNET_ENABLED
        return Ethernet.localIP().toString();
        #endif
    } else if (wifiActive) {
        #ifdef WIFI_FALLBACK_ENABLED
        return WiFi.localIP().toString();
        #endif
    } else if (apActive) {
        #ifdef WIFI_FALLBACK_ENABLED
        return WiFi.softAPIP().toString();
        #endif
    }
    return "0.0.0.0";
}

#ifdef MDNS_ENABLED
void SensorNetworkManager::initmDNS() {
    DEBUG_PRINTLN("Starting mDNS responder...");
    DEBUG_PRINT("Hostname: ");
    DEBUG_PRINTLN(DEVICE_HOSTNAME);
    
    if (!MDNS.begin(DEVICE_HOSTNAME)) {
        DEBUG_PRINTLN("✗ ERROR: mDNS.begin() failed!");
        DEBUG_PRINTLN("Possible reasons:");
        DEBUG_PRINTLN("  - Hostname already in use");
        DEBUG_PRINTLN("  - Invalid hostname format");
        DEBUG_PRINTLN("  - Network not ready");
        return;
    }
    
    // Add HTTP service advertisement
    if (MDNS.addService("http", "tcp", 80)) {
        DEBUG_PRINTLN("✓ HTTP service added to mDNS");
    } else {
        DEBUG_PRINTLN("✗ Failed to add HTTP service");
    }
    
    // Add additional service information
    MDNS.addServiceTxt("http", "tcp", "board", "ESP32-S3");
    MDNS.addServiceTxt("http", "tcp", "project", "Air Quality Monitor");
    
    DEBUG_PRINTLN("=============================================================");
    DEBUG_PRINTLN("✓ mDNS responder started successfully");
    DEBUG_PRINTLN("=============================================================");
    DEBUG_PRINT("Access via hostname: http://");
    DEBUG_PRINT(DEVICE_HOSTNAME);
    DEBUG_PRINTLN(".local/");
    DEBUG_PRINT("Access via IP:       http://");
    DEBUG_PRINTLN(getIPAddress());
    DEBUG_PRINTLN("=============================================================");
}
#endif