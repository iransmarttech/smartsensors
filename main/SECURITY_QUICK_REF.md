# Security Quick Reference

## 🚀 Quick Start (New Developers)

```bash
# 1. Clone repository
git clone <repo-url>
cd smartsensors/main

# 2. Create credentials file
cp credentials_template.h credentials.h

# 3. Edit with strong passwords
nano credentials.h

# 4. Compile & upload
# (credentials.h will NOT be committed to git)
```

---

## 🔑 Credentials Overview

| Credential | Purpose | Min Length | Example |
|-----------|---------|------------|---------|
| `WIFI_SSID` | WiFi network name | Any | "MyNetwork" |
| `WIFI_PASS` | WiFi password | 8 chars | "Str0ng!Pass" |
| `AP_SSID_PREFIX` | AP name prefix | Any | "SmartSensor-" |
| `AP_PASS` | AP password | 12 chars | "Secure@123AP" |
| `WEB_ADMIN_USERNAME` | Web login user | Any | "admin" |
| `WEB_ADMIN_PASSWORD` | Web login pass | 12 chars | "W3b@dmin!234" |
| `API_ACCESS_TOKEN` | API token | 32 chars | "a7f3e9c2d1b5..." |

---

## 🛡️ Password Requirements

### WiFi Password
- ✅ Minimum: 8 characters
- ✅ Use your actual WiFi password

### AP Password
- ✅ Minimum: 8 characters (WPA2 requirement)
- ⭐ Recommended: 12+ characters
- ⭐ Must contain: uppercase, lowercase, numbers
- ❌ Don't use: "12345678", "password"

### Web Admin Password
- ✅ Minimum: 12 characters
- ✅ Must contain: uppercase, lowercase, numbers, symbols
- ⭐ Recommended: 16+ characters
- ❌ Don't use: same as WiFi/AP password

### API Token
- ✅ Minimum: 32 characters
- ⭐ Recommended: 64 characters
- ✅ Generate randomly (see below)

---

## 🎲 Generate Strong Passwords

```bash
# Random password (20 characters)
openssl rand -base64 20

# API token (64 characters)
openssl rand -hex 32

# Alphanumeric + symbols (32 characters)
cat /dev/urandom | tr -dc 'a-zA-Z0-9!@#$%^&*' | fold -w 32 | head -n 1
```

---

## 🌐 Testing Authentication

### Web Browser
```bash
# Open in browser
http://<device-ip>/

# Should prompt for:
# Username: admin (or your custom username)
# Password: (your web password)
```

### API Test (curl)
```bash
# Using API token
curl -H "X-API-Token: your-token-here" http://<device-ip>/data

# Using Basic Auth
curl -u admin:password http://<device-ip>/data

# Should return JSON sensor data
```

### Rate Limit Test
```bash
# Send 70 requests (should block after 60)
for i in {1..70}; do
  curl -H "X-API-Token: your-token" http://<device-ip>/data
  echo "Request $i"
done
```

---

## 🚨 Common Issues

### Issue: "Authentication Required" on every page
- ✅ **Normal behavior** - HTTP Basic Auth working correctly
- Browser will remember credentials during session

### Issue: Can't access web interface
1. Check credentials.h exists
2. Verify passwords match exactly
3. Try different browser
4. Check Serial Monitor for errors

### Issue: API returns 401 Unauthorized
1. Check token matches exactly (case-sensitive)
2. Ensure no extra spaces in token
3. Try Basic Auth as fallback

### Issue: 403 Forbidden (Rate Limited)
- Wait 1 minute for rate limit reset
- Reduce request frequency to <60/minute

---

## 🔍 Security Checklist

Before deployment:
- [ ] Created `credentials.h` from template
- [ ] Set strong WiFi password
- [ ] Set strong AP password (12+ chars)
- [ ] Set strong web password (12+ chars complex)
- [ ] Generated random API token (32+ chars)
- [ ] Verified `credentials.h` in `.gitignore`
- [ ] Tested web authentication
- [ ] Tested API authentication
- [ ] Changed ALL default passwords
- [ ] Stored credentials in password manager

---

## 📋 Authentication Flow

### Web Interface (`/` or `/index`)
1. Browser requests page
2. Server checks for `Authorization` header
3. If missing → HTTP 401 + WWW-Authenticate
4. Browser prompts for username/password
5. Browser resends with `Authorization: Basic <base64>`
6. Server validates credentials
7. If valid → Send page
8. If invalid → HTTP 401 again

### API Endpoint (`/data`)
1. Client sends request with header
2. Server checks two options:
   - `X-API-Token: <token>` (preferred)
   - OR `Authorization: Basic <base64>` (fallback)
3. If valid → Return JSON data
4. If invalid → HTTP 401

### Rate Limiting (All Requests)
1. Server extracts client IP
2. Checks request count in last 60 seconds
3. If <60 requests → Allow
4. If ≥60 requests → HTTP 403 Forbidden
5. Counter resets after 60 seconds

---

## 🔐 Security Features

### Implemented ✅
- ✅ HTTP Basic Authentication
- ✅ API Token authentication
- ✅ Rate limiting (60 req/min per IP)
- ✅ Secure credential storage
- ✅ Security headers (X-Frame-Options, X-Content-Type-Options)
- ✅ CORS restriction (localhost only)
- ✅ Unique AP SSID (MAC-based)
- ✅ Constant-time password comparison

### Not Yet Implemented ⏳
- ⏳ HTTPS/TLS encryption (Phase 2)
- ⏳ Input validation (Phase 2)
- ⏳ Audit logging (Phase 3)
- ⏳ Multi-user support (Future)

---

## 📞 Need Help?

1. **Setup Issues**: Read `SECURITY_SETUP.md`
2. **Security Details**: Read `SECURITY_AUDIT.md`
3. **Implementation**: Read `SECURITY_IMPLEMENTATION.md`
4. **System Architecture**: Read `PROJECT_DOCUMENTATION.md`

---

## 🎯 Quick Commands

```bash
# Check if credentials.h is ignored by git
git status | grep credentials.h  # Should be empty

# Verify .gitignore
cat .gitignore | grep credentials.h  # Should show: credentials.h

# Check for accidental commits
git log --all -- "*credentials.h"  # Should be empty

# Generate new API token
openssl rand -hex 32

# Test authentication
curl -v -u admin:password http://<device-ip>/
```

---

**Last Updated**: 2024  
**For Full Details**: See `SECURITY_SETUP.md` and `SECURITY_AUDIT.md`
