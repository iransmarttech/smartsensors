# Smart Sensors - Pre-Deployment Checklist

Use this checklist to ensure your system is properly configured before deployment.

---

## 📋 Django Backend Checklist

### Installation
- [ ] Python 3.8+ installed
- [ ] Navigate to `smartsensors_django/` directory
- [ ] Run `pip install -r requirements.txt`
- [ ] Verify django-cors-headers is installed

### Database Setup
- [ ] Run `python manage.py makemigrations`
- [ ] Run `python manage.py migrate`
- [ ] (Optional) Create superuser: `python manage.py createsuperuser`

### Configuration
- [ ] Check `settings.py`:
  - [ ] `CORS_ALLOWED_ORIGINS` includes frontend URL
  - [ ] `ALLOWED_HOSTS` configured correctly
  - [ ] `DEBUG = True` for development (change for production)

### Testing
- [ ] Start server: `python manage.py runserver 0.0.0.0:8000`
- [ ] Access admin panel: http://localhost:8000/admin
- [ ] Test GET endpoint: http://localhost:8000/data
- [ ] Test POST endpoint with curl:
  ```bash
  curl -X POST http://localhost:8000/api/sensors \
    -H "Content-Type: application/json" \
    -d '{"air_quality":{"pm25":50}}'
  ```

---

## 🎨 React Frontend Checklist

### Installation
- [ ] Node.js 18+ installed
- [ ] Navigate to `smartsensors_front/` directory
- [ ] Run `npm install`

### Configuration
- [ ] Update `API_BASE_URL` in these files if Django is not on localhost:
  - [ ] `src/pages/AirQualityDashboard.jsx`
  - [ ] `src/pages/GasSensorsPanel.jsx`
  - [ ] `src/pages/DeviceInfoPanel.jsx`

### Testing
- [ ] Start dev server: `npm run dev`
- [ ] Access frontend: http://localhost:5173
- [ ] Open browser console (F12) and check for errors
- [ ] Verify no CORS errors
- [ ] Check that data fetching works

---

## 🔧 ESP32 Hardware Checklist

### Prerequisites
- [ ] ESP32-S3 board available
- [ ] Arduino IDE or PlatformIO installed
- [ ] All required libraries installed:
  - [ ] HTTPClient
  - [ ] WiFi or Ethernet
  - [ ] Sensor libraries

### Configuration
- [ ] Copy `main/credentials_template.h` to `main/credentials.h`
- [ ] Edit `credentials.h` and configure:
  - [ ] WiFi SSID and password (if using WiFi)
  - [ ] AP credentials
  - [ ] Web admin username and password
  - [ ] API access token
  - [ ] **DJANGO_SERVER_URL** (CRITICAL - set to Django server IP)
  - [ ] Device hostname

### Example `credentials.h`:
```cpp
const char* WIFI_SSID = "YourWiFiName";
const char* WIFI_PASS = "YourWiFiPassword";
const char* WEB_ADMIN_USERNAME = "admin";
const char* WEB_ADMIN_PASSWORD = "YourSecurePassword123!";
const char* API_ACCESS_TOKEN = "your-32-char-token-here";
const char* DJANGO_SERVER_URL = "http://192.168.1.100:8000/api/sensors";
const char* DEVICE_HOSTNAME = "smartsensor";
```

### Hardware Connections
- [ ] ZE40 sensor connected (DAC Pin 4, RX Pin 18, TX Pin 17)
- [ ] ZPHS01B sensor connected (RX Pin 8, TX Pin 7)
- [ ] MR007 sensor connected (Pin 5)
- [ ] ME4-SO2 sensor connected (Pin 9)
- [ ] Ethernet cable connected (if using Ethernet)
- [ ] Power supply adequate for all sensors

### Upload and Testing
- [ ] Upload code to ESP32-S3
- [ ] Open Serial Monitor (115200 baud)
- [ ] Verify initialization messages:
  - [ ] "Air Quality Monitor" appears
  - [ ] "Shared data initialized"
  - [ ] "All sensors initialized successfully"
  - [ ] Network connection established
  - [ ] "Django client initialized"
  - [ ] IP address displayed

### Data Transmission Test
- [ ] Watch Serial Monitor for messages like:
  ```
  Sending data to Django:
  Django response code: 200
  Django response: {"status":"success",...}
  ```
- [ ] If errors appear, check:
  - [ ] DJANGO_SERVER_URL is correct
  - [ ] Django server is running
  - [ ] Network connectivity
  - [ ] Firewall settings

---

## 🔄 End-to-End Testing Checklist

### Step 1: Start All Services
- [ ] Django running on port 8000
- [ ] Frontend running on port 5173
- [ ] ESP32 powered on and connected to network

### Step 2: Verify ESP32 → Django
- [ ] Check ESP32 serial monitor for successful POST requests
- [ ] Check Django terminal for incoming POST requests
- [ ] Check Django admin panel for new database entries
- [ ] Verify data timestamp is recent

### Step 3: Verify Django → Frontend
- [ ] Open frontend in browser
- [ ] Check that data is displayed
- [ ] Verify data updates every 2 seconds
- [ ] Check browser console for errors
- [ ] Verify charts are showing data

### Step 4: Test Data Flow
- [ ] Change a physical sensor reading (cover sensor, blow on it, etc.)
- [ ] Wait 10 seconds (ESP32 send interval)
- [ ] Check Django database for updated value
- [ ] Wait 2 seconds (Frontend fetch interval)
- [ ] Verify updated value appears in frontend

---

## 🐛 Troubleshooting Quick Reference

### ESP32 Issues

**Cannot connect to Django**
- Check DJANGO_SERVER_URL is correct
- Verify Django server is reachable (ping test)
- Check firewall allows port 8000
- Verify network connectivity

**Sensors not reading**
- Check wiring connections
- Verify sensor power supply
- Check serial monitor for initialization errors
- Allow sensors to warm up (3 minutes)

### Django Issues

**CORS errors**
- Install django-cors-headers: `pip install django-cors-headers`
- Verify CORS middleware is in settings.py
- Check CORS_ALLOWED_ORIGINS includes frontend URL

**Database errors**
- Run migrations: `python manage.py migrate`
- Check database file permissions
- Delete db.sqlite3 and recreate if corrupted

**API not receiving data**
- Check POST endpoint URL is `/api/sensors`
- Verify @csrf_exempt decorator is present
- Check request Content-Type is application/json

### Frontend Issues

**Cannot fetch data**
- Verify API_BASE_URL is correct
- Check Django server is running
- Open browser console for error messages
- Verify CORS is configured in Django

**Data not updating**
- Check browser console for fetch errors
- Verify useEffect hook is running
- Clear browser cache and reload

---

## 📊 System Health Indicators

### ESP32
✅ Serial monitor shows "Django response code: 200"  
✅ No timeout errors  
✅ Sensor readings look reasonable  

### Django
✅ POST requests appearing in terminal  
✅ No 500 errors  
✅ Database growing with new records  

### Frontend
✅ No console errors  
✅ Data updating every 2 seconds  
✅ Charts showing trends  
✅ Status indicator shows "در حال به روزرسانی" (green)  

---

## 🚀 Ready for Production?

Before deploying to production:

### Django
- [ ] Set `DEBUG = False`
- [ ] Change `SECRET_KEY`
- [ ] Configure proper `ALLOWED_HOSTS`
- [ ] Use PostgreSQL instead of SQLite
- [ ] Set up Nginx/Apache as reverse proxy
- [ ] Configure HTTPS with SSL certificate
- [ ] Set up proper logging

### Frontend
- [ ] Build for production: `npm run build`
- [ ] Deploy to production server
- [ ] Use environment variables for API URL
- [ ] Configure CDN for static assets
- [ ] Set up HTTPS

### ESP32
- [ ] Use HTTPS if Django uses SSL
- [ ] Implement proper authentication tokens
- [ ] Add OTA update capability
- [ ] Implement watchdog timer
- [ ] Add error recovery mechanisms

---

## 📞 Support

If you encounter issues not covered in this checklist:

1. Check `SETUP_GUIDE.md` for detailed instructions
2. Review `ARCHITECTURE.md` for system understanding
3. Check Serial Monitor (ESP32) and browser console (Frontend)
4. Verify all services are running
5. Test each component individually

---

## ✅ Final Verification

- [ ] All three components (ESP32, Django, Frontend) running
- [ ] Data flowing from ESP32 to Django
- [ ] Data flowing from Django to Frontend
- [ ] No errors in any logs/consoles
- [ ] Real-time updates working
- [ ] Historical charts displaying correctly

**System Status: Ready for Use! 🎉**
