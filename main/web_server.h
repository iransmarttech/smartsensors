#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include "config.h"

#ifdef WEB_SERVER_ENABLED

#ifdef WIFI_FALLBACK_ENABLED
#include <WiFi.h>
#endif

#ifdef ETHERNET_ENABLED
#include <Ethernet.h>
// Forward declaration to avoid early construction
class EthernetServer;
#endif

#include "network_manager.h"

// Connection management - removed to prevent pointer issues
// Each connection is now handled immediately without tracking

class WebServer {
public:
    void init();
    void handleEthernetClient();
    void handleWiFiClient();

private:
    void sendMainPage(Client &client);
    void sendJSONData(Client &client);
    void handleHTTPRequest(Client &client);
    
    volatile int activeClients = 0;
    
    #ifdef ETHERNET_ENABLED
    EthernetServer* ethServer = nullptr;
    #endif
    
    #ifdef WIFI_FALLBACK_ENABLED
    WiFiServer* wifiServer = nullptr;
    #endif
};

extern WebServer webServer;

#endif

#endif