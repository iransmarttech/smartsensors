#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <Arduino.h>
#include "config.h"

// CHANGED: Renamed to avoid conflict with ESP32 NetworkManager
class CustomNetworkManager {
public:
    #ifdef ETHERNET_ENABLED
    bool initEthernet();
    #endif

    #ifdef WIFI_FALLBACK_ENABLED
    bool initWiFi();
    void startAccessPoint();
    #endif

    void update();
    bool isConnected();
    String getIPAddress();

    bool isEthernetActive() { return ethActive; }
    bool isWifiActive() { return wifiActive; }
    bool isAPActive() { return apActive; }

    #ifdef MDNS_ENABLED
    void initmDNS();
    #endif

private:
    bool ethActive = false;
    bool wifiActive = false;
    bool apActive = false;
};

// CHANGED: Updated extern declaration
extern CustomNetworkManager networkManager;

#endif