# Security Setup Guide

## 🔐 Overview

This guide walks you through setting up secure credentials for your Smart Sensor System. **This is a critical step before deploying the system.**

---

## ⚠️ IMPORTANT: First-Time Setup

### Step 1: Create Your Credentials File

1. **Copy the template:**
   ```bash
   cp credentials_template.h credentials.h
   ```

2. **Edit `credentials.h`** with your secure passwords:
   ```bash
   nano credentials.h
   # or use your preferred editor
   ```

3. **Never commit `credentials.h` to git** - it's already in `.gitignore`

---

## 🔑 Credential Requirements

### WiFi Credentials
```cpp
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASS "YourStrongPassword123!"
```
- **SSID**: Your WiFi network name
- **Password**: Your WiFi password (minimum 8 characters recommended)

### Access Point (AP) Credentials
```cpp
#define AP_SSID_PREFIX "SmartSensor-"  // Will append MAC address
#define AP_PASS "StrongAPPassword456!"
```
- **SSID Prefix**: Device will append MAC address (e.g., "SmartSensor-A1B2C3")
- **Password**: 
  - ⚠️ **Minimum 8 characters** (WPA2 requirement)
  - ✅ **Recommended: 12+ characters** with mixed case, numbers, symbols
  - ❌ **DO NOT use**: "12345678", "password", birth dates

### Web Interface Authentication
```cpp
#define WEB_ADMIN_USERNAME "admin"
#define WEB_ADMIN_PASSWORD "SecureWebPass789!"
```
- **Username**: Admin login (can be any string, "admin" is default)
- **Password**: 
  - ✅ **Minimum: 12 characters**
  - ✅ **Must contain**: uppercase, lowercase, numbers, special characters
  - ❌ **DO NOT use**: same as WiFi password, common words

### API Access Token
```cpp
#define API_ACCESS_TOKEN "your-secret-api-token-here-min-32-chars"
```
- **Token**: Random string for API authentication
  - ✅ **Minimum: 32 characters**
  - ✅ **Recommended: 64+ characters**
  - ✅ **Use random generator** (see below)

---

## 🛡️ Password Generation

### Generate Strong Passwords (Linux/macOS)

```bash
# Random alphanumeric password (20 characters)
openssl rand -base64 20

# Hexadecimal token (64 characters)
openssl rand -hex 32

# Using /dev/urandom (32 characters)
cat /dev/urandom | tr -dc 'a-zA-Z0-9!@#$%^&*' | fold -w 32 | head -n 1
```

### Online Password Generators
- **LastPass Password Generator**: https://www.lastpass.com/features/password-generator
- **1Password Strong Password Generator**: https://1password.com/password-generator/
- **Bitwarden Password Generator**: Built into the Bitwarden extension

---

## 🔒 Security Best Practices

### DO ✅
- ✅ Use unique passwords for each credential
- ✅ Use a password manager (LastPass, 1Password, Bitwarden)
- ✅ Generate random API tokens (32+ characters)
- ✅ Change default credentials immediately
- ✅ Keep `credentials.h` secure (never share, never commit)
- ✅ Use strong AP password (prevents unauthorized access)
- ✅ Review SECURITY_AUDIT.md for complete security checklist

### DON'T ❌
- ❌ Use default/weak passwords like "12345678"
- ❌ Commit `credentials.h` to git (it's in .gitignore)
- ❌ Share credentials in chat, email, or screenshots
- ❌ Use the same password across multiple systems
- ❌ Store passwords in plain text files (except credentials.h)
- ❌ Use personal information (names, birthdates) in passwords

---

## 🚀 Deployment Checklist

Before deploying your device:

- [ ] Created `credentials.h` from template
- [ ] Set strong WiFi password (8+ characters)
- [ ] Set strong AP password (12+ characters, mixed case + numbers)
- [ ] Set strong web admin password (12+ characters, complex)
- [ ] Generated random API token (32+ characters)
- [ ] Verified `credentials.h` is in `.gitignore`
- [ ] Tested web interface authentication
- [ ] Changed all default passwords
- [ ] Documented credentials securely (password manager)
- [ ] Read SECURITY_AUDIT.md for additional hardening

---

## 🌐 Authentication Mechanisms

### 1. Web Interface (Browser Access)
- **Method**: HTTP Basic Authentication
- **Protected**: Main page (`/`) and all web UI
- **Credentials**: `WEB_ADMIN_USERNAME` + `WEB_ADMIN_PASSWORD`
- **Browser Behavior**: Browser will prompt for username/password
- **Caching**: Browser stores credentials during session

### 2. API Access (Programmatic)
- **Method**: API Token Header
- **Protected**: `/data` endpoint (sensor data JSON)
- **Header**: `X-API-Token: your-secret-api-token-here`
- **Fallback**: Also accepts Basic Auth

### 3. Rate Limiting
- **Limit**: 60 requests per minute per IP
- **Response**: HTTP 403 Forbidden when exceeded
- **Purpose**: Prevent brute force attacks

---

## 🧪 Testing Authentication

### Test Web Interface
1. Open browser: `http://<device-ip>/`
2. Browser should prompt for username/password
3. Enter credentials from `credentials.h`
4. Verify page loads successfully

### Test API Access (curl)
```bash
# Using API token
curl -H "X-API-Token: your-secret-api-token-here" http://<device-ip>/data

# Using Basic Auth
curl -u admin:SecureWebPass789! http://<device-ip>/data
```

### Test Rate Limiting
```bash
# Send 70 requests rapidly (should get 403 after 60)
for i in {1..70}; do 
  curl -H "X-API-Token: your-token" http://<device-ip>/data
  echo "Request $i"
done
```

---

## 📋 Example Secure Configuration

```cpp
// credentials.h - EXAMPLE (DO NOT USE THESE VALUES)

// WiFi Configuration
#define WIFI_SSID "MyHomeNetwork"
#define WIFI_PASS "Xk9$mP2nQ7@vL5wR"  // Strong random password

// Access Point Configuration
#define AP_SSID_PREFIX "SmartSensor-"
#define AP_PASS "7yB#nM3pK@9xV2qL4wE"  // 20 characters, complex

// Web Interface Authentication
#define WEB_ADMIN_USERNAME "admin"
#define WEB_ADMIN_PASSWORD "5hT@8jN#2kM$9pL4vQ6rX"  // 24 characters

// API Access Token (generated via: openssl rand -hex 32)
#define API_ACCESS_TOKEN "a7f3e9c2d1b5f8a6e4c9d7b2f1a8e5c3d9b6f2a7e4c1d8b5f9a3e6c2d7b1f4a8e"
```

---

## 🔍 Verification

After setup, verify security:

```bash
# Check credentials.h is NOT tracked by git
git status | grep credentials.h
# Should return nothing

# Verify .gitignore includes credentials.h
cat .gitignore | grep credentials.h
# Should show: credentials.h

# Check for accidental commits of passwords
git log --all --full-history -- "*credentials.h"
# Should be empty
```

---

## 🆘 Troubleshooting

### "Authentication Required" on every page load
- ✅ **Normal behavior** - this is HTTP Basic Auth working correctly
- Browser will remember credentials during session
- For permanent access, browser may offer to save credentials

### Can't access web interface
1. Verify credentials.h exists and has correct passwords
2. Check Serial Monitor for "Web authentication enabled" message
3. Ensure browser is sending username/password correctly
4. Try different browser (Firefox, Chrome, curl)

### API calls return 401 Unauthorized
1. Check `X-API-Token` header is included
2. Verify token matches exactly (case-sensitive, no spaces)
3. Try Basic Auth as fallback: `-u username:password`

### Rate limit (403 Forbidden)
- Wait 1 minute for rate limit window to reset
- Normal for >60 requests/minute from same IP
- Expected behavior during brute force attempts

---

## 📞 Support

For security issues:
1. Review **SECURITY_AUDIT.md** for comprehensive security analysis
2. Check **PROJECT_DOCUMENTATION.md** for system architecture
3. Enable `DEBUG_ENABLED` in config.h for detailed logs
4. Monitor Serial output for authentication events

---

## 🎯 Next Steps

After completing security setup:
1. ✅ Deploy device with secure credentials
2. ✅ Test all authentication mechanisms
3. ✅ Review SECURITY_AUDIT.md for additional hardening
4. ✅ Consider HTTPS/TLS implementation (Phase 2)
5. ✅ Set up secure remote access (VPN recommended)
6. ✅ Implement audit logging (see SECURITY_AUDIT.md)

---

**Last Updated**: 2024
**Security Level**: Industrial-Grade Basic Authentication
**Next Phase**: HTTPS/TLS Implementation (see SECURITY_AUDIT.md Phase 2)
