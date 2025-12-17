# smartsensors

## Project Overview
This project is a smart sensor platform integrating hardware, backend, and frontend components for real-time environmental monitoring and device management.

### Folder Structure
- **main/**: ESP32/Arduino source code for sensor hardware, handling data collection and device networking.
- **smartsensors_Application_1.0.0/smartsensors_django/**: Django backend for storing sensor data, managing devices, and providing APIs/web interfaces.
- **smartsensors_Application_1.0.0/smartsensors_front/**: React/Vite frontend for visualizing sensor data and device status in a user-friendly dashboard.

---

## Data Flow

```
ESP32 Hardware (Sensors)
    ↓ HTTP POST every 10 seconds
Django Backend (API + Database)
    ↓ HTTP GET every 2 seconds
React Frontend (User Interface)
```

---

## Documentation

- 📘 **[Setup Guide](SETUP_GUIDE.md)** - Complete installation and configuration instructions
- 🏗️ **[Architecture](ARCHITECTURE.md)** - System architecture, data flow diagrams, and component relationships
- 📝 **[Implementation Summary](IMPLEMENTATION_SUMMARY.md)** - Overview of what was implemented and how it works

---

## Quick Start


### 1. Django Backend (LAN Access)
```bash
cd smartsensors_Application_1.0.0/smartsensors_django
pip install -r requirements.txt
python manage.py migrate
# Run Django on all interfaces for LAN access:
python manage.py runserver 0.0.0.0:8000
```

**LAN Access:**
- Make sure your `config.py` in `smartsensors_Application_1.0.0/smartsensors_django/` has:
    - `DJANGO_HOST = '0.0.0.0'`
    - `DJANGO_URL = 'http://192.168.1.X:8000'` (replace X with your server's LAN IP)
    - Add your LAN IP to `ALLOWED_HOSTS` if needed (see config.py)
- The backend will be accessible to all devices on your 192.168.1.0/24 network at `http://192.168.1.X:8000`

### 2. React Frontend
```bash
cd smartsensors_Application_1.0.0/smartsensors_front
npm install
npm run dev
```

### 2. React Frontend
```bash
cd smartsensors_front
npm install
npm run dev
```

### 3. ESP32 Hardware
1. Copy `main/credentials_template.h` to `main/credentials.h`
2. Configure WiFi credentials and Django server URL (use your LAN IP, e.g. `http://192.168.1.X:8000`) in `credentials.h`
3. Upload to ESP32-S3 board

---

## Features

✅ Real-time sensor data collection (ZE40, ZPHS01B, MR007, ME4-SO2)  
✅ Automatic data transmission from ESP32 to Django  
✅ RESTful API for data access  
✅ SQLite database for data storage  
✅ Real-time web dashboard with live charts  
✅ Historical data visualization  
✅ Network status monitoring  
✅ CORS-enabled for cross-origin requests  

---

## Technology Stack

- **Hardware**: ESP32-S3, Multiple environmental sensors
- **Backend**: Python, Django 5.2+, SQLite, django-cors-headers
- **Frontend**: React 19, Vite, TailwindCSS, Recharts
- **Communication**: HTTP REST API, JSON

---

## Support

For detailed setup instructions, troubleshooting, and API documentation, please refer to [SETUP_GUIDE.md](SETUP_GUIDE.md).
