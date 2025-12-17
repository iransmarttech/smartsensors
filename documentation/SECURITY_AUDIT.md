# Security Audit and Industrial Hardening Guide

## Executive Summary

This document provides a comprehensive security analysis of the Air Quality Monitoring System and recommendations for hardening it to industrial-grade security standards. The current implementation has several critical security vulnerabilities that must be addressed before deployment in production environments.

## Current Security Posture

### Severity Classification
- CRITICAL: Immediate security risk, must fix before deployment
- HIGH: Significant security concern, fix within 24-48 hours
- MEDIUM: Should be addressed for production use
- LOW: Best practice improvement

---

## CRITICAL VULNERABILITIES

### 1. Hardcoded Credentials in Source Code

**Severity**: CRITICAL
**Location**: `config.cpp`

**Issue**:
```cpp
const char* WIFI_SSID = "Morteza";
const char* WIFI_PASS = "miopmiow";
const char* AP_SSID = "MultiSensor-AP";
const char* AP_PASS = "12341234";
```

**Risk**:
- WiFi password exposed in plain text
- AP password is weak ("12341234")
- Credentials visible in version control (GitHub)
- Anyone with code access can compromise network
- Cannot change credentials without recompiling

**Impact**: CRITICAL - Full network compromise, unauthorized access

**Solution**:

**Step 1: Move to Separate Credentials File**

Create `credentials.h` (add to `.gitignore`):
```cpp
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// WiFi Credentials
const char* WIFI_SSID = "YourSSID";
const char* WIFI_PASS = "YourStrongPassword";

// Access Point Credentials
const char* AP_SSID = "SmartSensor-AP-XXXXX";  // Use device serial/MAC
const char* AP_PASS = "YourStrongAPPassword";  // Minimum 12 characters

// Web Interface Credentials (if implementing authentication)
const char* WEB_USERNAME = "admin";
const char* WEB_PASSWORD = "YourStrongWebPassword";

#endif
```

Create `credentials_template.h` (commit this instead):
```cpp
#ifndef CREDENTIALS_H
#define CREDENTIALS_H

// INSTRUCTIONS: Copy this file to credentials.h and fill in your credentials
// Never commit credentials.h to version control

const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";
const char* AP_SSID = "SmartSensor-AP-XXXXX";
const char* AP_PASS = "MINIMUM_12_CHARACTERS";
const char* WEB_USERNAME = "admin";
const char* WEB_PASSWORD = "CHANGE_THIS_PASSWORD";

#endif
```

Add to `.gitignore`:
```
credentials.h
*.bin
*.elf
```

**Step 2: Use EEPROM/Preferences for Runtime Storage**

Store credentials in encrypted flash storage:
```cpp
#include <Preferences.h>

Preferences preferences;

void saveCredentials() {
    preferences.begin("credentials", false);
    preferences.putString("wifi_ssid", WIFI_SSID);
    preferences.putString("wifi_pass", WIFI_PASS);  // Ideally encrypted
    preferences.putString("ap_ssid", AP_SSID);
    preferences.putString("ap_pass", AP_PASS);
    preferences.end();
}

void loadCredentials() {
    preferences.begin("credentials", true);
    String ssid = preferences.getString("wifi_ssid", "");
    String pass = preferences.getString("wifi_pass", "");
    preferences.end();
}
```

**Step 3: Implement First-Boot Configuration**

Web portal for initial setup (runs only on first boot):
```cpp
bool isFirstBoot() {
    preferences.begin("system", true);
    bool configured = preferences.getBool("configured", false);
    preferences.end();
    return !configured;
}

void startConfigurationPortal() {
    // Start AP mode
    // Serve configuration web page
    // Accept credentials via HTTPS
    // Save to encrypted storage
    // Mark as configured
}
```

---

### 2. No Authentication on Web Interface

**Severity**: CRITICAL
**Location**: `web_server.cpp`

**Issue**:
- Anyone on network can access sensor data
- No login required
- No access control
- No session management

**Risk**:
- Unauthorized monitoring
- Data privacy violation
- Industrial espionage
- Compliance violations (GDPR, HIPAA if applicable)

**Impact**: CRITICAL - Complete unauthorized access to all data

**Solution**:

**Option 1: HTTP Basic Authentication** (Simple)

Add to `web_server.h`:
```cpp
class WebServer {
private:
    bool checkAuth(const String& authHeader);
    void sendAuthRequired(Client& client);
    String base64Decode(const String& input);
};
```

Implement in `web_server.cpp`:
```cpp
bool WebServer::checkAuth(const String& authHeader) {
    if (authHeader.indexOf("Authorization: Basic ") == -1) {
        return false;
    }
    
    // Extract and decode credentials
    int start = authHeader.indexOf("Basic ") + 6;
    int end = authHeader.indexOf("\r\n", start);
    String encoded = authHeader.substring(start, end);
    String decoded = base64Decode(encoded);
    
    // Check against stored credentials
    String expectedAuth = String(WEB_USERNAME) + ":" + String(WEB_PASSWORD);
    return (decoded == expectedAuth);
}

void WebServer::sendAuthRequired(Client& client) {
    client.println("HTTP/1.1 401 Unauthorized");
    client.println("WWW-Authenticate: Basic realm=\"Smart Sensor\"");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<h1>401 Unauthorized</h1>");
    client.println("<p>Authentication required</p>");
}

void WebServer::handleHTTPRequest(Client &client) {
    String request = "";
    // Read request...
    
    // Check authentication
    if (!checkAuth(request)) {
        sendAuthRequired(client);
        return;
    }
    
    // Proceed with normal handling
}
```

**Option 2: Session-Based Authentication** (Recommended for Industrial)

Implement proper session management:
```cpp
struct Session {
    String sessionId;
    unsigned long expiry;
    String username;
};

Session sessions[MAX_SESSIONS];

String generateSessionId() {
    // Generate cryptographically random session ID
    char sessionId[33];
    for (int i = 0; i < 32; i++) {
        sessionId[i] = "0123456789ABCDEF"[random(16)];
    }
    sessionId[32] = '\0';
    return String(sessionId);
}

bool validateSession(const String& sessionId) {
    unsigned long now = millis();
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (sessions[i].sessionId == sessionId && 
            sessions[i].expiry > now) {
            // Refresh session
            sessions[i].expiry = now + SESSION_TIMEOUT;
            return true;
        }
    }
    return false;
}
```

---

### 3. Unencrypted HTTP Communication

**Severity**: CRITICAL
**Location**: All web server communications

**Issue**:
- All data transmitted in plain text
- Credentials sent unencrypted
- No protection against man-in-the-middle attacks
- No integrity verification

**Risk**:
- Password interception
- Data eavesdropping
- Session hijacking
- Credential theft

**Impact**: CRITICAL - All network traffic can be intercepted and read

**Solution**:

**Implement HTTPS with TLS/SSL**:

**Step 1: Generate Self-Signed Certificate**

```cpp
#include <WiFiClientSecure.h>

// Self-signed certificate (for development)
const char* server_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDXTCCAkWgAwIBAgIJAKL0UG+mRKZ7MA0GCSqGSIb3DQEBCwUAMEUxCzAJBgNV
... (certificate content)
-----END CERTIFICATE-----
)EOF";

const char* server_key = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDXXpJn7W7Qq4xP
... (private key content)
-----END PRIVATE KEY-----
)EOF";
```

**Step 2: Create HTTPS Server**

```cpp
#include <WiFiServerSecure.h>

WiFiServerSecure httpsServer(443);

void setup() {
    httpsServer.loadCertificate(server_cert, strlen(server_cert));
    httpsServer.loadPrivateKey(server_key, strlen(server_key));
    httpsServer.begin();
}
```

**Step 3: Certificate Management for Production**

For production, use proper certificates from:
- Let's Encrypt (free, but requires domain name)
- Internal CA (for enterprise deployments)
- Commercial CA (for public-facing systems)

---

## HIGH SEVERITY VULNERABILITIES

### 4. Cross-Origin Resource Sharing (CORS) Misconfiguration

**Severity**: HIGH
**Location**: `web_server.cpp` - JSON API

**Issue**:
```cpp
"Access-Control-Allow-Origin: *\r\n"
```

**Risk**:
- Any website can access your sensor data
- Cross-site scripting attacks
- Data exfiltration from malicious websites

**Solution**:

```cpp
// Option 1: No CORS (most secure for local-only access)
// Remove Access-Control-Allow-Origin header entirely

// Option 2: Specific Origin (if external access needed)
const char HTTP_NO_CACHE_HEADER[] PROGMEM = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: application/json\r\n"
    "Access-Control-Allow-Origin: https://yourdomain.com\r\n"  // Specific domain
    "Access-Control-Allow-Methods: GET\r\n"
    "Access-Control-Allow-Credentials: true\r\n"
    "Cache-Control: no-cache, no-store, must-revalidate\r\n"
    "Connection: close\r\n\r\n";
```

---

### 5. No Input Validation

**Severity**: HIGH
**Location**: Any future forms/API endpoints

**Risk**:
- Buffer overflow attacks
- SQL injection (if database added)
- Command injection
- Denial of service

**Solution**:

```cpp
bool validateInput(const String& input, int maxLength, bool alphanumericOnly) {
    if (input.length() > maxLength) return false;
    if (input.length() == 0) return false;
    
    if (alphanumericOnly) {
        for (unsigned int i = 0; i < input.length(); i++) {
            char c = input.charAt(i);
            if (!isalnum(c) && c != '_' && c != '-') {
                return false;
            }
        }
    }
    
    return true;
}

// Usage
String userInput = getFormValue("parameter");
if (!validateInput(userInput, 64, true)) {
    sendError(client, "Invalid input");
    return;
}
```

---

### 6. No Rate Limiting

**Severity**: HIGH
**Location**: Web server and API endpoints

**Issue**:
- Unlimited requests allowed
- No protection against brute force
- No DoS protection

**Risk**:
- Brute force password attacks
- Denial of service
- Resource exhaustion

**Solution**:

```cpp
struct RateLimitEntry {
    IPAddress ip;
    unsigned long requests[RATE_LIMIT_WINDOW];
    int requestCount;
};

RateLimitEntry rateLimitTable[MAX_TRACKED_IPS];

bool checkRateLimit(IPAddress clientIP) {
    unsigned long now = millis();
    
    // Find or create entry for this IP
    RateLimitEntry* entry = findOrCreateEntry(clientIP);
    
    // Remove old requests outside window
    removeOldRequests(entry, now);
    
    // Check if exceeded limit
    if (entry->requestCount >= MAX_REQUESTS_PER_WINDOW) {
        return false;  // Rate limit exceeded
    }
    
    // Add current request
    entry->requests[entry->requestCount++] = now;
    return true;
}

void WebServer::handleEthernetClient() {
    EthernetClient client = ethServer->available();
    
    if (client) {
        // Check rate limit
        if (!checkRateLimit(client.remoteIP())) {
            client.println("HTTP/1.1 429 Too Many Requests");
            client.println("Retry-After: 60");
            client.println();
            client.stop();
            return;
        }
        
        // Process request
    }
}
```

---

## MEDIUM SEVERITY VULNERABILITIES

### 7. Information Disclosure

**Severity**: MEDIUM
**Location**: Serial debug output, HTTP headers

**Issue**:
- Detailed error messages
- System information in responses
- Debug output enabled in production

**Risk**:
- Reveals system architecture
- Helps attackers plan attacks
- Leaks internal information

**Solution**:

```cpp
// config.h - Conditional debug output
#ifdef PRODUCTION_BUILD
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINTF(...)
#else
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#endif

// Generic error responses
void sendGenericError(Client& client) {
    client.println("HTTP/1.1 500 Internal Server Error");
    client.println("Content-Type: text/plain");
    client.println();
    client.println("An error occurred");  // Don't reveal details
}

// Remove server identification
// Don't add: Server: ESP32-S3/1.0
```

---

### 8. No Firmware Update Authentication

**Severity**: MEDIUM
**Location**: OTA update mechanism (if implemented)

**Risk**:
- Malicious firmware installation
- Device compromise
- Backdoor insertion

**Solution**:

```cpp
#include <Update.h>
#include <mbedtls/sha256.h>

bool verifyFirmware(const uint8_t* firmware, size_t len, const uint8_t* signature) {
    uint8_t hash[32];
    mbedtls_sha256_context ctx;
    
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
    mbedtls_sha256_update(&ctx, firmware, len);
    mbedtls_sha256_finish(&ctx, hash);
    mbedtls_sha256_free(&ctx);
    
    // Verify signature with public key
    return verifySignature(hash, signature, public_key);
}

void updateFirmware(const uint8_t* data, size_t len, const uint8_t* sig) {
    if (!verifyFirmware(data, len, sig)) {
        Serial.println("Firmware verification failed!");
        return;
    }
    
    if (Update.begin(len)) {
        Update.write(data, len);
        Update.end();
    }
}
```

---

### 9. Weak Access Point Password

**Severity**: MEDIUM
**Location**: `config.cpp`

**Issue**:
```cpp
const char* AP_PASS = "12341234";  // 8 digits, no complexity
```

**Risk**:
- Easy to brute force
- Dictionary attack vulnerable
- WPA2 handshake capture and offline cracking

**Solution**:

```cpp
// Minimum requirements for AP password:
// - 12+ characters
// - Mix of upper/lowercase
// - Numbers and symbols
// - Not in common password lists

const char* AP_PASS = "SmartSensor!2024@Secure";

// Better: Generate unique password per device
String generateAPPassword() {
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char password[20];
    sprintf(password, "SS-%02X%02X%02X-%04X", 
            mac[3], mac[4], mac[5], random(0x10000));
    return String(password);
}
```

---

### 10. No Watchdog Timer for Security

**Severity**: MEDIUM

**Issue**:
- No protection against infinite loops
- No recovery from attacks
- No automatic reboot on freeze

**Solution**:

```cpp
#include "esp_task_wdt.h"

void setup() {
    // Enable watchdog timer (10 seconds)
    esp_task_wdt_init(10, true);
    esp_task_wdt_add(NULL);
}

void loop() {
    // Reset watchdog periodically
    esp_task_wdt_reset();
    
    // Your code
}
```

---

## LOW SEVERITY / BEST PRACTICES

### 11. No Security Headers

**Severity**: LOW

**Solution**:

```cpp
const char SECURITY_HEADERS[] PROGMEM =
    "X-Content-Type-Options: nosniff\r\n"
    "X-Frame-Options: DENY\r\n"
    "X-XSS-Protection: 1; mode=block\r\n"
    "Referrer-Policy: no-referrer\r\n"
    "Content-Security-Policy: default-src 'self'\r\n";
```

---

### 12. No Audit Logging

**Severity**: LOW

**Solution**:

```cpp
void logSecurityEvent(const char* event, IPAddress ip) {
    unsigned long timestamp = millis();
    Serial.printf("[SECURITY] %lu - %s from %d.%d.%d.%d\n",
                  timestamp, event, ip[0], ip[1], ip[2], ip[3]);
    
    // Optional: Write to SD card or flash
}

// Usage
logSecurityEvent("Failed login attempt", client.remoteIP());
logSecurityEvent("Successful authentication", client.remoteIP());
logSecurityEvent("Rate limit exceeded", client.remoteIP());
```

---

### 13. No Secure Boot

**Severity**: LOW (for industrial use)

**Solution**:

Enable ESP32 Secure Boot in menuconfig:
```
Security features -> Enable hardware Secure Boot
```

This prevents unauthorized firmware from running.

---

## INDUSTRIAL-GRADE REQUIREMENTS

### Physical Security

**1. Tamper Detection**
```cpp
#define TAMPER_PIN 39

void setup() {
    pinMode(TAMPER_PIN, INPUT_PULLUP);
    attachInterrupt(TAMPER_PIN, tamperDetected, FALLING);
}

void tamperDetected() {
    // Erase sensitive data
    preferences.clear();
    
    // Log event
    logSecurityEvent("TAMPER DETECTED", IPAddress(0,0,0,0));
    
    // Shutdown or alert
    esp_deep_sleep_start();
}
```

**2. Secure Element Integration**

Use ATECC608A chip for:
- Secure key storage
- Hardware encryption
- Certificate storage

---

### Network Security

**1. Network Segmentation**
- Deploy on separate VLAN
- Firewall rules to restrict access
- No direct internet access

**2. VPN for Remote Access**
- Use WireGuard or IPSec
- Never expose directly to internet

**3. MAC Address Filtering**
```cpp
const uint8_t allowed_macs[][6] = {
    {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF},
    {0x11, 0x22, 0x33, 0x44, 0x55, 0x66}
};

bool isAllowedMAC(uint8_t* mac) {
    for (int i = 0; i < sizeof(allowed_macs) / 6; i++) {
        if (memcmp(mac, allowed_macs[i], 6) == 0) {
            return true;
        }
    }
    return false;
}
```

---

### Data Security

**1. Encrypt Stored Credentials**
```cpp
#include <mbedtls/aes.h>

void encryptData(const char* plaintext, char* ciphertext, const char* key) {
    mbedtls_aes_context aes;
    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, 256);
    mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT,
                          (const unsigned char*)plaintext,
                          (unsigned char*)ciphertext);
    mbedtls_aes_free(&aes);
}
```

**2. Secure Data Transmission**
- Use TLS 1.2 or higher
- Verify certificates
- Pin certificates for critical connections

---

### Compliance Requirements

**1. GDPR Compliance** (if in EU)
- Data minimization
- Right to erasure
- Data encryption
- Audit logs

**2. IEC 62443** (Industrial Cybersecurity)
- Security levels SL 1-4
- Defense in depth
- Security lifecycle

**3. NIST Cybersecurity Framework**
- Identify
- Protect
- Detect
- Respond
- Recover

---

## IMPLEMENTATION PRIORITY

### Phase 1: Immediate (Before ANY Deployment)
1. Remove hardcoded credentials from code
2. Implement authentication on web interface
3. Change default passwords
4. Disable debug output in production

### Phase 2: Before Production (1 week)
1. Implement HTTPS/TLS
2. Add rate limiting
3. Input validation
4. CORS restriction
5. Implement audit logging

### Phase 3: Industrial Hardening (1 month)
1. Secure boot
2. Firmware signing
3. Encrypted storage
4. Tamper detection
5. Network segmentation

### Phase 4: Compliance (Ongoing)
1. Security testing
2. Penetration testing
3. Vulnerability scanning
4. Regular updates
5. Security training

---

## SECURITY CHECKLIST

### Pre-Deployment
- [ ] All credentials moved to separate file (not in git)
- [ ] Strong passwords (12+ characters, complex)
- [ ] Authentication enabled on web interface
- [ ] HTTPS/TLS implemented
- [ ] Debug output disabled
- [ ] Default credentials changed
- [ ] Rate limiting enabled
- [ ] Input validation implemented
- [ ] Security headers added
- [ ] Audit logging enabled

### Network
- [ ] Deployed on separate VLAN
- [ ] Firewall rules configured
- [ ] No direct internet exposure
- [ ] VPN configured for remote access
- [ ] mDNS disabled or restricted
- [ ] CORS properly configured

### Physical
- [ ] Device in secure enclosure
- [ ] Tamper detection enabled
- [ ] Serial port access restricted
- [ ] USB port disabled or protected

### Operational
- [ ] Regular security updates
- [ ] Vulnerability monitoring
- [ ] Incident response plan
- [ ] Backup and recovery procedures
- [ ] Security audit schedule

---

## TESTING AND VALIDATION

### Security Testing Tools

**1. Network Scanning**
```bash
nmap -sV -sC 192.168.1.3
```

**2. SSL/TLS Testing**
```bash
openssl s_client -connect 192.168.1.3:443 -tls1_2
```

**3. Brute Force Testing**
```bash
hydra -l admin -P passwords.txt http-get://192.168.1.3/
```

**4. Vulnerability Scanning**
- OWASP ZAP
- Burp Suite
- Metasploit

---

## INCIDENT RESPONSE PLAN

### Detection
1. Monitor audit logs
2. Watch for failed authentication attempts
3. Track unusual network traffic
4. Monitor system resource usage

### Response
1. Isolate affected device
2. Preserve evidence (logs)
3. Analyze attack vector
4. Patch vulnerability
5. Reset credentials
6. Update all devices

### Recovery
1. Restore from backup
2. Verify integrity
3. Update security measures
4. Document lessons learned

---

## MAINTENANCE AND UPDATES

### Regular Tasks (Weekly)
- Review audit logs
- Check for failed login attempts
- Monitor system health

### Monthly Tasks
- Update firmware (after testing)
- Review security policies
- Test backup/restore
- Review access logs

### Quarterly Tasks
- Security audit
- Penetration testing
- Policy review
- Training refresh

### Annually
- Full security assessment
- Compliance audit
- Disaster recovery drill
- Security roadmap update

---

## CONCLUSION

The current implementation lacks critical security controls required for industrial deployment. Before production use:

1. **MUST FIX** (CRITICAL):
   - Remove hardcoded credentials
   - Implement authentication
   - Enable HTTPS
   - Fix CORS policy

2. **SHOULD FIX** (HIGH):
   - Add rate limiting
   - Implement input validation
   - Add security headers
   - Enable audit logging

3. **RECOMMENDED** (MEDIUM/LOW):
   - Secure boot
   - Firmware signing
   - Tamper detection
   - Network segmentation

Following this guide will transform the system from a development prototype to an industrial-grade, secure monitoring solution suitable for production deployment.

## References

- OWASP IoT Top 10
- NIST Cybersecurity Framework
- IEC 62443 Industrial Security
- ESP32 Security Features Guide
- PCI DSS Requirements
- GDPR Technical Requirements
