# 🔧 Configuration Guide - Smart Sensors Project

## Overview

This guide explains how to configure all three components (ESP32, Django, Frontend) of the Smart Sensors project. All configuration is centralized in easy-to-edit files.

---

## 📁 Configuration Files

| Component | Configuration File | Location |
|-----------|-------------------|----------|
| **Django Backend** | `config.py` | `smartsensors_django/config.py` |
| **ESP32 Hardware** | `config.h` | `main/config.h` (copy from `config_template.h`) |
| **React Frontend** | `config.js` | `smartsensors_front/src/config.js` |

---

## 🚀 Quick Setup (Most Important Setting)

### The ONE Setting You Must Change

**Change the Django server IP address in ALL THREE configuration files:**

1. **Django Backend** (`smartsensors_django/config.py`):
   ```python
   DJANGO_URL = 'http://192.168.1.100:8000'  # Change to your server IP
   ```

2. **ESP32 Hardware** (`main/config.h`):
   ```cpp
   #define DJANGO_SERVER_IP "192.168.1.100"  // Change to your server IP
   ```

3. **React Frontend** (`smartsensors_front/src/config.js`):
   ```javascript
   BASE_URL: 'http://192.168.1.100:8000',  // Change to your server IP
   ```

**That's it!** Replace `192.168.1.100` with your actual server IP address in all three files.

---

## 🔍 How to Find Your Server IP

### Linux/Mac:
```bash
ip addr show  # or ifconfig
# Look for inet 192.168.x.x
```

### Windows:
```bash
ipconfig
# Look for IPv4 Address: 192.168.x.x
```

### Check Current IP:
The IP address should be on the same network as your ESP32 device.

---

## 📋 Step-by-Step Configuration

### 1️⃣ Django Backend Configuration

**File:** `smartsensors_django/config.py`

#### Basic Setup:
```python
# Change this to your server IP
DJANGO_URL = 'http://192.168.1.100:8000'

# If running on localhost only:
# DJANGO_URL = 'http://localhost:8000'

# For production with domain:
# DJANGO_URL = 'https://your-domain.com'
```

#### Advanced Settings:

**CORS Configuration:**
```python
# Allow these frontend URLs to access the backend
CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',      # Vite dev server
    'http://192.168.1.100:5173',  # Network access
    'http://192.168.1.101:5173',  # Another computer
]

# For development only - allows all (DISABLE IN PRODUCTION!)
CORS_ALLOW_ALL_ORIGINS = True  # Set to False in production
```

**Security Settings:**
```python
# Rate limiting
RATE_LIMIT_MAX_REQUESTS = 100      # Max requests per minute
RATE_LIMIT_WINDOW_SECONDS = 60     # Time window

# Auto IP blocking
AUTO_BLOCK_VIOLATIONS_THRESHOLD = 5  # Violations before block
AUTO_BLOCK_DURATION_HOURS = 24       # Block duration
```

**Production Settings:**
```python
# IMPORTANT: Change these for production!
DEBUG_MODE = False  # Disable debug in production
SECRET_KEY = 'your-new-secret-key'  # Generate new key
```

#### Test Configuration:
```bash
cd smartsensors_django
python config.py
```

This will print configuration summary and show any warnings/errors.

---

### 2️⃣ ESP32 Hardware Configuration

**Files:**
- Template: `main/config_template.h`
- Your config: `main/config.h` (create from template)

#### First Time Setup:

1. **Copy the template:**
   ```bash
   cd main
   cp config_template.h config.h
   ```

2. **Edit `config.h`** and change Django server IP:
   ```cpp
   #define DJANGO_SERVER_IP "192.168.1.100"  // Your server IP
   #define DJANGO_SERVER_PORT 8000
   ```

#### Network Configuration:

**Option 1: Ethernet (Recommended)**
```cpp
#define NETWORK_MODE "ethernet"
#define ETHERNET_CS_PIN 5
#define ETHERNET_RESET_PIN 17

// Use DHCP (automatic IP)
#define ETHERNET_USE_STATIC_IP false

// Or use static IP
#define ETHERNET_USE_STATIC_IP true
#define ETHERNET_STATIC_IP "192.168.1.150"
#define ETHERNET_GATEWAY "192.168.1.1"
#define ETHERNET_SUBNET "255.255.255.0"
```

**Option 2: WiFi**
```cpp
#define NETWORK_MODE "wifi"
#define WIFI_SSID "YourWiFiName"
#define WIFI_PASSWORD "YourWiFiPassword"

// Use DHCP (automatic IP)
#define WIFI_USE_STATIC_IP false

// Or use static IP
#define WIFI_USE_STATIC_IP true
#define WIFI_STATIC_IP "192.168.1.151"
```

**Option 3: Access Point (AP)**
```cpp
#define NETWORK_MODE "ap"
#define AP_SSID "SmartSensors_AP"
#define AP_PASSWORD "12345678"
#define AP_LOCAL_IP "192.168.4.1"
```

#### Sensor Configuration:

Enable/disable sensors:
```cpp
#define ZPHS01B_ENABLED true   // Particulate matter
#define MR007_ENABLED true     // Combustible gas
#define ME4SO2_ENABLED true    // SO2
#define ZE40_ENABLED true      // TVOC
```

#### Timing Configuration:

```cpp
#define SENSOR_READ_INTERVAL 2000   // Read sensors every 2 seconds
#define DJANGO_SEND_INTERVAL 2000   // Send to Django every 2 seconds
```

#### Upload to ESP32:

After editing `config.h`:
```bash
platformio run --target upload
```

#### View Configuration:

Open Serial Monitor (115200 baud) to see configuration printed on startup.

---

### 3️⃣ React Frontend Configuration

**File:** `smartsensors_front/src/config.js`

#### Basic Setup:
```javascript
export const API_CONFIG = {
  // Change this to your Django server IP
  BASE_URL: 'http://192.168.1.100:8000',
  
  // For localhost development:
  // BASE_URL: 'http://localhost:8000',
  
  // For production with domain:
  // BASE_URL: 'https://your-domain.com',
};
```

#### Polling Configuration:

How often to fetch data:
```javascript
export const POLLING_CONFIG = {
  SENSOR_DATA_INTERVAL: 2000,  // 2 seconds
  DEVICE_INFO_INTERVAL: 2000,   // 2 seconds
  GAS_SENSORS_INTERVAL: 2000,   // 2 seconds
};
```

#### Logging Configuration:

```javascript
export const LOGGING_CONFIG = {
  ENABLED: true,              // Enable logging
  SEND_TO_BACKEND: true,      // Send logs to Django
  SEND_ONLY_ERRORS: true,     // Only send errors (saves bandwidth)
  MAX_LOCAL_LOGS: 100,        // Max logs in localStorage
};
```

#### Development vs Production:

```javascript
export const DEV_CONFIG = {
  DEBUG: true,           // Set to false in production
  CONSOLE_LOGS: true,    // Show logs in console
  USE_MOCK_DATA: false,  // Use fake data for testing
};
```

#### Test Configuration:

The configuration validates automatically when you import it. Check browser console for warnings/errors.

---

## 🔄 Network Scenarios

### Scenario 1: All on Same Network (Recommended)

```
┌──────────────────────────────────────┐
│  Network: 192.168.1.0/24             │
├──────────────────────────────────────┤
│  Router: 192.168.1.1                 │
│  Django Server: 192.168.1.100        │
│  ESP32 (DHCP): 192.168.1.150         │
│  Frontend Dev: 192.168.1.101         │
└──────────────────────────────────────┘
```

**Configuration:**
- Django: `DJANGO_URL = 'http://192.168.1.100:8000'`
- ESP32: `DJANGO_SERVER_IP = "192.168.1.100"`
- Frontend: `BASE_URL: 'http://192.168.1.100:8000'`

---

### Scenario 2: Django on Cloud/Remote Server

```
┌────────────────────────────────────────┐
│  Internet                              │
│  Django Server: your-domain.com        │
└────────────────────────────────────────┘
         ▲           ▲
         │           │
    ┌────┴────┐  ┌──┴──────┐
    │  ESP32  │  │ Frontend│
    └─────────┘  └─────────┘
```

**Configuration:**
- Django: `DJANGO_URL = 'https://your-domain.com'`
- ESP32: `DJANGO_SERVER_IP = "your-domain.com"`
- Frontend: `BASE_URL: 'https://your-domain.com'`

**Note:** Use HTTPS for security!

---

### Scenario 3: ESP32 Static IP (No DHCP)

If your network doesn't have DHCP, or you want a fixed IP:

```cpp
// ESP32 config.h
#define ETHERNET_USE_STATIC_IP true
#define ETHERNET_STATIC_IP "192.168.1.150"
#define ETHERNET_GATEWAY "192.168.1.1"
#define ETHERNET_SUBNET "255.255.255.0"
#define ETHERNET_DNS "8.8.8.8"
```

---

## 🧪 Testing Your Configuration

### 1. Test Django Backend

```bash
cd smartsensors_django
python manage.py runserver 0.0.0.0:8000
```

Access from browser:
- Same computer: `http://localhost:8000`
- Other computer: `http://YOUR_SERVER_IP:8000`

### 2. Test ESP32

1. Upload firmware with your config.h
2. Open Serial Monitor (115200 baud)
3. Look for:
   ```
   Configuration printed on startup
   Network connection status
   Django communication logs
   ```

### 3. Test Frontend

```bash
cd smartsensors_front
npm run dev
```

Check browser console for:
- Configuration validation messages
- API calls to Django
- Any CORS errors

---

## ⚠️ Common Issues

### Issue 1: ESP32 Can't Connect to Django

**Symptoms:**
- Serial Monitor shows "Failed to connect"
- HTTP errors in ESP32 logs

**Solutions:**
1. **Check IP address** in `config.h`:
   ```cpp
   #define DJANGO_SERVER_IP "192.168.1.100"  // Correct IP?
   ```

2. **Check Django is running**:
   ```bash
   curl http://192.168.1.100:8000/api/sensors
   ```

3. **Check network connectivity**:
   - ESP32 and Django on same network?
   - Firewall blocking port 8000?
   - Ping Django server from ESP32's network

4. **Check Django ALLOWED_HOSTS** in `config.py`:
   ```python
   ALLOWED_HOSTS = ['*']  # Allow all (for testing)
   ```

---

### Issue 2: Frontend Can't Connect to Django

**Symptoms:**
- Browser console shows CORS errors
- Network errors in developer tools

**Solutions:**
1. **Check API URL** in `config.js`:
   ```javascript
   BASE_URL: 'http://192.168.1.100:8000',  // Correct IP?
   ```

2. **Check CORS settings** in Django `config.py`:
   ```python
   CORS_ALLOW_ALL_ORIGINS = True  # For testing
   ```

3. **Check Django is accessible**:
   - Open `http://192.168.1.100:8000` in browser
   - Should see Django page

4. **Check browser network tab**:
   - See the actual request URL
   - Check response status code

---

### Issue 3: ESP32 Gets DHCP IP But It Changes

**Problem:** ESP32 gets different IP each time it restarts.

**Solution 1 - Use Static IP:**
```cpp
#define ETHERNET_USE_STATIC_IP true
#define ETHERNET_STATIC_IP "192.168.1.150"
```

**Solution 2 - DHCP Reservation:**
Configure your router to always give the same IP to ESP32's MAC address.

**Solution 3 - Use mDNS:**
```cpp
#define ENABLE_MDNS true
```
Then access via: `http://smartsensors.local`
(Django must also support mDNS)

---

### Issue 4: Configuration Not Taking Effect

**ESP32:**
- Did you upload new firmware after changing config.h?
- Run: `platformio run --target upload`

**Django:**
- Did you restart Django server after changing config.py?
- Press Ctrl+C and run again

**Frontend:**
- Did you restart Vite dev server?
- Press Ctrl+C and run `npm run dev` again
- Clear browser cache (Ctrl+Shift+R)

---

## 🔒 Production Configuration Checklist

Before deploying to production:

### Django (`config.py`):
- [ ] Change `SECRET_KEY` to new random value
- [ ] Set `DEBUG_MODE = False`
- [ ] Set `CORS_ALLOW_ALL_ORIGINS = False`
- [ ] Update `CORS_ALLOWED_ORIGINS` with actual frontend URLs
- [ ] Update `ALLOWED_HOSTS` with actual domain
- [ ] Consider using PostgreSQL instead of SQLite
- [ ] Set up proper database backups

### ESP32 (`config.h`):
- [ ] Use production Django URL
- [ ] Consider using static IP
- [ ] Disable verbose debug output if desired
- [ ] Set appropriate timeouts

### Frontend (`config.js`):
- [ ] Update `BASE_URL` to production Django URL
- [ ] Set `DEV_CONFIG.DEBUG = false`
- [ ] Disable console logs in production
- [ ] Use HTTPS for API calls if available

---

## 📝 Configuration File Templates

### Quick Reference Card

Print this and keep it handy:

```
===========================================
SMART SENSORS - CONFIGURATION QUICK REF
===========================================

Django Server IP: ___________________
Django Port: 8000

Django URL Format:
http://YOUR_IP:8000

ESP32 Network Mode: [ ] Ethernet
                    [ ] WiFi
                    [ ] AP

WiFi Credentials (if using WiFi):
SSID: _______________________________
Password: ____________________________

ESP32 IP (if static): ________________

Frontend Dev Machine IP: _____________

===========================================
```

---

## 🆘 Getting Help

### View Current Configuration:

**Django:**
```bash
cd smartsensors_django
python config.py
```

**Frontend:**
Check browser console on startup for configuration summary.

**ESP32:**
Open Serial Monitor - configuration printed on startup.

### Validate All Configurations:

```bash
# Django
python smartsensors_django/config.py

# Frontend (check browser console)
npm run dev

# ESP32 (check serial monitor)
platformio device monitor
```

---

## 📚 Related Documentation

- **PROJECT_DOCUMENTATION.md** - Overall project architecture
- **LOGGING_AND_SECURITY.md** - Security configuration details
- **README.md** - General project information

---

## 🎯 Summary

1. **Find your Django server IP address**
2. **Update IP in ALL THREE config files**:
   - `smartsensors_django/config.py` → `DJANGO_URL`
   - `main/config.h` → `DJANGO_SERVER_IP`
   - `smartsensors_front/src/config.js` → `BASE_URL`
3. **For ESP32**: Copy `config_template.h` to `config.h` first
4. **Upload new firmware to ESP32** after changing config
5. **Restart Django and Frontend** after changing configs
6. **Test each component** individually before connecting them

---

**Last Updated:** November 22, 2025  
**Version:** 1.0  
**Status:** Complete ✅
