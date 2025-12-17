#ifndef CREDENTIALS_TEMPLATE_H
#define CREDENTIALS_TEMPLATE_H

// ============================================================================
// CREDENTIALS TEMPLATE - SAFE TO COMMIT TO VERSION CONTROL
// ============================================================================
// INSTRUCTIONS:
// 1. Copy this file to "credentials.h"
// 2. Fill in your actual credentials in credentials.h
// 3. Add credentials.h to .gitignore
// 4. NEVER commit credentials.h to git
// ============================================================================

// WiFi Station Mode Credentials
extern const char* WIFI_SSID;
extern const char* WIFI_PASS;

// Access Point Mode Credentials
extern const char* AP_SSID_PREFIX;
extern const char* AP_PASS;

// Web Interface Authentication
extern const char* WEB_ADMIN_USERNAME;
extern const char* WEB_ADMIN_PASSWORD;

// API Access Token
extern const char* API_ACCESS_TOKEN;

// Django Backend Server URL
extern const char* DJANGO_SERVER_URL;

// mDNS Hostname
extern const char* DEVICE_HOSTNAME;

// NOTE: The actual values for these credentials should be defined in credentials.cpp
// Example values:
//   WIFI_SSID = "YourWiFiNetworkName";
//   WIFI_PASS = "YourStrongWiFiPassword123!";
//   AP_SSID_PREFIX = "SmartSensor-";
//   AP_PASS = "StrongAPPassword123!";
//   WEB_ADMIN_USERNAME = "admin";
//   WEB_ADMIN_PASSWORD = "ChangeThisPassword123!";
//   API_ACCESS_TOKEN = "generate-random-32-char-token-here";
//   DJANGO_SERVER_URL = "http://192.168.1.100:8000/api/sensors";
//   DEVICE_HOSTNAME = "smartSensor";

#endif
