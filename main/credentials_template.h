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
const char* WIFI_SSID = "YourWiFiNetworkName";
const char* WIFI_PASS = "YourStrongWiFiPassword123!";

// Access Point Mode Credentials
const char* AP_SSID_PREFIX = "SmartSensor-";
const char* AP_PASS = "StrongAPPassword123!";  // Minimum 16 characters

// Web Interface Authentication
const char* WEB_ADMIN_USERNAME = "admin";
const char* WEB_ADMIN_PASSWORD = "ChangeThisPassword123!";  // Minimum 16 characters

// API Access Token
const char* API_ACCESS_TOKEN = "generate-random-32-char-token-here";

// Django Backend Server URL
const char* DJANGO_SERVER_URL = "http://192.168.1.100:8000/api/sensors";  // Change to your Django server IP and port

// mDNS Hostname
const char* DEVICE_HOSTNAME = "smartSensor";

#endif
