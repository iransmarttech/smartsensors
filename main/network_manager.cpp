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

// CHANGED: Updated to CustomNetworkManager
CustomNetworkManager networkManager;

#ifdef ETHERNET_ENABLED
bool CustomNetworkManager::initEthernet() {
    DEBUG_PRINTLN("Initializing Ethernet...");

    SPI.begin(ETH_SCK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN, ETH_CS_PIN);
    Ethernet.init(ETH_CS_PIN);

    if (Ethernet.begin(mac)) {
        DEBUG_PRINT("Ethernet connected. IP: ");
        DEBUG_PRINTLN(Ethernet.localIP());
        ethActive = true;

        #ifdef WIFI_FALLBACK_ENABLED
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);
        DEBUG_PRINTLN("WiFi disabled - Ethernet active");
        #endif

        #ifdef MDNS_ENABLED
        initmDNS();
        #endif

        return true;
    }

    DEBUG_PRINTLN("Ethernet connection failed");
    return false;
}
#endif

#ifdef WIFI_FALLBACK_ENABLED
bool CustomNetworkManager::initWiFi() {
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

void CustomNetworkManager::startAccessPoint() {
    DEBUG_PRINTLN("Starting Access Point...");

    WiFi.disconnect(true);
    WiFi.mode(WIFI_AP);

    if (WiFi.softAP(AP_SSID, AP_PASS)) {
        DEBUG_PRINT("AP started. IP: ");
        DEBUG_PRINTLN(WiFi.softAPIP());
        apActive = true;

        #ifdef MDNS_ENABLED
        initmDNS();
        #endif
    }
}
#endif

void CustomNetworkManager::update() {
    // Network maintenance
}

bool CustomNetworkManager::isConnected() {
    return ethActive || wifiActive || apActive;
}

String CustomNetworkManager::getIPAddress() {
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
void CustomNetworkManager::initmDNS() {
    if (MDNS.begin(HOSTNAME)) {
        MDNS.addService("http", "tcp", 80);
        DEBUG_PRINT("mDNS started: ");
        DEBUG_PRINT(HOSTNAME);
        DEBUG_PRINTLN(".local");
    }
}
#endif