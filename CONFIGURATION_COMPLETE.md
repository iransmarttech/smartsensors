# ✅ Centralized Configuration System - Complete

## What Was Implemented

I've created a **centralized configuration system** for your Smart Sensors project to make managing IP addresses and settings much easier, especially when ESP32 gets its IP via DHCP.

---

## 📁 Configuration Files Created

### 1. **Django Backend Configuration**
**File:** `smartsensors_django/config.py`

Contains all Django settings:
- Django server URL and port
- CORS allowed origins
- Rate limiting settings
- Auto IP blocking configuration
- Log retention policies
- Security settings
- Database configuration

**Key Setting:**
```python
DJANGO_URL = 'http://192.168.1.100:8000'  # Change to your server IP
```

**Test it:**
```bash
cd smartsensors_django
python config.py
```

---

### 2. **ESP32 Hardware Configuration**
**File:** `main/config.h` (copy from `main/config_template.h`)

Contains all ESP32 settings:
- Django server IP and port
- Network mode (Ethernet/WiFi/AP)
- Static IP vs DHCP
- WiFi credentials
- Sensor enable/disable flags
- Pin configurations
- Timing intervals
- Debug settings

**Key Setting:**
```cpp
#define DJANGO_SERVER_IP "192.168.1.100"  // Your Django server IP
#define DJANGO_SERVER_PORT 8000
```

**Setup:**
```bash
cd main
cp config_template.h config.h
# Edit config.h with your settings
```

---

### 3. **React Frontend Configuration**
**File:** `smartsensors_front/src/config.js`

Contains all frontend settings:
- Django API base URL
- Polling intervals
- Logging configuration
- Display settings
- Sensor thresholds
- Error handling
- Development vs Production settings

**Key Setting:**
```javascript
BASE_URL: 'http://192.168.1.100:8000',  // Your Django server IP
```

---

## 🎯 THE ONE THING YOU NEED TO DO

**When ESP32 gets a new IP or you move the Django server:**

1. Find your Django server's IP address:
   ```bash
   ip addr show   # Linux/Mac
   ipconfig       # Windows
   ```

2. Update THIS ONE setting in all 3 config files:

   - **Django** (`smartsensors_django/config.py`):
     ```python
     DJANGO_URL = 'http://YOUR_IP:8000'
     ```

   - **ESP32** (`main/config.h`):
     ```cpp
     #define DJANGO_SERVER_IP "YOUR_IP"
     ```

   - **Frontend** (`smartsensors_front/src/config.js`):
     ```javascript
     BASE_URL: 'http://YOUR_IP:8000',
     ```

3. Restart/reupload:
   - Django: Restart server
   - ESP32: Upload new firmware
   - Frontend: Restart dev server (or rebuild)

That's it! Everything else will work automatically.

---

## 🔄 Integration with Existing Code

All components now use the centralized configuration:

### Django (`settings.py`)
✅ Automatically imports from `config.py`
```python
from config import CONFIG
SECRET_KEY = CONFIG.get('SECRET_KEY')
DEBUG = CONFIG.get('DEBUG_MODE')
ALLOWED_HOSTS = CONFIG.get('ALLOWED_HOSTS')
```

### ESP32 (All `.ino` and `.cpp` files)
✅ Include `config.h` at the top
```cpp
#include "config.h"
// Now use DJANGO_SERVER_IP, DJANGO_SERVER_PORT, etc.
```

### Frontend (All React components)
✅ Import from `config.js`
```javascript
import { getSensorDataUrl, getPollingInterval } from '../config';

const API_URL = getSensorDataUrl();
const INTERVAL = getPollingInterval('sensor_data');
```

**Updated Components:**
- ✅ `AirQualityDashboard.jsx` - Uses centralized API URL and polling interval
- ✅ `GasSensorsPanel.jsx` - Uses centralized API URL and polling interval
- ✅ `DeviceInfoPanel.jsx` - Uses centralized API URL and polling interval
- ✅ `logger.js` - Uses centralized log URL and logging settings

---

## 📋 Benefits

### Before (Hardcoded):
```javascript
const API_BASE_URL = "http://localhost:8000";  // Component 1
const API_BASE_URL = "http://localhost:8000";  // Component 2
const API_BASE_URL = "http://localhost:8000";  // Component 3
// Change IP? Edit 10+ files! 😰
```

### After (Centralized):
```javascript
import { getSensorDataUrl } from '../config';
const API_BASE_URL = getSensorDataUrl();
// Change IP? Edit 1 file! 😄
```

---

## 🚀 Quick Start Guide

### Initial Setup:

1. **Django:**
   ```bash
   cd smartsensors_django
   # Edit config.py with your IP
   python config.py  # Test configuration
   python manage.py runserver 0.0.0.0:8000
   ```

2. **ESP32:**
   ```bash
   cd main
   cp config_template.h config.h
   # Edit config.h with your IP and network settings
   platformio run --target upload
   platformio device monitor  # View configuration
   ```

3. **Frontend:**
   ```bash
   cd smartsensors_front
   # Edit src/config.js with your IP
   npm run dev
   # Check browser console for configuration validation
   ```

---

## 🔧 Configuration Options

### Django (config.py)

**Network:**
- `DJANGO_URL` - Full Django URL for external access
- `DJANGO_HOST` - Host to bind (0.0.0.0 for all interfaces)
- `DJANGO_PORT` - Port number (default 8000)
- `CORS_ALLOWED_ORIGINS` - Frontend URLs allowed to access
- `ALLOWED_HOSTS` - Allowed hostnames

**Security:**
- `RATE_LIMIT_MAX_REQUESTS` - Max requests per minute
- `AUTO_BLOCK_VIOLATIONS_THRESHOLD` - Violations before block
- `AUTO_BLOCK_DURATION_HOURS` - Block duration

**Logging:**
- `LOG_RETENTION` - How long to keep each log type
- `LOGGING_ENABLED` - Enable/disable log types

---

### ESP32 (config.h)

**Network:**
- `NETWORK_MODE` - "ethernet", "wifi", or "ap"
- `DJANGO_SERVER_IP` - Django server IP
- `DJANGO_SERVER_PORT` - Django server port
- `ETHERNET_USE_STATIC_IP` - Use static or DHCP
- `WIFI_SSID` / `WIFI_PASSWORD` - WiFi credentials

**Sensors:**
- `ZPHS01B_ENABLED` - Enable/disable particulate sensor
- `MR007_ENABLED` - Enable/disable gas sensor
- `ME4SO2_ENABLED` - Enable/disable SO2 sensor
- `ZE40_ENABLED` - Enable/disable TVOC sensor

**Timing:**
- `SENSOR_READ_INTERVAL` - How often to read sensors (ms)
- `DJANGO_SEND_INTERVAL` - How often to send data (ms)

---

### Frontend (config.js)

**API:**
- `API_CONFIG.BASE_URL` - Django backend URL
- `API_CONFIG.TIMEOUT` - Request timeout
- `API_CONFIG.MAX_RETRIES` - Retry attempts

**Polling:**
- `POLLING_CONFIG.SENSOR_DATA_INTERVAL` - Data fetch interval
- `POLLING_CONFIG.MIN_INTERVAL` - Minimum safe interval

**Logging:**
- `LOGGING_CONFIG.ENABLED` - Enable logging
- `LOGGING_CONFIG.SEND_TO_BACKEND` - Send logs to Django
- `LOGGING_CONFIG.SEND_ONLY_ERRORS` - Only send errors
- `LOGGING_CONFIG.MAX_LOCAL_LOGS` - Max logs in localStorage

**Development:**
- `DEV_CONFIG.DEBUG` - Debug mode
- `DEV_CONFIG.CONSOLE_LOGS` - Show console logs
- `DEV_CONFIG.USE_MOCK_DATA` - Use fake data for testing

---

## 🧪 Testing Configuration

### Django:
```bash
cd smartsensors_django
python config.py
```
Shows:
- Django URL
- Debug mode
- CORS settings
- Rate limiting
- Validation warnings/errors

### ESP32:
Upload firmware and check Serial Monitor (115200 baud).
Shows:
- Device name and version
- Network mode
- Django URL
- Sensor status
- Timing configuration

### Frontend:
Start dev server and check browser console.
Shows:
- API base URL
- Environment (dev/prod)
- Debug mode
- Polling intervals
- Validation warnings

---

## 📚 Documentation

Created comprehensive guides:

1. **CONFIGURATION_GUIDE.md** (This file)
   - Complete setup instructions
   - Network scenarios
   - Troubleshooting
   - Common issues

2. **config.py** - Django configuration with inline comments
3. **config_template.h** - ESP32 configuration template with comments
4. **config.js** - Frontend configuration with JSDoc comments

---

## 🔍 Example Scenarios

### Scenario 1: ESP32 Gets New DHCP IP

**Problem:** ESP32 restarts, gets new IP from DHCP.

**Solution:** You don't need to change anything! ESP32 will:
1. Get new IP from DHCP
2. Still connect to Django using `DJANGO_SERVER_IP` from config
3. Django logs the ESP32's new IP automatically

**Only change config if:**
- Django server changes IP
- You move to different network

---

### Scenario 2: Moving to Production

**Django** (`config.py`):
```python
DEBUG_MODE = False
SECRET_KEY = 'new-random-secret-key'
CORS_ALLOW_ALL_ORIGINS = False
CORS_ALLOWED_ORIGINS = ['https://yourdomain.com']
DJANGO_URL = 'https://yourdomain.com'
```

**ESP32** (`config.h`):
```cpp
#define DJANGO_SERVER_IP "yourdomain.com"
#define DJANGO_SERVER_PORT 443  // HTTPS
```

**Frontend** (`config.js`):
```javascript
BASE_URL: 'https://yourdomain.com',
DEV_CONFIG.DEBUG: false,
```

---

### Scenario 3: Testing Without Hardware

**Frontend** (`config.js`):
```javascript
DEV_CONFIG.USE_MOCK_DATA: true,
```

This will use fake sensor data for testing the UI without ESP32 or Django.

---

## ✅ Checklist

### First Time Setup:
- [ ] Copy `config_template.h` to `config.h`
- [ ] Find your server IP address
- [ ] Update Django `config.py` with IP
- [ ] Update ESP32 `config.h` with IP
- [ ] Update Frontend `config.js` with IP
- [ ] Test Django: `python config.py`
- [ ] Upload ESP32 firmware
- [ ] Start frontend: `npm run dev`
- [ ] Check all components connect successfully

### When IP Changes:
- [ ] Find new server IP
- [ ] Update Django `config.py`
- [ ] Update ESP32 `config.h`
- [ ] Update Frontend `config.js`
- [ ] Restart Django server
- [ ] Upload ESP32 firmware
- [ ] Restart frontend dev server

---

## 🎉 Summary

✅ **Centralized configuration** - All settings in one place per component  
✅ **Easy IP management** - Change one setting, not 10+ files  
✅ **DHCP friendly** - ESP32 can get dynamic IP without issues  
✅ **Environment support** - Easy switch between dev/production  
✅ **Self-documenting** - Inline comments explain every setting  
✅ **Validation** - Automatic checks for common mistakes  
✅ **Backwards compatible** - Existing code still works  

**The main problem solved:** When ESP32 gets IP via DHCP, you only need to update the Django server IP in 3 config files (one per component), instead of hunting through dozens of source files!

---

**Created:** November 22, 2025  
**Version:** 1.0  
**Status:** Complete and Ready ✅
