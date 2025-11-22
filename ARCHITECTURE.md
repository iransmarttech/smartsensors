# Smart Sensors - Data Flow Architecture

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────────┐
│                         SMART SENSORS SYSTEM                         │
└─────────────────────────────────────────────────────────────────────┘

┌──────────────────────────┐
│   ESP32-S3 HARDWARE      │
│  (Sensor Board)          │
│                          │
│  ┌────────────────────┐  │
│  │  ZE40 TVOC Sensor  │  │
│  └────────────────────┘  │
│  ┌────────────────────┐  │
│  │ ZPHS01B Air Quality│  │
│  └────────────────────┘  │
│  ┌────────────────────┐  │
│  │ MR007 Combustible  │  │
│  └────────────────────┘  │
│  ┌────────────────────┐  │
│  │   ME4-SO2 Sensor   │  │
│  └────────────────────┘  │
│                          │
│  ┌────────────────────┐  │
│  │  Django Client     │  │
│  │  (HTTP POST)       │  │
│  └──────────┬─────────┘  │
└─────────────┼────────────┘
              │
              │ HTTP POST /api/sensors
              │ Every 10 seconds
              │ JSON Payload
              ▼
┌─────────────────────────────────────────┐
│      DJANGO BACKEND SERVER              │
│      (Python/Django)                    │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  POST /api/sensors                │  │
│  │  - Receives sensor data           │  │
│  │  - Validates JSON                 │  │
│  │  - Stores in database             │  │
│  └───────────────────────────────────┘  │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  SQLite Database                  │  │
│  │  Tables:                          │  │
│  │  - AirQuality                     │  │
│  │  - MR007                          │  │
│  │  - ME4SO2                         │  │
│  │  - ZE40                           │  │
│  │  - DeviceInfo                     │  │
│  └───────────────────────────────────┘  │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  GET /data                        │  │
│  │  - Returns latest sensor data     │  │
│  │  - Returns historical data (50)   │  │
│  │  - JSON format                    │  │
│  └─────────────┬─────────────────────┘  │
└────────────────┼────────────────────────┘
                 │
                 │ HTTP GET /data
                 │ Every 2 seconds
                 │ JSON Response
                 ▼
┌─────────────────────────────────────────┐
│      REACT FRONTEND                     │
│      (Vite + React + TailwindCSS)       │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  AirQualityDashboard.jsx          │  │
│  │  - Fetches air quality data       │  │
│  │  - Displays PM1, PM2.5, PM10      │  │
│  │  - Shows CO2, temperature, etc.   │  │
│  └───────────────────────────────────┘  │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  GasSensorsPanel.jsx              │  │
│  │  - Fetches MR007, ME4-SO2, ZE40   │  │
│  │  - Displays gas concentrations    │  │
│  └───────────────────────────────────┘  │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  DeviceInfoPanel.jsx              │  │
│  │  - Shows device IP address        │  │
│  │  - Shows network mode             │  │
│  │  - Shows last update time         │  │
│  └───────────────────────────────────┘  │
│                                         │
│  ┌───────────────────────────────────┐  │
│  │  Real-time Charts                 │  │
│  │  - Line charts for trends         │  │
│  │  - Historical data visualization  │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
                 │
                 ▼
        ┌────────────────┐
        │   USER         │
        │   (Web Browser)│
        └────────────────┘
```

---

## Data Flow Sequence

### 1. ESP32 → Django (Data Upload)

```
┌─────────┐                              ┌─────────┐
│  ESP32  │                              │ Django  │
└────┬────┘                              └────┬────┘
     │                                        │
     │ 1. Read sensors every few seconds     │
     │    (ZE40, ZPHS01B, MR007, ME4-SO2)    │
     │                                        │
     │ 2. Build JSON payload                 │
     │                                        │
     │ 3. HTTP POST /api/sensors             │
     │───────────────────────────────────────>│
     │    Content-Type: application/json     │
     │    Body: { air_quality: {...}, ... }  │
     │                                        │
     │                                        │ 4. Parse JSON
     │                                        │ 5. Validate data
     │                                        │ 6. Store in database
     │                                        │
     │ 7. HTTP 200 OK                        │
     │<───────────────────────────────────────│
     │    {"status":"success"}               │
     │                                        │
     │ 8. Wait 10 seconds                    │
     │                                        │
     │ 9. Repeat from step 1                 │
     │                                        │
```

### 2. Frontend → Django (Data Retrieval)

```
┌─────────┐                              ┌─────────┐
│Frontend │                              │ Django  │
└────┬────┘                              └────┬────┘
     │                                        │
     │ 1. HTTP GET /data                     │
     │───────────────────────────────────────>│
     │                                        │
     │                                        │ 2. Query database
     │                                        │ 3. Get latest records
     │                                        │ 4. Get history (50 records)
     │                                        │ 5. Build JSON response
     │                                        │
     │ 6. HTTP 200 OK                        │
     │<───────────────────────────────────────│
     │    {                                  │
     │      air_quality: {...},              │
     │      mr007: {...},                    │
     │      me4_so2: {...},                  │
     │      ze40: {...},                     │
     │      history: {...}                   │
     │    }                                  │
     │                                        │
     │ 7. Update UI components               │
     │ 8. Display charts                     │
     │                                        │
     │ 9. Wait 2 seconds                     │
     │                                        │
     │ 10. Repeat from step 1                │
     │                                        │
```

---

## Component Relationships

### ESP32 Components

```
main.ino
    ├── task_manager.cpp
    │   ├── initSensors()
    │   │   ├── ze40_sensor.cpp
    │   │   ├── zphs01b_sensor.cpp
    │   │   ├── mr007_sensor.cpp
    │   │   ├── me4_so2_sensor.cpp
    │   │   └── django_client.cpp  ← Initialize Django client
    │   │
    │   └── readSensors()
    │       ├── Read all sensors
    │       ├── Update shared_data
    │       └── django_client.sendSensorData()  ← Send to Django
    │
    ├── shared_data.cpp
    │   └── Global sensor data storage
    │
    ├── network_manager.cpp
    │   └── Ethernet/WiFi connectivity
    │
    └── config.h
        └── DJANGO_SERVER_URL
```

### Django Components

```
smartsensors_django/
    ├── sensors/
    │   ├── models.py
    │   │   ├── AirQuality
    │   │   ├── MR007
    │   │   ├── ME4SO2
    │   │   ├── ZE40
    │   │   └── DeviceInfo
    │   │
    │   ├── views.py
    │   │   ├── receive_sensor_data()  ← POST /api/sensors
    │   │   └── data_api()             ← GET /data
    │   │
    │   └── urls.py
    │       ├── /api/sensors → receive_sensor_data
    │       └── /data → data_api
    │
    └── settings.py
        ├── CORS configuration
        └── Database configuration
```

### Frontend Components

```
smartsensors_front/src/
    ├── App.jsx
    │   └── Router configuration
    │
    ├── pages/
    │   ├── AirQualityDashboard.jsx
    │   │   ├── Fetch /data API
    │   │   ├── Display air quality sensors
    │   │   └── Show historical charts
    │   │
    │   ├── GasSensorsPanel.jsx
    │   │   ├── Fetch /data API
    │   │   └── Display MR007, ME4-SO2, ZE40
    │   │
    │   └── DeviceInfoPanel.jsx
    │       ├── Fetch /data API
    │       └── Display device information
    │
    └── components/
        ├── SensorCard.jsx
        ├── SensorCardWithChart.jsx
        └── RealTimeChart.jsx
```

---

## Network Configuration

### Development Environment

```
┌──────────────────────────────────────────────┐
│         Local Development Network            │
│                                              │
│  ┌────────────┐                              │
│  │   ESP32    │ WiFi/Ethernet                │
│  │ 192.168.1.X│────────┐                     │
│  └────────────┘        │                     │
│                        │                     │
│  ┌────────────┐        │                     │
│  │   Django   │        │                     │
│  │ localhost  │<───────┤                     │
│  │   :8000    │        │                     │
│  └────────────┘        │                     │
│                        │                     │
│  ┌────────────┐        │                     │
│  │  Frontend  │        │                     │
│  │ localhost  │<───────┘                     │
│  │   :5173    │                              │
│  └────────────┘                              │
│                                              │
└──────────────────────────────────────────────┘
```

### Production Environment

```
┌──────────────────────────────────────────────┐
│         Production Network                   │
│                                              │
│  ┌────────────┐                              │
│  │   ESP32    │ WiFi/Ethernet                │
│  │ 192.168.1.X│────────┐                     │
│  └────────────┘        │                     │
│                        │                     │
│  ┌────────────┐        │                     │
│  │   Django   │        │                     │
│  │   Nginx    │<───────┤                     │
│  │   :443     │        │                     │
│  │   (HTTPS)  │        │                     │
│  └────────────┘        │                     │
│                        │                     │
│  ┌────────────┐        │                     │
│  │  Frontend  │        │                     │
│  │   Nginx    │<───────┘                     │
│  │   :443     │                              │
│  │  (Static)  │                              │
│  └────────────┘                              │
│                                              │
└──────────────────────────────────────────────┘
```

---

## Key Configuration Files

### ESP32
- `main/credentials.h` - WiFi, Django URL, API tokens
- `main/config.h` - Feature flags, pins, intervals
- `main/django_client.h` - Send interval (10 seconds)

### Django
- `smartsensors_django/settings.py` - CORS, database, security
- `smartsensors_django/sensors/models.py` - Database schema
- `smartsensors_django/sensors/views.py` - API endpoints

### Frontend
- `smartsensors_front/src/pages/*.jsx` - API URL, fetch interval (2 seconds)
- `smartsensors_front/vite.config.js` - Build configuration
- `smartsensors_front/package.json` - Dependencies

---

## Timing Configuration

- **ESP32 sensor reading**: Variable (2-30 seconds per sensor)
- **ESP32 → Django**: Every 10 seconds
- **Frontend → Django**: Every 2 seconds
- **Data retention**: 50 historical records per sensor type

These intervals can be adjusted based on your requirements.
