#ifndef DJANGO_CLIENT_H
#define DJANGO_CLIENT_H

#include <Arduino.h>
#include "config.h"

class DjangoClient {
public:
    static void init();
    static void sendSensorData();
    static void setServerURL(const char* url);
    
private:
    static String serverURL;
    static unsigned long lastSendTime;
    static const unsigned long SEND_INTERVAL = 10000; // Send every 10 seconds
    
    static String buildJSONPayload();
    static bool sendHTTPPOST(const String& url, const String& payload);
};

#endif
