#include "credentials.h"

// ============================================================================
// CREDENTIALS DEFINITIONS
// ============================================================================
// This file contains the actual definitions of credentials declared in credentials.h
// ============================================================================

// WiFi Station Mode Credentials
const char* WIFI_SSID = "Morteza";
const char* WIFI_PASS = "miopmiow";

// Access Point Mode Credentials
const char* AP_SSID_PREFIX = "SmartSensor-app1";
const char* AP_PASS = "12341234";  // Minimum 8 characters

// Web Interface Authentication
const char* WEB_ADMIN_USERNAME = "admin";
const char* WEB_ADMIN_PASSWORD = "12341234";  // Minimum 8 characters

// API Access Token
const char* API_ACCESS_TOKEN = "generate-random-32-char-token-here";

// Django Backend Server URL
// Set to your laptop/Django server IP address
// For auto-detection to work reliably, use the actual IP (e.g., 192.168.1.4)
const char* DJANGO_SERVER_URL = "http://192.168.4.2:8000/api/sensors";

// mDNS Hostname
const char* DEVICE_HOSTNAME = "smartSensor";