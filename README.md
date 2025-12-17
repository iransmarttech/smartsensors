# Smart Sensors IoT Platform

## Overview

This project provides an integrated sensor monitoring system with hardware sensors, backend data storage, and web-based visualization. The system collects environmental data from multiple sensors on an ESP32 board and transmits it to a Django backend for storage and analysis.

## Project Structure

```
smartsensors/
├── main/                          # ESP32 firmware (Arduino/C++)
│   ├── config.h                  # Task stack sizes and feature flags
│   ├── credentials.h/cpp         # WiFi and server configuration
│   ├── buffer_manager.h/cpp      # Data buffering on flash storage
│   ├── django_client.h/cpp       # HTTP client for Django API
│   ├── network_manager.h/cpp     # Ethernet/WiFi connectivity
│   ├── shared_data.h/cpp         # Shared sensor data with mutex
│   ├── task_manager.h/cpp        # FreeRTOS task management
│   ├── web_server.h/cpp          # Local web interface
│   └── [sensor_name]_sensor.h/cpp # Individual sensor drivers
│
├── smartsensors_Application_1.0.0/
│   ├── smartsensors_django/      # Django backend
│   │   ├── config.py            # Django configuration
│   │   ├── manage.py            # Django management
│   │   ├── sensors/             # Sensor app (models, views, urls)
│   │   └── requirements.txt      # Python dependencies
│   └── smartsensors_front/       # React frontend
│       ├── src/config.js        # API endpoint configuration
│       ├── package.json         # Node dependencies
│       └── vite.config.js       # Vite dev server configuration
│
└── documentation/                # Technical documentation
    ├── NETWORK_MIGRATION_GUIDE.md
    ├── DATA_BUFFERING_GUIDE.md
    └── [other guides]
```

## System Architecture

```
ESP32 Sensors
  ├─ ZE40 (VOC/TVOC)
  ├─ ZPHS01B (Air Quality: PM, CO2, VOC, etc.)
  ├─ MR007 (Combustible Gas)
  └─ ME4-SO2 (SO2 Gas)
    │
    └──► Network (Ethernet W5500 or WiFi)
        │
        ├──► HTTP POST to Django API (192.168.1.4:8000)
        │    - Sends sensor readings every 10 seconds
        │    - Buffers data if network unavailable
        │    - Automatic retry when network returns
        │
        └──► Django Backend
             ├─ Stores readings in SQLite database
             ├─ Exposes REST API for data access
             └─ Handles CORS for frontend access
                │
                └──► React Frontend (192.168.1.4:5173)
                     ├─ Displays real-time dashboard
                     ├─ Polls sensor data every 2 seconds
                     └─ Shows historical charts
```

## Installation

### Prerequisites

- Python 3.9+
- Node.js 16+
- pip (Python package manager)
- npm (Node package manager)
- Arduino IDE or PlatformIO with ESP32 board support

### Django Backend

```bash
cd smartsensors_Application_1.0.0/smartsensors_django

# Create virtual environment
python3 -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt

# Run migrations
python manage.py migrate

# Start server (listen on all interfaces for LAN access)
python manage.py runserver 0.0.0.0:8000
```

Django will be accessible at `http://192.168.1.X:8000` (replace X with your laptop's IP).

### React Frontend

```bash
cd smartsensors_Application_1.0.0/smartsensors_front

# Install dependencies
npm install

# Start development server
npm run dev
```

Frontend will be accessible at `http://192.168.1.X:5173` (replace X with your laptop's IP).

For LAN access on the same network, use:
```bash
HOST=0.0.0.0 npm run dev
```

Or set environment variable and start normally:
```bash
HMR_HOST=192.168.1.X npm run dev
```

### ESP32 Firmware

1. Navigate to the `main/` directory in Arduino IDE or PlatformIO
2. Copy `credentials_template.h` to `credentials.h`
3. Configure in `credentials.h`:
   - WiFi SSID and password
   - Django server URL: `http://192.168.1.X:8000/api/sensors`
   - Replace X with your Django server's IP address
4. Compile and upload to ESP32-S3 board

## Configuration

### Network Setup

The system can run on any network. Update these files when changing networks:

1. **ESP32 Board** (`main/credentials.cpp`):
   - `WIFI_SSID` - Network name
   - `WIFI_PASS` - Network password
   - `DJANGO_SERVER_URL` - Backend API URL

2. **Django Backend** (`smartsensors_django/config.py`):
   - `DJANGO_URL` - External URL for API access
   - `ALLOWED_HOSTS` - List of allowed IPs/domains
   - `CORS_ALLOWED_ORIGINS` - Frontend URL

3. **React Frontend** (`smartsensors_front/src/config.js`):
   - `BASE_URL` - Django backend API address

See `NETWORK_MIGRATION_GUIDE.md` for detailed network configuration instructions.

## Data Buffering

When network connectivity is interrupted, sensor data is automatically saved to the ESP32's flash memory. Once the network reconnects, all buffered data is sent to Django.

- Maximum buffer: 500 KB (configurable)
- Capacity: 1000-1400 readings
- Automatic retry: Every 30 seconds
- Persistence: Survives power loss and reboots

See `new_documentation/DATA_BUFFERING_GUIDE.md` for implementation details.

## API Endpoints

### Django REST API

**Sensor Data Submission:**
```
POST /api/sensors
Content-Type: application/json

{
  "timestamp": 1702916400,
  "ze40": { "tvoc_ppb": 45.2, ... },
  "air_quality": { "pm1": 12, "pm25": 22.5, ... },
  "mr007": { "voltage": 2.4, ... },
  "me4_so2": { "so2": 15.3, ... },
  "ip_address": "192.168.1.3",
  "network_mode": "eth"
}
```

**Retrieve Sensor Data:**
```
GET /data
```

Returns all stored sensor readings in JSON format.

## Sensors Supported

- **ZE40**: VOC/TVOC measurement (UART)
- **ZPHS01B**: Air quality (PM1, PM2.5, PM10, CO2, VOC, CH2O, CO, O3, NO2, temperature, humidity)
- **MR007**: Combustible gas measurement
- **ME4-SO2**: SO2 gas measurement

## Technology Stack

- **Hardware**: ESP32-S3, W5500 Ethernet module
- **Firmware**: Arduino/C++, FreeRTOS
- **Backend**: Python 3.9+, Django 5.2, SQLite
- **Frontend**: React 19, Vite, TailwindCSS, Recharts
- **Communication**: HTTP REST API, JSON

## Documentation

Comprehensive documentation is provided for all aspects of the system:

- **SETUP_GUIDE.md** - Installation and initial configuration
- **NETWORK_MIGRATION_GUIDE.md** - Changing network configuration
- **DATA_BUFFERING_GUIDE.md** - Persistent data buffering implementation
- **ARCHITECTURE.md** - System architecture and design
- **SECURITY_COMPLETE.md** - Security implementation details
- **IMPLEMENTATION_SUMMARY.md** - Feature overview and implementation status

## Troubleshooting

### ESP32 Not Connecting to Django

- Verify Django is running: `python manage.py runserver 0.0.0.0:8000`
- Check board can ping server: Use mDNS or direct IP
- Verify correct URL in `credentials.h`: Must be IP address (DNS not available on W5500)
- Check firewall allows port 8000

### React Not Loading from Network IP

- Verify Vite is bound to 0.0.0.0: `npm run dev` should show network URL
- Check firewall allows port 5173
- Update `BASE_URL` in `src/config.js` to Django server IP

### Data Not Appearing in Dashboard

- Check Django is running and accessible
- Verify ESP32 connected to network (check serial output)
- Check for HTTP errors in Django logs
- Verify firewall allows traffic between board and server

## License

[Specify appropriate license]

## Support

For questions or issues:
1. Check relevant documentation files
2. Review serial output from ESP32 for error messages
3. Check Django logs for API errors
4. Verify network connectivity between components
