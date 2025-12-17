# Network Configuration & Setup Guide

## Current Network Configuration

```
Your Laptop (Django Backend + React Frontend)
├── IP Address: 192.168.1.4
├── Django Server: http://192.168.1.4:8000
├── React Frontend: http://192.168.1.4:5173
└── OS: Linux

ESP32-S3 Board (Sensor Hardware)
├── IP Address: 192.168.1.3 (via Ethernet W5500)
├── Connected via: Ethernet LAN cable
└── Sensor data POST: http://192.168.1.4:8000/api/sensors
```

---

## Changes Made

### 1. **ESP32 Board Configuration** (`credentials.cpp`)
```cpp
// Updated DJANGO_SERVER_URL from 192.168.1.100 to your laptop's IP
const char* DJANGO_SERVER_URL = "http://192.168.1.4:8000/api/sensors";
```
✅ Now board will send data to the correct Django server

### 2. **Django Backend Configuration** (`config.py`)
```python
# Updated DJANGO_URL to point to your laptop
DJANGO_URL = 'http://192.168.1.4:8000'

# Updated ALLOWED_HOSTS to accept requests from board and LAN
ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '192.168.1.4',    # Your laptop
    '192.168.1.3',    # ESP32 board
    '192.168.1.0',    # Entire LAN subnet
]

# Updated CORS_ALLOWED_ORIGINS for frontend communication
CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',
    'http://127.0.0.1:5173',
    'http://192.168.1.4:5173',  # Your laptop's React frontend
]
```
✅ Django now accepts connections from board and frontend on the same network

### 3. **React Frontend Configuration** (`src/config.js`)
```javascript
// Updated BASE_URL to use your laptop's IP
export const API_CONFIG = {
  BASE_URL: 'http://192.168.1.4:8000',  // Your laptop's Django server
  // ... rest of config
};
```
✅ React frontend now fetches data from the correct Django server

---

## Step-by-Step Setup Instructions

### **Step 1: Update ESP32 Code**
The credentials have already been updated. You just need to:
1. Open Arduino IDE
2. Open the ESP32 project from `main/` folder
3. **Compile and Upload** to the board

Expected behavior after upload:
```
✓ Ethernet initialized successfully
✓ IP address: 192.168.1.3
✓ System ready - Web interface available
╔════════════════════════════════════════╗
║   SENDING DATA TO DJANGO BACKEND       ║
╚════════════════════════════════════════╝
→ Target URL: http://192.168.1.4:8000/api/sensors
✓ Connecting to 192.168.1.4:8000
✓ Connected in XXms
✓ Request sent, waiting for response...
✓ HTTP Status: 200
✓ Data successfully sent to Django
```

### **Step 2: Update Django Backend**
1. Navigate to the Django project:
   ```bash
   cd smartsensors_Application_1.0.0/smartsensors_django
   ```

2. Make sure Django is running:
   ```bash
   python manage.py runserver 0.0.0.0:8000
   ```
   
   ✅ Django should now be accessible at `http://192.168.1.4:8000`

### **Step 3: Update React Frontend**
The frontend configuration has been updated. To run it:
1. Navigate to the frontend project:
   ```bash
   cd smartsensors_Application_1.0.0/smartsensors_front
   ```

2. Make sure dependencies are installed:
   ```bash
   npm install
   ```

3. Start the development server:
   ```bash
   npm run dev
   ```

4. Open your browser and go to:
   ```
   http://192.168.1.4:5173
   ```

---

## Verification Checklist

- [ ] **Board Connection:**
  - [ ] Serial monitor shows "✓ Ethernet initialized successfully"
  - [ ] Board gets IP: 192.168.1.3
  - [ ] Data payload is built correctly

- [ ] **Django Communication:**
  - [ ] Board sends POST to http://192.168.1.4:8000/api/sensors
  - [ ] Serial shows "✓ HTTP Status: 200" or "✓ HTTP Status: 207"
  - [ ] Django receives data (check Django admin or logs)

- [ ] **Django to Frontend:**
  - [ ] React loads at http://192.168.1.4:5173
  - [ ] Check browser console (F12) for any CORS errors
  - [ ] Dashboard shows sensor data
  - [ ] Data updates every 2 seconds

---

## Data Flow Verification

### **Check ESP32 → Django**
```bash
# SSH into your laptop or use curl to check
curl -X GET http://192.168.1.4:8000/data

# Should return JSON with sensor data:
{
  "air_quality": { "pm25": ..., "pm10": ..., ... },
  "ze40": { "tvoc_ppb": ..., ... },
  "mr007": { "voltage": ..., ... },
  "me4_so2": { "so2_concentration": ..., ... },
  "ip_address": "192.168.1.3",
  "network_mode": "eth"
}
```

### **Check Django → React**
1. Open browser at http://192.168.1.4:5173
2. Press F12 to open Developer Tools
3. Go to Network tab
4. Refresh the page
5. You should see requests to `/data` endpoint returning 200 status with sensor data
6. Dashboard should display:
   - ZE40 TVOC readings
   - Air quality (PM2.5, PM10, CO2, temperature, humidity)
   - MR007 gas sensor readings
   - ME4-SO2 sulfur dioxide readings
   - Network status (Ethernet)
   - Device IP address

---

## Troubleshooting

### **Board can't connect to Django (192.168.1.4)**
- [ ] Check if Django is running: `python manage.py runserver 0.0.0.0:8000`
- [ ] Verify board has network connectivity: Serial shows "✓ Ethernet initialized"
- [ ] Check firewall isn't blocking port 8000
- [ ] Verify Django is listening on all interfaces (0.0.0.0)

### **React can't fetch data from Django**
- [ ] Check browser console (F12) for CORS errors
- [ ] Verify React is accessing correct URL (http://192.168.1.4:5173)
- [ ] Verify Django frontend config has correct BASE_URL
- [ ] Check Django CORS_ALLOWED_ORIGINS includes React URL

### **No data appearing in React dashboard**
- [ ] Check if board is sending data (serial monitor shows successful posts)
- [ ] Check Django database has records (go to /admin)
- [ ] Check browser Network tab to see actual response from /data endpoint
- [ ] Clear browser cache (Ctrl+Shift+Delete)

---

## What's Working Now

✅ **ESP32 to Django:**
- Board detects Ethernet connection
- Gets IP address (192.168.1.3)
- Sends JSON POST every 10 seconds
- Native socket implementation (no HTTPClient mutex issues)
- Uses correct server IP (192.168.1.4:8000)

✅ **Django Backend:**
- Receives POST requests from ESP32
- Stores sensor data in database
- Provides REST API at /data endpoint
- CORS enabled for frontend
- ALLOWED_HOSTS configured for network access

✅ **React Frontend:**
- Fetches sensor data from Django every 2 seconds
- Displays real-time readings
- Shows historical charts
- Network status monitoring
- Responsive design

---

## Next Steps (If Needed)

### **Auto-Detection of Django Server**
If you want the board to auto-detect Django's IP address instead of hardcoding it:
- We can implement mDNS hostname discovery
- Or implement network broadcast discovery
- Let us know if you need this feature!

### **Data Buffering**
The board will retry sending data if Django is temporarily unavailable:
- Logs failed attempts to serial monitor
- Retries at next interval
- No data loss during brief Django downtime

### **WiFi Fallback**
The board is configured to fall back to WiFi if Ethernet fails:
- Automatically connects to your WiFi (SSID: "Morteza")
- Or starts Access Point if WiFi fails
- All with the same Django communication

---

## Security Notes (For Production)

These are DEV settings. For production:
- [ ] Set `CORS_ALLOW_ALL_ORIGINS = False`
- [ ] Specify exact CORS origins
- [ ] Use HTTPS instead of HTTP
- [ ] Change default credentials in credentials.cpp
- [ ] Add firewall rules to restrict access
- [ ] Use API tokens for authentication

---

**All configurations have been updated. You're ready to test!** 🎉

