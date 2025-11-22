# Smart Sensors - Comprehensive Logging and Security Monitoring

## Overview

This document describes the complete logging and security monitoring system implemented across all three components (ESP32, Django, Frontend) to enable troubleshooting, forensic analysis, and intrusion detection.

---

## Architecture

```
┌─────────────┐
│    ESP32    │  → Serial Monitor Logs (Real-time)
└──────┬──────┘
       │ POST /api/sensors
       ▼
┌─────────────────────────────────────────┐
│         DJANGO BACKEND                  │
│  ┌───────────────────────────────────┐  │
│  │  Logging Middleware               │  │
│  │  - All requests/responses         │  │
│  │  - Authentication attempts        │  │
│  │  - Security events                │  │
│  │  - ESP32 connections              │  │
│  │  - System errors                  │  │
│  └───────────────────────────────────┘  │
│  ┌───────────────────────────────────┐  │
│  │  Database (SQLite/PostgreSQL)     │  │
│  │  - APIAccessLog                   │  │
│  │  - AuthenticationLog              │  │
│  │  - SecurityEvent                  │  │
│  │  - ESP32ConnectionLog             │  │
│  │  - SystemErrorLog                 │  │
│  │  - FrontendLog                    │  │
│  │  - IPBlacklist                    │  │
│  └───────────────────────────────────┘  │
└─────────────────────────────────────────┘
       ▲
       │ POST /api/log/frontend
┌──────┴──────┐
│  FRONTEND   │  → Browser Console + localStorage
│  (React)    │  → Sends errors to Django
└─────────────┘
```

---

## Logging Types

### 1. ESP32 Serial Logging

**Location**: ESP32 device serial output  
**Access**: Serial Monitor (115200 baud)  
**Real-time**: Yes

**What's Logged**:
- System initialization
- Sensor initialization and readings
- Network connectivity (Ethernet/WiFi/AP)
- Django communication attempts
- Success/failure of data transmission
- HTTP response codes and messages
- Error conditions
- Timing information

**Example Output**:
```
╔════════════════════════════════════════╗
║   SENDING DATA TO DJANGO BACKEND      ║
╚════════════════════════════════════════╝
→ Target URL: http://192.168.1.100:8000/api/sensors
→ Payload size: 458 bytes
→ Timestamp: 1234s

Payload:
{"air_quality":{"pm25":45,...},...}

✓ Django Response Received:
  Status Code: 200
  Response Time: 234 ms
  Response: {"status":"success",...}
✓ Data successfully stored in Django
═══════════════════════════════════════════
```

---

### 2. Django API Access Logs

**Model**: `APIAccessLog`  
**Purpose**: Track all API requests and responses  
**Retention**: Permanent (configure cleanup policy)

**Fields**:
- `timestamp` - When the request occurred
- `ip_address` - Client IP address
- `user_agent` - Client browser/device information
- `method` - HTTP method (GET, POST, etc.)
- `endpoint` - URL path
- `status_code` - HTTP response code
- `response_time_ms` - Processing time
- `request_body_size` - Size of request payload
- `response_body_size` - Size of response payload

**Use Cases**:
- Monitor API usage patterns
- Identify slow endpoints
- Track request volumes
- Detect unusual traffic

**Django Admin**: Available at `/admin/sensors/apiaccesslog/`

---

### 3. Authentication Logs

**Model**: `AuthenticationLog`  
**Purpose**: Track all authentication attempts  
**Retention**: Permanent

**Fields**:
- `timestamp` - When authentication was attempted
- `ip_address` - Source IP
- `user_agent` - Client information
- `auth_type` - Type (web_login, api_token, web_access)
- `username` - Username (if provided)
- `status` - Result (success, failed, blocked)
- `failure_reason` - Why it failed

**Use Cases**:
- Detect brute force attacks
- Track successful logins
- Monitor failed attempts
- Identify compromised credentials

**Django Admin**: Available at `/admin/sensors/authenticationlog/`

---

### 4. Security Events

**Model**: `SecurityEvent`  
**Purpose**: Log security-related incidents  
**Retention**: Permanent

**Event Types**:
- Rate Limit Exceeded
- Invalid API Token
- Malformed Request
- SQL Injection Attempt
- XSS Attempt
- Suspicious Pattern Detected
- Unauthorized Access Attempt
- CSRF Validation Failed

**Severity Levels**:
- **Low**: Minor issues, informational
- **Medium**: Potential threats, investigate
- **High**: Active threats, immediate attention
- **Critical**: System compromise attempts, alert admin

**Fields**:
- `timestamp` - When event occurred
- `ip_address` - Source IP
- `event_type` - Type of security event
- `severity` - Severity level
- `description` - Detailed description
- `request_path` - URL being accessed
- `request_data` - Sanitized request data

**Auto-Actions**:
- 5+ high/critical events in 1 hour → Auto-block IP for 24 hours

**Django Admin**: Available at `/admin/sensors/securityevent/`

---

### 5. ESP32 Connection Logs

**Model**: `ESP32ConnectionLog`  
**Purpose**: Track all ESP32 device data submissions  
**Retention**: Permanent

**Fields**:
- `timestamp` - Connection time
- `device_ip` - ESP32 IP address
- `network_mode` - Connection type (eth, wifi, ap)
- `data_received` - Success/failure
- `sensors_included` - Which sensors sent data (JSON array)
- `payload_size` - Data size in bytes
- `processing_time_ms` - Server processing time
- `errors` - Any errors encountered

**Use Cases**:
- Monitor ESP32 connectivity
- Track data submission frequency
- Identify sensor failures
- Detect ESP32 issues

**Django Admin**: Available at `/admin/sensors/esp32connectionlog/`

---

### 6. System Error Logs

**Model**: `SystemErrorLog`  
**Purpose**: Track all system errors and exceptions  
**Retention**: Permanent

**Fields**:
- `timestamp` - When error occurred
- `level` - Severity (debug, info, warning, error, critical)
- `module` - Python module where error occurred
- `function` - Function name
- `message` - Error message
- `stack_trace` - Full stack trace
- `request_path` - URL being processed
- `user_ip` - IP of request that caused error

**Use Cases**:
- Debug application issues
- Monitor system health
- Track recurring errors
- Identify code bugs

**Django Admin**: Available at `/admin/sensors/systemerrorlog/`

---

### 7. Frontend Logs

**Model**: `FrontendLog`  
**Purpose**: Track frontend errors and events  
**Retention**: Permanent

**Fields**:
- `timestamp` - When logged
- `level` - Severity (info, warning, error)
- `user_ip` - Client IP
- `user_agent` - Browser information
- `component` - React component name
- `message` - Log message
- `error_stack` - JavaScript stack trace
- `url` - Page URL

**Local Storage**:
- Logs also stored in browser localStorage
- Up to 100 most recent entries
- Can be exported as JSON

**Use Cases**:
- Debug frontend issues
- Track user-facing errors
- Monitor API communication issues
- Understand user behavior

**Django Admin**: Available at `/admin/sensors/frontendlog/`

---

### 8. IP Blacklist

**Model**: `IPBlacklist`  
**Purpose**: Block malicious IP addresses  
**Retention**: Until manually removed or expired

**Fields**:
- `ip_address` - Blocked IP (unique)
- `blocked_at` - When blocked
- `reason` - Why blocked
- `blocked_by` - Who blocked (auto/admin username)
- `violations_count` - Number of violations
- `is_active` - Whether block is active
- `expires_at` - When block expires (optional)

**Auto-Blocking Rules**:
- 5+ high/critical security events in 1 hour
- Auto-blocks for 24 hours
- Can be manually extended or removed

**Actions Available**:
- View all blocked IPs
- Activate/deactivate blocks
- Set expiration times
- View violation history

**Django Admin**: Available at `/admin/sensors/ipblacklist/`

---

## Security Monitoring

### Automatic Threat Detection

**1. Rate Limiting**
- Max 100 requests per minute per IP
- Exceeding limit logs security event
- Repeated violations lead to blocking

**2. Suspicious Pattern Detection**
Automatically scans for:
- SQL injection attempts (`union select`, `drop table`, etc.)
- XSS attempts (`<script>`, `javascript:`, etc.)
- Path traversal (`../`, `/etc/passwd`)
- Command injection (`exec(`, `cmd.exe`)

**3. IP Blocking**
- Automatic blocking after threshold violations
- Manual blocking via admin panel
- Temporary or permanent blocks
- Block expiration support

---

## Viewing Logs

### Method 1: Django Admin Panel

1. Access admin: `http://your-server:8000/admin`
2. Login with superuser credentials
3. Navigate to "Sensors" section
4. Select log type to view

**Features**:
- Filtering by date, IP, status, severity
- Searching
- Sorting
- Export capabilities
- Bulk actions

### Method 2: Web Interface

Access: `http://your-server:8000/logs`

**Query Parameters**:
- `type`: api, auth, security, esp32, errors, frontend
- `limit`: Number of records (default: 100)

Example: `http://your-server:8000/logs?type=security&limit=50`

### Method 3: API Endpoint

**Endpoint**: `GET /api/logs`

**Parameters**:
- `type`: Log type
- `limit`: Number of records
- `severity`: Filter by severity (security logs)
- `ip`: Filter by IP address

**Example**:
```bash
curl "http://your-server:8000/api/logs?type=security&severity=high&limit=20"
```

**Response**:
```json
{
  "logs": [...],
  "count": 20,
  "type": "security"
}
```

### Method 4: Database Direct Access

```bash
python manage.py dbshell
```

```sql
-- View recent security events
SELECT * FROM sensors_securityevent 
ORDER BY timestamp DESC LIMIT 20;

-- Count events by IP
SELECT ip_address, COUNT(*) as count 
FROM sensors_securityevent 
GROUP BY ip_address 
ORDER BY count DESC;

-- Find blocked IPs
SELECT * FROM sensors_ipblacklist 
WHERE is_active = 1;
```

---

## Forensic Analysis Examples

### Example 1: Investigating Failed Logins

```sql
-- Find all failed authentication attempts in last 24 hours
SELECT * FROM sensors_authenticationlog
WHERE status = 'failed'
AND timestamp >= datetime('now', '-1 day')
ORDER BY ip_address, timestamp;

-- Group by IP to find brute force attempts
SELECT ip_address, COUNT(*) as attempts
FROM sensors_authenticationlog
WHERE status = 'failed'
AND timestamp >= datetime('now', '-1 day')
GROUP BY ip_address
HAVING attempts > 5
ORDER BY attempts DESC;
```

### Example 2: Tracking Suspicious Activity

```sql
-- Find high-severity security events
SELECT timestamp, ip_address, event_type, description
FROM sensors_securityevent
WHERE severity IN ('high', 'critical')
ORDER BY timestamp DESC
LIMIT 50;

-- Find IPs with multiple security violations
SELECT ip_address, 
       COUNT(*) as violations,
       GROUP_CONCAT(DISTINCT event_type) as event_types
FROM sensors_securityevent
WHERE timestamp >= datetime('now', '-7 days')
GROUP BY ip_address
HAVING violations > 3
ORDER BY violations DESC;
```

### Example 3: ESP32 Connection Issues

```sql
-- Find failed ESP32 connections
SELECT * FROM sensors_esp32connectionlog
WHERE data_received = 0
OR errors IS NOT NULL
ORDER BY timestamp DESC;

-- Track ESP32 connectivity over time
SELECT 
    DATE(timestamp) as date,
    COUNT(*) as total_attempts,
    SUM(CASE WHEN data_received = 1 THEN 1 ELSE 0 END) as successful,
    AVG(processing_time_ms) as avg_time_ms
FROM sensors_esp32connectionlog
GROUP BY DATE(timestamp)
ORDER BY date DESC;
```

### Example 4: Performance Analysis

```sql
-- Find slow API endpoints
SELECT endpoint, 
       COUNT(*) as requests,
       AVG(response_time_ms) as avg_time,
       MAX(response_time_ms) as max_time
FROM sensors_apiaccesslog
WHERE timestamp >= datetime('now', '-1 day')
GROUP BY endpoint
ORDER BY avg_time DESC;

-- Track error rates
SELECT 
    DATE(timestamp) as date,
    COUNT(*) as total_requests,
    SUM(CASE WHEN status_code >= 400 THEN 1 ELSE 0 END) as errors,
    ROUND(SUM(CASE WHEN status_code >= 400 THEN 1 ELSE 0 END) * 100.0 / COUNT(*), 2) as error_rate
FROM sensors_apiaccesslog
GROUP BY DATE(timestamp)
ORDER BY date DESC;
```

---

## Security Incident Response

### Step 1: Detect

**Indicators of Compromise**:
- Multiple failed authentication attempts
- High-severity security events
- Unusual API access patterns
- Suspicious request patterns
- Rate limit violations

**Where to Look**:
1. Django Admin → Security Events (filter by severity: high/critical)
2. Check Authentication Logs for failed attempts
3. Review API Access Logs for unusual patterns

### Step 2: Investigate

**Gather Information**:
```sql
-- Get all activity from suspicious IP
SELECT 'API' as source, timestamp, method || ' ' || endpoint as activity
FROM sensors_apiaccesslog WHERE ip_address = 'X.X.X.X'
UNION ALL
SELECT 'Auth' as source, timestamp, auth_type || ' ' || status as activity
FROM sensors_authenticationlog WHERE ip_address = 'X.X.X.X'
UNION ALL
SELECT 'Security' as source, timestamp, event_type || ': ' || description
FROM sensors_securityevent WHERE ip_address = 'X.X.X.X'
ORDER BY timestamp DESC;
```

### Step 3: Contain

**Block Malicious IP**:
1. Go to Django Admin → IP Blacklist
2. Click "Add IP Blacklist"
3. Enter IP address, reason, and duration
4. Save

**Or via Django shell**:
```python
from sensors.models import IPBlacklist
from django.utils import timezone
from datetime import timedelta

IPBlacklist.objects.create(
    ip_address='X.X.X.X',
    reason='Security incident - SQL injection attempt',
    blocked_by='admin',
    violations_count=10,
    is_active=True,
    expires_at=timezone.now() + timedelta(days=7)
)
```

### Step 4: Analyze

**Questions to Answer**:
- What was the attacker trying to access?
- Were they successful?
- What vulnerabilities were targeted?
- How did they find the system?
- Are there other related IPs?

### Step 5: Remediate

**Actions**:
1. Apply security patches
2. Update credentials if compromised
3. Review and tighten access controls
4. Add additional monitoring
5. Update firewall rules
6. Document the incident

---

## Log Maintenance

### Cleanup Old Logs

Create a Django management command:

```python
# sensors/management/commands/cleanup_logs.py
from django.core.management.base import BaseCommand
from django.utils import timezone
from datetime import timedelta
from sensors.models import APIAccessLog, AuthenticationLog

class Command(BaseCommand):
    help = 'Clean up old log entries'

    def handle(self, *args, **options):
        cutoff = timezone.now() - timedelta(days=90)
        
        # Keep security events and errors forever
        APIAccessLog.objects.filter(timestamp__lt=cutoff).delete()
        # Keep auth logs for 1 year
        one_year_ago = timezone.now() - timedelta(days=365)
        AuthenticationLog.objects.filter(
            timestamp__lt=one_year_ago,
            status='success'
        ).delete()
        
        self.stdout.write(self.style.SUCCESS('Logs cleaned up'))
```

Run: `python manage.py cleanup_logs`

**Recommended Retention**:
- API Access Logs: 90 days
- Authentication Logs (success): 1 year
- Authentication Logs (failed): Permanent
- Security Events: Permanent
- ESP32 Logs: 90 days
- System Errors: Permanent
- Frontend Logs: 30 days

---

## Best Practices

### 1. Regular Review
- Check security events daily
- Review authentication logs weekly
- Analyze API patterns monthly

### 2. Alerting
Set up email alerts for:
- Critical security events
- System errors
- Failed authentication spikes
- ESP32 connection failures

### 3. Backup
- Backup logs before cleanup
- Export security events regularly
- Keep offline copies of critical logs

### 4. Privacy
- Don't log sensitive data (passwords, tokens)
- Sanitize request bodies
- Comply with GDPR/privacy laws
- Allow users to request data deletion

### 5. Performance
- Archive old logs to separate database
- Use indexes on frequently queried fields
- Consider log aggregation tools (ELK, Splunk)

---

## Frontend Log Viewer

Access logs in browser console:

```javascript
// Import logger
import logger from './utils/logger';

// View all logs
console.log(logger.getLogs());

// View only errors
console.log(logger.getLogs('error'));

// Export logs to file
logger.exportLogs();

// Clear logs
logger.clearLogs();
```

---

## Support

For questions about the logging system:
1. Review this documentation
2. Check Django Admin for log entries
3. Use API endpoints to query logs programmatically
4. Examine ESP32 serial output for device-side logs
