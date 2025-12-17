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
#include "web_auth.h"

// Connection management - removed to prevent pointer issues
// Each connection is now handled immediately without tracking

class SensorWebServer {
public:
    void init();
    void handleEthernetClient();
    void handleWiFiClient();

private:
    void sendMainPage(Client &client, bool authenticated);
    void sendJSONData(Client &client, bool authenticated);
    void handleHTTPRequest(Client &client, const String& clientIP);
    void sendUnauthorized(Client &client);
    void sendForbidden(Client &client);
    void sendSecurityHeaders(Client &client);
    bool checkAuthentication(const String& authHeader);
    bool checkAPIToken(const String& tokenHeader);
    
    volatile int activeClients = 0;
    unsigned long lastRateLimitCleanup = 0;
    
    #ifdef ETHERNET_ENABLED
    EthernetServer* ethServer = nullptr;
    #endif
    
    #ifdef WIFI_FALLBACK_ENABLED
    WiFiServer* wifiServer = nullptr;
    #endif
};

extern SensorWebServer webServer;

#endif

#endif