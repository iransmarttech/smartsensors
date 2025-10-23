#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>  // ADD: Include Arduino first
#include "config.h"

#ifdef WEB_SERVER_ENABLED

// REORDERED: Include WiFi before Ethernet to avoid conflicts
#ifdef WIFI_FALLBACK_ENABLED
#include <WiFi.h>
#endif

#ifdef ETHERNET_ENABLED
#include <Ethernet.h>
#endif

#include "ze40_sensor.h"
#include "zphs01b_sensor.h"
#include "mr007_sensor.h"
#include "me4_so2_sensor.h"
#include "network_manager.h"

struct ConnectionState {
    Client* client;
    unsigned long lastActivity;
    bool inUse;
};

class WebServer {
public:
    void init();
    void handleEthernetClient();
    void handleWiFiClient();
    void sendJSONData(Client &client);

private:
    void sendMainPage(Client &client);
    void handleHTTPRequest(Client &client);
    void initConnections();
    void cleanupConnections();
    int getFreeConnectionSlot();
    
    ConnectionState connections[MAX_CONCURRENT_CONNECTIONS];
};

extern WebServer webServer;

#endif

#endif