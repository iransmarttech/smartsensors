#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include "config.h"

// Renamed to avoid conflict with ESP32 NetworkManager
class SensorNetworkManager {
public:
    #ifdef ETHERNET_ENABLED
    bool initEthernet();
    #endif

    #ifdef WIFI_FALLBACK_ENABLED
    bool initWiFi();
    void startAccessPoint();
    #endif

    String getIPAddress();
    bool isEthernetActive() { return ethActive; }
    bool isWifiActive() { return wifiActive; }
    bool isAPActive() { return apActive; }
    
    // Security: Generate unique AP SSID using MAC address
    String generateAPSSID();

private:
    bool ethActive = false;
    bool wifiActive = false;
    bool apActive = false;

    #ifdef MDNS_ENABLED
    void initmDNS();
    #endif
};

// Updated extern declaration
extern SensorNetworkManager networkManager;

#endif