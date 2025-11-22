# ✅ Logging System - Final Checklist

## 📋 Pre-Deployment Steps

### 1. Database Migration
```bash
cd smartsensors_django
python manage.py makemigrations
python manage.py migrate
```
**Expected Output:** 7 new tables created (APIAccessLog, AuthenticationLog, etc.)

---

### 2. Create Superuser (if not exists)
```bash
python manage.py createsuperuser
```
Follow prompts to create admin account for accessing logs.

---

### 3. Test Django Server
```bash
python manage.py runserver 0.0.0.0:8000
```

**Test URLs:**
- Admin: http://localhost:8000/admin
- Logs Viewer: http://localhost:8000/logs
- API Logs: http://localhost:8000/api/logs

---

### 4. Test Frontend
```bash
cd ../smartsensors_front
npm install  # If not done
npm run dev
```

**Check:**
- Browser console for logger messages
- localStorage for 'app_logs' entry
- Network tab for API calls being logged

---

### 5. Test ESP32
1. Upload code to ESP32
2. Open Serial Monitor (115200 baud)
3. Check for boxed logging output
4. Verify Django communication logs

---

## 🔍 Post-Deployment Verification

### 1. Check Logs Are Being Created

**Django Admin:**
```
http://your-server:8000/admin
→ Sensors
→ API Access Logs (should have entries after accessing API)
```

**Expected:** See entries for each API request

---

### 2. Test Frontend Logging

**Browser Console:**
```javascript
logger.info('Test', 'Manual test log');
logger.getLogs();  // Should show the test log
```

**Expected:** Console shows log entries, localStorage has 'app_logs'

---

### 3. Test ESP32 Logging

**Serial Monitor:**
```
Should see:
╔════════════════════════════════════════╗
║   SENDING DATA TO DJANGO BACKEND      ║
╚════════════════════════════════════════╝
```

**Expected:** Boxed output with URL, timing, response

---

### 4. Test Security Features

**Rate Limiting Test:**
```bash
# Send 150 requests quickly (should trigger rate limit)
for i in {1..150}; do curl http://localhost:8000/api/data; done
```

**Check:**
- Django Admin → Security Events
- Should see "Rate Limit Exceeded" entries
- After 5 high-severity events, IP should be auto-blocked

---

### 5. Test Log Viewer

**Access:**
```
http://your-server:8000/logs
```

**Test:**
- [ ] Page loads with beautiful UI
- [ ] Filters work (type, severity, IP, limit)
- [ ] Statistics display correctly
- [ ] Auto-refresh works (wait 30 seconds)
- [ ] Export logs downloads JSON file

---

## 🔒 Security Checklist

### Production Settings

**Django settings.py:**
```python
# 1. Change secret key
SECRET_KEY = 'your-new-secret-key-here'  # Generate new one

# 2. Disable debug in production
DEBUG = False

# 3. Set allowed hosts
ALLOWED_HOSTS = ['your-domain.com', 'your-ip-address']

# 4. Configure CORS properly
CORS_ALLOWED_ORIGINS = [
    "http://your-frontend-domain.com",
]
```

---

### IP Blacklist Configuration

**Check auto-blocking settings:**
```python
# sensors/logging_utils.py line ~110
if violations >= 5:  # Adjust threshold if needed
    block_duration = timedelta(hours=24)  # Adjust duration
```

---

### Rate Limiting Configuration

**Check rate limits:**
```python
# sensors/logging_utils.py line ~15
def __init__(self, max_requests=100, window_seconds=60):
```

**Adjust based on your traffic:**
- Low traffic: `max_requests=50`
- Normal traffic: `max_requests=100` (default)
- High traffic: `max_requests=200`

---

## 📊 Monitoring Setup

### Daily Tasks

**Check critical security events:**
```bash
python manage.py shell
```
```python
from sensors.models import SecurityEvent
from django.utils import timezone
from datetime import timedelta

# Today's critical events
SecurityEvent.objects.filter(
    severity='critical',
    timestamp__gte=timezone.now() - timedelta(days=1)
).count()
```

---

### Weekly Tasks

**Review blocked IPs:**
```
Django Admin → IP Blacklist
```
- Review blocked IPs
- Remove blocks if necessary
- Check for patterns

---

### Monthly Tasks

**Cleanup old logs:**
```bash
python manage.py cleanup_logs  # After creating the command
```

**Backup security logs:**
```bash
python manage.py dumpdata sensors.SecurityEvent > security_backup_$(date +%Y%m%d).json
python manage.py dumpdata sensors.IPBlacklist > ipblock_backup_$(date +%Y%m%d).json
```

---

## 🧪 Testing Scenarios

### Scenario 1: Normal Operation
1. ESP32 sends data every 2 seconds
2. Frontend fetches data every 2 seconds
3. Check logs show successful operations
4. Response times are reasonable (<500ms)

✅ **Pass Criteria:** All operations logged, no errors

---

### Scenario 2: ESP32 Network Failure
1. Disconnect ESP32 from network
2. Check Serial Monitor for error logs
3. Check Django logs (no ESP32 connections)
4. Reconnect and verify recovery

✅ **Pass Criteria:** Errors logged, recovery successful

---

### Scenario 3: Backend Failure
1. Stop Django server
2. Check frontend console for error logs
3. Verify errors stored in localStorage
4. Restart Django and check logs received

✅ **Pass Criteria:** Frontend logs errors, sends to backend when recovered

---

### Scenario 4: Security Attack Simulation
1. Try SQL injection: `http://localhost:8000/api/data?id=1' OR '1'='1`
2. Check Security Events for detection
3. Repeat 5 times quickly
4. Verify IP gets auto-blocked

✅ **Pass Criteria:** Attack detected, IP blocked

---

### Scenario 5: Rate Limit Test
1. Send 150 requests in 30 seconds
2. Check Security Events
3. Verify rate limit warnings
4. Wait 60 seconds and try again

✅ **Pass Criteria:** Rate limit enforced, recovers after window

---

## 📝 Documentation Checklist

- [x] LOGGING_AND_SECURITY.md - Complete reference guide
- [x] LOGGING_QUICK_REFERENCE.md - Quick access commands
- [x] LOGGING_IMPLEMENTATION_COMPLETE.md - Implementation summary
- [x] This checklist for deployment

**Location:** `/home/unton/iran_smart_tech/smartsensors/`

---

## 🔧 Troubleshooting

### Issue: Logs not appearing in database

**Solution:**
```bash
# Check migrations
python manage.py showmigrations sensors

# If not migrated
python manage.py migrate sensors

# Check middleware in settings.py
# Should have:
# 'sensors.middleware.SecurityLoggingMiddleware',
# 'sensors.middleware.SuspiciousPatternMiddleware',
```

---

### Issue: Frontend logs not sent to backend

**Solution:**
```javascript
// Check logger import
import logger from '../utils/logger';

// Check network tab for POST to /api/log/frontend
// Check browser console for CORS errors

// Verify CORS settings in Django:
CORS_ALLOW_ALL_ORIGINS = True  # For testing
```

---

### Issue: ESP32 not showing detailed logs

**Solution:**
1. Check Serial Monitor baud rate: 115200
2. Verify django_client.cpp was uploaded to ESP32
3. Look for compilation errors
4. Check USB cable connection

---

### Issue: Log viewer page not loading

**Solution:**
```bash
# Check template exists
ls sensors/templates/logs_viewer.html

# Check URL in urls.py
# Should have: path('logs', views.view_logs, name='view_logs')

# Try direct URL
http://localhost:8000/logs
```

---

### Issue: IP blocking not working

**Solution:**
```python
# Check SecurityLoggingMiddleware in settings.py MIDDLEWARE list
# Should be near the top

# Check database for IPBlacklist entries
python manage.py shell
from sensors.models import IPBlacklist
IPBlacklist.objects.filter(is_active=True)
```

---

## 🎯 Success Indicators

### ✅ System is Working When:

1. **Django Admin**
   - All 7 log models visible
   - New entries appear after actions
   - Filters and search work

2. **Log Viewer Web Interface**
   - Page loads with data
   - Filters update results
   - Statistics show correct numbers
   - Auto-refresh works

3. **ESP32 Serial Monitor**
   - Boxed output visible
   - Shows URL and timing
   - Reports success/failure clearly

4. **Frontend Console**
   - Logger messages visible
   - API calls logged with timing
   - Errors show stack traces

5. **Security Features**
   - Rate limiting triggers on spam
   - SQL injection detected
   - IPs auto-blocked after violations
   - Blocked IPs can't access API

---

## 📊 Performance Benchmarks

### Expected Performance:

| Metric | Expected Value | Action if Exceeds |
|--------|----------------|-------------------|
| API Response Time | < 200ms | Optimize queries |
| ESP32 Send Time | < 500ms | Check network |
| Frontend Load Time | < 2s | Optimize bundle |
| Log Write Time | < 10ms | Check DB indexes |
| Database Size Growth | ~100MB/month | Enable cleanup |

---

## 🚀 Production Deployment Checklist

### Before Going Live:

- [ ] All migrations applied
- [ ] DEBUG = False in settings.py
- [ ] SECRET_KEY changed
- [ ] ALLOWED_HOSTS configured
- [ ] CORS_ALLOWED_ORIGINS set correctly
- [ ] Static files collected (`python manage.py collectstatic`)
- [ ] Superuser created
- [ ] All tests passed (5 scenarios above)
- [ ] Log viewer accessible
- [ ] Documentation reviewed
- [ ] Backup strategy in place
- [ ] Monitoring alerts configured (if applicable)

---

### After Going Live:

- [ ] Monitor logs for first 24 hours
- [ ] Check error rates
- [ ] Verify ESP32 connections stable
- [ ] Review security events daily for first week
- [ ] Test log viewer with real data
- [ ] Verify auto-blocking works
- [ ] Set up log cleanup schedule

---

## 📞 Quick Reference Commands

```bash
# Start Django
python manage.py runserver 0.0.0.0:8000

# Start Frontend
npm run dev

# Check migrations
python manage.py showmigrations

# Create superuser
python manage.py createsuperuser

# Django shell
python manage.py shell

# Database shell
python manage.py dbshell

# Export logs
python manage.py dumpdata sensors.SecurityEvent > backup.json
```

---

## 🎓 Training Checklist

### Team should know how to:

- [ ] Access Django admin and view logs
- [ ] Use log viewer web interface
- [ ] Filter logs by type, severity, IP
- [ ] Export logs to JSON
- [ ] Manually block/unblock IPs
- [ ] Read ESP32 serial output
- [ ] Check frontend logs in browser
- [ ] Investigate security incidents
- [ ] Run forensic queries
- [ ] Perform log cleanup
- [ ] Interpret log messages
- [ ] Respond to security alerts

---

## ✨ Final Verification

Run this command to verify everything:

```bash
# Check all files exist
ls smartsensors_django/sensors/models.py
ls smartsensors_django/sensors/logging_utils.py
ls smartsensors_django/sensors/middleware.py
ls smartsensors_django/sensors/templates/logs_viewer.html
ls smartsensors_front/src/utils/logger.js
ls main/django_client.cpp
ls LOGGING_AND_SECURITY.md
ls LOGGING_QUICK_REFERENCE.md
ls LOGGING_IMPLEMENTATION_COMPLETE.md

echo "✅ All files present!"
```

---

## 🎉 Completion

Once all items in this checklist are complete:

✅ **Your logging system is fully operational and production-ready!**

**You can now:**
- Monitor all system activity
- Detect security threats
- Investigate incidents
- Track performance
- Debug issues
- Analyze usage patterns
- Ensure compliance

**The system will:**
- Automatically log all events
- Detect suspicious patterns
- Block malicious IPs
- Alert on critical issues
- Provide forensic data
- Enable quick troubleshooting

---

**Checklist Version:** 1.0  
**Last Updated:** {{ today }}  
**Status:** Ready for Production ✅
