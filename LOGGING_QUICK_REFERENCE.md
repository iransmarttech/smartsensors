# 🔍 راهنمای سریع سیستم لاگینگ

## دسترسی سریع به لاگ‌ها

### 1. وب اینترفیس (پیشنهادی)
```
http://your-server:8000/logs
```
- نمایش گرافیکی و زیبا
- فیلتر بر اساس نوع، شدت، IP
- آمار و نمودار
- دانلود JSON

### 2. پنل ادمین جنگو
```
http://your-server:8000/admin
```
ورود با اکانت superuser

**بخش‌های لاگ:**
- API Access Logs
- Authentication Logs  
- Security Events (⚠️ مهم)
- ESP32 Connection Logs
- System Error Logs
- Frontend Logs
- IP Blacklist (مسدودسازی)

### 3. API Endpoint
```bash
# همه لاگ‌ها
curl http://localhost:8000/api/logs?limit=100

# فقط رویدادهای امنیتی
curl http://localhost:8000/api/logs?type=security&limit=50

# فیلتر بر اساس شدت
curl http://localhost:8000/api/logs?type=security&severity=high

# فیلتر بر اساس IP
curl http://localhost:8000/api/logs?ip=192.168.1.100
```

---

## بررسی سریع امنیت

### چک کردن حملات احتمالی

```bash
# ورود به shell جنگو
python manage.py shell
```

```python
from sensors.models import SecurityEvent, IPBlacklist

# رویدادهای بحرانی امروز
SecurityEvent.objects.filter(
    severity__in=['high', 'critical'],
    timestamp__gte=timezone.now() - timedelta(days=1)
).values('ip_address', 'event_type', 'description')

# IPهای مسدود شده
IPBlacklist.objects.filter(is_active=True).values('ip_address', 'reason', 'violations_count')

# آی‌پی‌هایی با بیشترین تخلف
from django.db.models import Count
SecurityEvent.objects.values('ip_address').annotate(
    count=Count('id')
).order_by('-count')[:10]
```

### مسدود کردن دستی IP

```python
from sensors.models import IPBlacklist
from django.utils import timezone
from datetime import timedelta

IPBlacklist.objects.create(
    ip_address='192.168.1.50',
    reason='Suspicious activity detected',
    blocked_by='admin',
    violations_count=5,
    is_active=True,
    expires_at=timezone.now() + timedelta(days=7)
)
```

---

## مانیتورینگ ESP32

### چک کردن اتصالات ESP32

```python
from sensors.models import ESP32ConnectionLog

# آخرین اتصال‌ها
ESP32ConnectionLog.objects.order_by('-timestamp')[:10]

# اتصالات ناموفق
ESP32ConnectionLog.objects.filter(
    data_received=False
).order_by('-timestamp')[:20]

# میانگین زمان پردازش
from django.db.models import Avg
ESP32ConnectionLog.objects.aggregate(
    avg_time=Avg('processing_time_ms')
)
```

---

## لاگ‌های فرانت‌اند

### مشاهده در Browser Console

```javascript
// در Developer Tools Console
import logger from './utils/logger';

// مشاهده همه لاگ‌ها
logger.getLogs()

// فقط خطاها
logger.getLogs('error')

// پاک کردن
logger.clearLogs()

// خروجی فایل
logger.exportLogs()
```

### لاگ دستی

```javascript
import logger from './utils/logger';

// اطلاعات عمومی
logger.info('ComponentName', 'Something happened', { extra: 'data' });

// هشدار
logger.warn('ComponentName', 'Warning message', { details: '...' });

// خطا (خودکار به بکند ارسال می‌شود)
logger.error('ComponentName', 'Error occurred', error, { context: '...' });

// لاگ API call
logger.logAPICall('/api/endpoint', 'GET', 200, 150);

// لاگ اقدام کاربر
logger.logAction('Button clicked', { button: 'submit' });
```

---

## خواندن لاگ‌های ESP32

### Serial Monitor

```
PlatformIO → Monitor (or press Ctrl+Shift+M)
Baud Rate: 115200
```

**خروجی مثال:**
```
╔════════════════════════════════════════╗
║   SENDING DATA TO DJANGO BACKEND      ║
╚════════════════════════════════════════╝
→ Target URL: http://192.168.1.100:8000/api/sensors
→ Payload size: 458 bytes
→ Timestamp: 1234s

✓ Django Response Received:
  Status Code: 200
  Response Time: 234 ms
```

---

## پاکسازی لاگ‌های قدیمی

```python
# ساخت management command
# sensors/management/commands/cleanup_logs.py

from django.core.management.base import BaseCommand
from django.utils import timezone
from datetime import timedelta
from sensors.models import *

class Command(BaseCommand):
    def handle(self, *args, **options):
        cutoff_90 = timezone.now() - timedelta(days=90)
        cutoff_30 = timezone.now() - timedelta(days=30)
        
        # API logs - 90 days
        deleted = APIAccessLog.objects.filter(
            timestamp__lt=cutoff_90
        ).delete()
        self.stdout.write(f'Deleted {deleted[0]} API logs')
        
        # Frontend logs - 30 days  
        deleted = FrontendLog.objects.filter(
            timestamp__lt=cutoff_30,
            level='info'
        ).delete()
        self.stdout.write(f'Deleted {deleted[0]} frontend logs')
        
        # ESP32 logs - 90 days
        deleted = ESP32ConnectionLog.objects.filter(
            timestamp__lt=cutoff_90
        ).delete()
        self.stdout.write(f'Deleted {deleted[0]} ESP32 logs')
```

**اجرا:**
```bash
python manage.py cleanup_logs
```

---

## Query های مفید

### تحلیل API

```sql
-- بیشترین endpoint های استفاده شده
SELECT endpoint, COUNT(*) as count 
FROM sensors_apiaccesslog 
GROUP BY endpoint 
ORDER BY count DESC 
LIMIT 10;

-- کندترین endpoint ها
SELECT endpoint, AVG(response_time_ms) as avg_time
FROM sensors_apiaccesslog
GROUP BY endpoint
ORDER BY avg_time DESC
LIMIT 10;

-- نرخ خطا در 24 ساعت گذشته
SELECT 
    COUNT(*) as total,
    SUM(CASE WHEN status_code >= 400 THEN 1 ELSE 0 END) as errors,
    ROUND(SUM(CASE WHEN status_code >= 400 THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) as error_rate
FROM sensors_apiaccesslog
WHERE timestamp >= datetime('now', '-1 day');
```

### تحلیل امنیت

```sql
-- رویدادهای امنیتی به تفکیک نوع
SELECT event_type, severity, COUNT(*) as count
FROM sensors_securityevent
GROUP BY event_type, severity
ORDER BY count DESC;

-- IP های مشکوک (بیش از 5 تخلف)
SELECT ip_address, COUNT(*) as violations
FROM sensors_securityevent
WHERE severity IN ('high', 'critical')
GROUP BY ip_address
HAVING violations > 5
ORDER BY violations DESC;

-- تلاش های ورود ناموفق
SELECT ip_address, COUNT(*) as attempts, MAX(timestamp) as last_attempt
FROM sensors_authenticationlog
WHERE status = 'failed'
GROUP BY ip_address
HAVING attempts > 3
ORDER BY attempts DESC;
```

### تحلیل ESP32

```sql
-- نرخ موفقیت اتصالات ESP32
SELECT 
    COUNT(*) as total_connections,
    SUM(CASE WHEN data_received = 1 THEN 1 ELSE 0 END) as successful,
    ROUND(SUM(CASE WHEN data_received = 1 THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) as success_rate
FROM sensors_esp32connectionlog
WHERE timestamp >= datetime('now', '-1 day');

-- سنسورهای فعال
SELECT 
    json_extract(sensors_included, '$') as sensors,
    COUNT(*) as count
FROM sensors_esp32connectionlog
WHERE data_received = 1
GROUP BY sensors_included
LIMIT 10;
```

---

## تنظیمات Rate Limiting

```python
# در sensors/logging_utils.py

class RateLimiter:
    def __init__(self, max_requests=100, window_seconds=60):
        self.max_requests = max_requests      # تعداد درخواست
        self.window_seconds = window_seconds  # بازه زمانی (ثانیه)
```

**تغییر محدودیت:**
- پیش‌فرض: 100 درخواست در 60 ثانیه
- برای سختگیری بیشتر: کاهش `max_requests`
- برای بازه بزرگتر: افزایش `window_seconds`

---

## Auto-blocking Settings

```python
# در sensors/logging_utils.py → check_and_block_ip()

violations = SecurityEvent.objects.filter(
    ip_address=ip,
    severity__in=['high', 'critical'],
    timestamp__gte=one_hour_ago
).count()

if violations >= 5:  # تعداد تخلف برای مسدودسازی
    block_duration = timedelta(hours=24)  # مدت مسدودی
```

**تغییر قوانین:**
- `violations >= 5`: تعداد تخلف
- `timedelta(hours=24)`: مدت مسدودی

---

## Export/Backup Logs

### از طریق Django Shell

```python
import json
from sensors.models import SecurityEvent

# Export رویدادهای امنیتی
logs = SecurityEvent.objects.all().values()
with open('security_logs_backup.json', 'w') as f:
    json.dump(list(logs), f, default=str, indent=2)
```

### از طریق Command Line

```bash
# Export به JSON
python manage.py dumpdata sensors.SecurityEvent > security_logs.json
python manage.py dumpdata sensors.IPBlacklist > ip_blacklist.json

# Restore از backup
python manage.py loaddata security_logs.json
```

---

## Alert Integration (Future)

### مثال: ارسال ایمیل برای رویدادهای بحرانی

```python
# در sensors/logging_utils.py

def log_security_event(...):
    event = SecurityEvent.objects.create(...)
    
    # ارسال alert برای رویدادهای بحرانی
    if severity == 'critical':
        send_alert_email(
            subject=f'Security Alert: {event_type}',
            message=f'Critical security event from {ip_address}',
            recipient='admin@example.com'
        )
```

---

## دستورات مفید Django

```bash
# ساخت superuser
python manage.py createsuperuser

# مشاهده migrations
python manage.py showmigrations

# اجرای migrations
python manage.py migrate

# ورود به shell
python manage.py shell

# ورود به database shell
python manage.py dbshell

# جمع‌آوری static files
python manage.py collectstatic
```

---

## Troubleshooting

### لاگ‌ها ذخیره نمی‌شوند

1. چک کنید middleware فعال باشد:
```python
# settings.py
MIDDLEWARE = [
    # ...
    'sensors.middleware.SecurityLoggingMiddleware',
    'sensors.middleware.SuspiciousPatternMiddleware',
]
```

2. چک کنید migrations اجرا شده باشد:
```bash
python manage.py migrate
```

### فرانت‌اند لاگ نمی‌کند

1. چک کنید logger import شده:
```javascript
import logger from '../utils/logger';
```

2. چک کنید console برای خطاها
3. چک کنید localStorage:
```javascript
localStorage.getItem('app_logs')
```

### ESP32 لاگ نمی‌فرستد

1. چک کنید Serial Monitor باز باشد (115200 baud)
2. چک کنید network connection
3. چک کنید Django URL در `credentials.h`

---

## مستندات کامل

برای اطلاعات بیشتر:
- [LOGGING_AND_SECURITY.md](LOGGING_AND_SECURITY.md) - مستندات کامل
- [PROJECT_DOCUMENTATION.md](main/PROJECT_DOCUMENTATION.md) - معماری پروژه
- [SECURITY_README.md](main/SECURITY_README.md) - امنیت سیستم

---

## نکات امنیتی مهم ⚠️

1. **هیچ‌وقت** پسورد یا token را لاگ نکنید
2. لاگ‌های حساس را رمزنگاری کنید
3. دسترسی به `/admin` را محدود کنید
4. IP blacklist را منظم چک کنید
5. رویدادهای critical را روزانه بررسی کنید
6. backup منظم از لاگ‌های امنیتی بگیرید
7. قوانین rate limiting را متناسب با ترافیک تنظیم کنید
8. لاگ‌های قدیمی را پاک کنید (حافظه)

---

**تاریخ ایجاد:** {{ today }}  
**نسخه:** 1.0  
**وضعیت:** آماده استفاده ✅
