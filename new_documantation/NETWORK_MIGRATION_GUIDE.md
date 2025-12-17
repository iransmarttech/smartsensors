# Network Migration Guide

## Overview

This guide explains how to configure the entire Smart Sensors system (ESP32 board, Django backend, React frontend) on a **different network** with new IP addresses. It also covers full installation and running steps for all components.

---

## Table of Contents

1. [Network Configuration Steps](#network-configuration-steps)
2. [Files to Change](#files-to-change)
3. [Installation & Setup](#installation--setup)
4. [Running Each Component](#running-each-component)
5. [Verification & Troubleshooting](#verification--troubleshooting)
6. [Example Scenarios](#example-scenarios)

---

## Network Configuration Steps

### Step 1: Plan Your Network

Before making any changes, decide on your new network setup:

| Component | Current Setup | Your New Setup |
|-----------|---------------|----------------|
| **Laptop (Django + React)** | `192.168.1.4` | `___________` |
| **Django Port** | `8000` | `___________` |
| **React Port** | `5173` | `___________` |
| **ESP32 Board** | `192.168.1.3` | `___________` |
| **WiFi SSID** | `Morteza` | `___________` |
| **WiFi Password** | `miopmiow` | `___________` |

**Example new network:**
- Laptop IP: `192.168.10.100`
- Django Port: `8000`
- React Port: `5173`
- Board IP: `192.168.10.50`

---

## Files to Change

### 1. **ESP32 Board Configuration** 
**File:** `main/credentials.cpp`

This file stores the board's connection settings and server addresses.

**What to change:**
- `WIFI_SSID` — Update to your new WiFi network name
- `WIFI_PASS` — Update to your new WiFi password
- `DJANGO_SERVER_URL` — Update to your laptop's new IP and port
- `DEVICE_HOSTNAME` — (Optional) Change device name for mDNS discovery

**Before:**
```cpp
const char* WIFI_SSID = "Morteza";
const char* WIFI_PASS = "miopmiow";
const char* DJANGO_SERVER_URL = "http://192.168.1.4:8000/api/sensors";
const char* DEVICE_HOSTNAME = "smartSensor";
```

**After (example for new network 192.168.10.x):**
```cpp
const char* WIFI_SSID = "YourNewNetworkName";
const char* WIFI_PASS = "YourNewPassword";
const char* DJANGO_SERVER_URL = "http://192.168.10.100:8000/api/sensors";
const char* DEVICE_HOSTNAME = "smartSensor";
```

**Note:** The IP in `DJANGO_SERVER_URL` must match where your Django server is running.

---

### 2. **Django Backend Configuration**
**File:** `smartsensors_Application_1.0.0/smartsensors_django/config.py`

This is the main configuration for the Django backend.

**What to change:**
- `DJANGO_URL` — Update to your laptop's new IP and port
- `ALLOWED_HOSTS` — Update to include your laptop and board IPs
- `CORS_ALLOWED_ORIGINS` — Update to include your React dev server IP and port

**Before:**
```python
DJANGO_HOST = '0.0.0.0'
DJANGO_PORT = 8000
DJANGO_URL = 'http://192.168.1.4:8000'

CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',
    'http://127.0.0.1:5173',
    'http://192.168.1.4:5173',
    'http://localhost:3000',
]

ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '192.168.1.4',
    '192.168.1.3',
    '192.168.1.0',
]
```

**After (example for new network 192.168.10.x):**
```python
DJANGO_HOST = '0.0.0.0'
DJANGO_PORT = 8000
DJANGO_URL = 'http://192.168.10.100:8000'

CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',
    'http://127.0.0.1:5173',
    'http://192.168.10.100:5173',  # Your laptop's new IP
    'http://localhost:3000',
]

ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '192.168.10.100',  # Your laptop's new IP
    '192.168.10.50',   # Board's new IP
    '192.168.10.0',    # Entire subnet
]
```

---

### 3. **React Frontend Configuration**
**File:** `smartsensors_Application_1.0.0/smartsensors_front/src/config.js`

This tells the React frontend where to find the Django backend API.

**What to change:**
- `BASE_URL` under `API_CONFIG` — Update to your laptop's new IP and port

**Before:**
```javascript
export const API_CONFIG = {
  BASE_URL: 'http://192.168.1.4:8000',
  // ... rest of config
};
```

**After (example for new network 192.168.10.x):**
```javascript
export const API_CONFIG = {
  BASE_URL: 'http://192.168.10.100:8000',  // Your laptop's new IP
  // ... rest of config
};
```

---

### 4. **Django Settings (Optional for Advanced Cases)**
**File:** `smartsensors_Application_1.0.0/smartsensors_django/smartsensors_django/settings.py`

Usually you don't need to touch this if `config.py` is properly set, but if you encounter issues:

Look for any hardcoded IPs and replace them with your new network values.

**Check sections:**
- `ALLOWED_HOSTS`
- `CORS_ALLOWED_ORIGINS`
- Any static file serving paths

---

## Installation & Setup

### Prerequisites

Ensure you have the following installed on your laptop:

- **Python 3.9+** (for Django)
- **Node.js 16+** (for React)
- **pip** (Python package manager)
- **npm** or **yarn** (Node package manager)

Check what you have:
```bash
python3 --version
node --version
npm --version
```

---

## Django Backend Installation & Running

### Step 1: Install Django Requirements

Navigate to the Django project directory:

```bash
cd smartsensors_Application_1.0.0/smartsensors_django
```

Create a Python virtual environment (recommended):

```bash
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
```

Install dependencies from `requirements.txt`:

```bash
pip install -r requirements.txt
```

**If `requirements.txt` is missing or incomplete, install manually:**
```bash
pip install django
pip install djangorestframework
pip install django-cors-headers
pip install python-dotenv
```

### Step 2: Run Database Migrations

```bash
python manage.py migrate
```

This sets up the SQLite database (`db.sqlite3`).

### Step 3: Create a Superuser (Optional)

If you want to use Django admin panel:

```bash
python manage.py createsuperuser
```

Follow prompts to create admin username and password.

### Step 4: Run Django Server

**For local development only (localhost):**
```bash
python manage.py runserver
```

**To make it accessible on your network (bind to all interfaces):**
```bash
python manage.py runserver 0.0.0.0:8000
```

**To specify a custom IP address:**
```bash
python manage.py runserver 192.168.10.100:8000  # Use your laptop's actual IP
```

When Django starts, you should see:
```
Starting development server at http://0.0.0.0:8000/
Quit the server with CONTROL-C.
```

**Django is now running and accessible on your network!**

---

## React Frontend Installation & Running

### Step 1: Install React Dependencies

Navigate to the React project directory:

```bash
cd smartsensors_Application_1.0.0/smartsensors_front
```

Install all dependencies:

```bash
npm install
```

This reads `package.json` and installs all required packages.

### Step 2: Run Vite Dev Server

**Basic command:**
```bash
npm run dev
```

**To bind to all network interfaces (makes it accessible on LAN):**
```bash
HOST=0.0.0.0 npm run dev
```

**With explicit HMR configuration for your network IP:**
```bash
HMR_HOST=192.168.10.100 npm run dev
```

When the dev server starts, you'll see output like:
```
  VITE v7.1.7  ready in 256 ms

  ➜  Local:   http://localhost:5173/
  ➜  Network: http://192.168.10.100:5173/
```

**Access the frontend:**
- Locally: `http://localhost:5173`
- From another device on the network: `http://192.168.10.100:5173` (use your laptop's IP)

---

## Running Each Component

### Summary: All Commands to Run Everything

#### Terminal 1: Django Backend
```bash
cd smartsensors_Application_1.0.0/smartsensors_django
source venv/bin/activate  # Activate virtual environment
python manage.py runserver 0.0.0.0:8000
```

#### Terminal 2: React Frontend
```bash
cd smartsensors_Application_1.0.0/smartsensors_front
npm run dev
```

#### Terminal 3: Monitor ESP32
Use Arduino IDE or VS Code with PlatformIO:
1. Ensure `credentials.cpp` has been updated with new network settings
2. Compile and upload to the ESP32 board
3. Monitor serial output to verify connections

---

## Verification & Troubleshooting

### Checklist: Is Everything Working?

#### 1. **Django Backend**

Check if Django is running:

```bash
curl http://localhost:8000
# or use your actual IP:
curl http://192.168.10.100:8000
```

You should see a response (not a connection error).

Check API endpoint for sensor data:

```bash
curl http://192.168.10.100:8000/data
```

Should return JSON data (possibly empty if no board has sent data yet).

#### 2. **React Frontend**

Open your browser and navigate to:
- Local: `http://localhost:5173`
- Network: `http://192.168.10.100:5173`

You should see the frontend dashboard. Check the browser console (F12) for any errors.

#### 3. **ESP32 Board Communication**

Open Arduino IDE Serial Monitor or use Vite dev tools to monitor:
1. Board connects to WiFi (check serial output)
2. Board sends HTTP POST to Django (check for "✓ Data sent" or error messages)
3. Django receives the data (check Django logs and `/data` endpoint)
4. React displays the data (check dashboard on frontend)

---

### Troubleshooting: Common Issues

#### Issue: React Page Won't Load from Network IP

**Solution:**
1. Ensure Vite is running with `HOST=0.0.0.0` or check `vite.config.js` has server.host set
2. Check firewall is not blocking port 5173
3. Verify you're using the correct laptop IP address

```bash
# Kill existing Vite process and restart
HOST=0.0.0.0 npm run dev
```

#### Issue: ESP32 Board Can't Connect to Django

**Solution:**
1. Verify `DJANGO_SERVER_URL` in `credentials.cpp` is correct
2. Ensure board and laptop are on the same network
3. Check that Django is running and accessible: `curl http://[LAPTOP_IP]:8000`
4. Look at ESP32 serial output for specific error messages

#### Issue: CORS Error in Browser Console

**Solution:**
1. Verify `CORS_ALLOWED_ORIGINS` in `config.py` includes your React frontend URL
2. If accessing from a different network, add that network's origin
3. Make sure you restarted Django after editing `config.py`

```python
CORS_ALLOWED_ORIGINS = [
    'http://192.168.10.100:5173',  # Make sure this is here
]
```

#### Issue: Django Shows "DisallowedHost" Error

**Solution:**
1. Check `ALLOWED_HOSTS` in `config.py` includes your laptop's IP
2. Restart Django server after making changes

```python
ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '192.168.10.100',  # Add your laptop IP here
]
```

---

## Example Scenarios

### Scenario 1: Moving to a Home WiFi Network

**Network details:**
- WiFi Network: "HomeNet"
- WiFi Password: "secure123"
- Laptop IP: 192.168.0.50
- Board will get: 192.168.0.100 (via DHCP)

**Changes needed:**

1. **credentials.cpp:**
```cpp
const char* WIFI_SSID = "HomeNet";
const char* WIFI_PASS = "secure123";
const char* DJANGO_SERVER_URL = "http://192.168.0.50:8000/api/sensors";
```

2. **config.py:**
```python
DJANGO_URL = 'http://192.168.0.50:8000'
CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',
    'http://127.0.0.1:5173',
    'http://192.168.0.50:5173',
]
ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '192.168.0.50',
    '192.168.0.100',
    '192.168.0.0',
]
```

3. **config.js:**
```javascript
BASE_URL: 'http://192.168.0.50:8000',
```

---

### Scenario 2: Moving to Corporate/Public Network

**Network details:**
- Laptop IP: 10.50.25.100
- Using domain instead of IP: myserver.example.com
- Board IP: 10.50.25.101

**Changes needed:**

1. **credentials.cpp:**
```cpp
// Using domain name (if W5500 supports it, otherwise use static IP)
const char* DJANGO_SERVER_URL = "http://myserver.example.com:8000/api/sensors";
// Or use the IP if domain doesn't resolve on board:
const char* DJANGO_SERVER_URL = "http://10.50.25.100:8000/api/sensors";
```

2. **config.py:**
```python
DJANGO_URL = 'http://myserver.example.com:8000'  # or use IP
CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',
    'http://127.0.0.1:5173',
    'http://10.50.25.100:5173',
    'http://myserver.example.com:5173',
]
ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '10.50.25.100',
    '10.50.25.101',
    '10.50.25.0',
    'myserver.example.com',
]
```

3. **config.js:**
```javascript
BASE_URL: 'http://myserver.example.com:8000',  // or use IP
```

---

### Scenario 3: Docker / Cloud Deployment

If deploying Django and React on a cloud server or Docker:

**Network details:**
- Server IP: 203.0.113.50 (public IP)
- Using HTTPS: https://myapp.example.com
- Board still on local WiFi but posting to cloud

**Changes needed:**

1. **credentials.cpp:**
```cpp
const char* DJANGO_SERVER_URL = "https://myapp.example.com/api/sensors";
// Note: Ensure your board's SSL certificates are trusted
```

2. **config.py:**
```python
DJANGO_URL = 'https://myapp.example.com'
CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',
    'http://127.0.0.1:5173',
    'https://myapp.example.com',
]
ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '203.0.113.50',
    'myapp.example.com',
]
# Enable HTTPS
SECURE_SSL_REDIRECT = True
CSRF_TRUSTED_ORIGINS = ['https://myapp.example.com']
```

3. **config.js:**
```javascript
BASE_URL: 'https://myapp.example.com',
```

---

## Quick Reference: All Configuration Values

### By File

| File | Key Setting | Purpose |
|------|-------------|---------|
| `main/credentials.cpp` | `DJANGO_SERVER_URL` | Board's target server URL |
| `main/credentials.cpp` | `WIFI_SSID` / `WIFI_PASS` | Board's WiFi connection |
| `config.py` | `DJANGO_URL` | Backend external URL |
| `config.py` | `ALLOWED_HOSTS` | IPs/domains allowed to connect |
| `config.py` | `CORS_ALLOWED_ORIGINS` | Frontend origins allowed to make requests |
| `config.js` | `BASE_URL` | Frontend's backend API address |
| `vite.config.js` | `server.host` | Dev server bind address |

---

## Summary

To set up the system on a new network:

1. **Plan** your new IP addresses and network
2. **Update 4 files:**
   - `main/credentials.cpp` (WiFi + server URL)
   - `smartsensors_django/config.py` (DJANGO_URL + ALLOWED_HOSTS + CORS)
   - `smartsensors_front/src/config.js` (BASE_URL)
   - (Optionally) `smartsensors_django/smartsensors_django/settings.py`
3. **Install dependencies:**
   - Django: `pip install -r requirements.txt`
   - React: `npm install`
4. **Run each component:**
   - Django: `python manage.py runserver 0.0.0.0:8000`
   - React: `npm run dev`
   - Board: Recompile and upload updated `credentials.cpp`
5. **Verify** with the checklist and examples above

---

## Additional Resources

- [Django Documentation](https://docs.djangoproject.com/)
- [React Documentation](https://react.dev/)
- [Vite Configuration](https://vitejs.dev/config/)
- [ESP32 Arduino Guide](https://docs.espressif.com/projects/arduino-esp32/en/latest/)

For questions or issues specific to your setup, check the serial output of the ESP32 and Django server logs for detailed error messages.
