# Smart Sensors - Complete Setup Guide

This guide explains how to set up the complete data flow from ESP32 → Django → Frontend.

## Architecture Overview

```
ESP32 Hardware (Sensors)
    ↓ (HTTP POST every 10 seconds)
Django Backend (API + Database)
    ↓ (HTTP GET every 2 seconds)
React Frontend (User Interface)
```

## Prerequisites

- Python 3.8+ with pip
- Node.js 18+ with npm
- ESP32-S3 with Arduino IDE or PlatformIO
- Network connectivity (WiFi or Ethernet)

---

## Part 1: Django Backend Setup

### 1.1 Install Dependencies

```bash
cd smartsensors_django
pip install -r requirements.txt
```

### 1.2 Run Migrations

```bash
python manage.py makemigrations
python manage.py migrate
```

### 1.3 Create Superuser (Optional)

```bash
python manage.py createsuperuser
```

### 1.4 Start Django Server

```bash
python manage.py runserver 0.0.0.0:8000
```

The server will be accessible at:
- Local: http://127.0.0.1:8000
- Network: http://YOUR_IP:8000 (for ESP32 to access)

### 1.5 Verify API Endpoints

- Dashboard: http://127.0.0.1:8000/
- Data API (GET): http://127.0.0.1:8000/data
- Receive Sensor Data (POST): http://127.0.0.1:8000/api/sensors

---

## Part 2: Frontend Setup

### 2.1 Install Dependencies

```bash
cd smartsensors_front
npm install
```

### 2.2 Configure API URL

The API URL is configured in each component file:
- `src/pages/AirQualityDashboard.jsx`
- `src/pages/GasSensorsPanel.jsx`
- `src/pages/DeviceInfoPanel.jsx`

Default: `http://localhost:8000`

If Django is running on a different machine, update `API_BASE_URL` in these files.

### 2.3 Start Development Server

```bash
npm run dev
```

The frontend will be accessible at http://localhost:5173

---

## Part 3: ESP32 Hardware Setup

### 3.1 Configure Credentials

1. Copy `main/credentials_template.h` to `main/credentials.h`
2. Edit `credentials.h` and set:
   - WiFi credentials (if using WiFi)
   - Web authentication credentials
   - **Django Server URL** (very important!)

```cpp
// Django Backend Server URL
const char* DJANGO_SERVER_URL = "http://192.168.1.100:8000/api/sensors";
```

Replace `192.168.1.100` with your Django server's IP address.

### 3.2 Include Required Libraries

Make sure your ESP32 project includes:
- `HTTPClient.h` (for sending data to Django)
- `WiFi.h` or `Ethernet.h` (for network connectivity)

### 3.3 Upload Code to ESP32

1. Open `main/main.ino` in Arduino IDE
2. Select your ESP32-S3 board
3. Upload the code

### 3.4 Monitor Serial Output

Open Serial Monitor (115200 baud) to verify:
- Network connection established
- Django client initialized
- Data being sent to Django server

---

## Part 4: Testing the Data Flow

### 4.1 Verify ESP32 → Django

1. Check ESP32 serial monitor for messages like:
   ```
   Sending data to Django:
   Django response code: 200
   Django response: {"status":"success","message":"Data stored successfully"}
   ```

2. Check Django terminal for incoming POST requests:
   ```
   [22/Nov/2025 10:30:15] "POST /api/sensors HTTP/1.1" 200
   ```

3. Verify data in Django admin panel:
   - Go to http://127.0.0.1:8000/admin
   - Login with superuser credentials
   - Check AirQuality, MR007, ME4SO2, ZE40, DeviceInfo tables

### 4.2 Verify Django → Frontend

1. Open frontend at http://localhost:5173
2. Check browser console (F12) for:
   ```
   No errors
   API calls returning data successfully
   ```

3. Verify data is displayed and updating every 2 seconds

### 4.3 End-to-End Test

1. Change a physical sensor reading
2. Wait 10 seconds (ESP32 send interval)
3. Verify the change appears in Django database
4. Wait 2 seconds (Frontend fetch interval)
5. Verify the change appears in the frontend UI

---

## Troubleshooting

### ESP32 Cannot Connect to Django

- **Check network connectivity**: Ping Django server from another device
- **Verify DJANGO_SERVER_URL**: Must use IP address, not hostname (unless mDNS is set up)
- **Check firewall**: Django server port 8000 must be open
- **CORS issues**: Should be resolved by django-cors-headers configuration

### Frontend Shows "Error fetching data"

- **Django not running**: Make sure Django server is running
- **CORS error**: Check browser console for CORS errors
  - Solution: Verify `CORS_ALLOWED_ORIGINS` in Django settings.py includes `http://localhost:5173`
- **Wrong API URL**: Check API_BASE_URL in frontend components

### No Data in Django Database

- **ESP32 not sending**: Check serial monitor for errors
- **POST endpoint not working**: Test manually with curl:
  ```bash
  curl -X POST http://localhost:8000/api/sensors \
    -H "Content-Type: application/json" \
    -d '{"air_quality":{"pm25":50,"pm10":100,"co2":400,"temperature":25,"humidity":60}}'
  ```
- **Database issue**: Check Django migrations are applied

### Data Not Updating in Frontend

- **Stale data**: Clear browser cache and reload
- **JavaScript errors**: Check browser console for errors
- **API returning old data**: Check Django `/data` endpoint directly in browser

---

## Configuration Summary

### Django Settings
- File: `smartsensors_django/smartsensors_django/settings.py`
- CORS origins: `['http://localhost:5173', 'http://127.0.0.1:5173']`
- Allowed hosts: `['*']` (change for production)

### ESP32 Configuration
- File: `main/credentials.h`
- Django URL: `http://YOUR_DJANGO_IP:8000/api/sensors`
- Send interval: 10 seconds (configurable in `django_client.h`)

### Frontend Configuration
- Files: `src/pages/*.jsx`
- API URL: `http://localhost:8000` (or Django server IP)
- Fetch interval: 2 seconds

---

## Production Deployment Notes

### Security Considerations

1. **Django**:
   - Set `DEBUG = False`
   - Use proper `SECRET_KEY`
   - Restrict `ALLOWED_HOSTS`
   - Use HTTPS with SSL certificate
   - Configure proper CORS origins

2. **ESP32**:
   - Use HTTPS if Django is using SSL
   - Keep credentials.h secure
   - Implement token-based authentication

3. **Frontend**:
   - Build for production: `npm run build`
   - Deploy to production server
   - Use environment variables for API URLs

### Performance Optimization

- **Django**: Use PostgreSQL instead of SQLite
- **Frontend**: Implement data caching and debouncing
- **ESP32**: Adjust send interval based on needs (currently 10s)

---

## API Documentation

### POST /api/sensors
Receives sensor data from ESP32

**Request Body:**
```json
{
  "air_quality": {
    "pm1": 0,
    "pm25": 0,
    "pm10": 0,
    "co2": 0,
    "voc": 0,
    "ch2o": 0,
    "co": 0.0,
    "o3": 0.0,
    "no2": 0.0,
    "temperature": 0.0,
    "humidity": 0
  },
  "mr007": {
    "voltage": 0.0,
    "rawValue": 0,
    "lel_concentration": 0.0
  },
  "me4_so2": {
    "voltage": 0.0,
    "rawValue": 0,
    "current_ua": 0.0,
    "so2_concentration": 0.0
  },
  "ze40": {
    "tvoc_ppb": 0.0,
    "tvoc_ppm": 0.0,
    "dac_voltage": 0.0,
    "dac_ppm": 0.0,
    "uart_data_valid": false,
    "analog_data_valid": true
  },
  "ip_address": "192.168.1.100",
  "network_mode": "eth"
}
```

**Response:**
```json
{
  "status": "success",
  "message": "Data stored successfully"
}
```

### GET /data
Returns latest sensor data and history

**Response:**
```json
{
  "air_quality": { ... },
  "mr007": { ... },
  "me4_so2": { ... },
  "ze40": { ... },
  "ip_address": "192.168.1.100",
  "network_mode": "eth",
  "history": {
    "air_quality": [...],
    "mr007": [...],
    "me4_so2": [...],
    "ze40": [...]
  }
}
```

---

## Support

For issues or questions:
1. Check this setup guide
2. Review troubleshooting section
3. Check serial monitor (ESP32) and browser console (Frontend)
4. Verify all services are running
