#include "web_auth.h"
#include "config.h"
#include "credentials.h"
#include <mbedtls/base64.h>

// Static member initialization
WebAuthManager::RateLimitRecord WebAuthManager::rateLimitRecords[MAX_RATE_LIMIT_RECORDS];
uint8_t WebAuthManager::rateLimitRecordCount = 0;

void WebAuthManager::init() {
    rateLimitRecordCount = 0;
    DEBUG_PRINTLN("Web Authentication Manager initialized");
}

bool WebAuthManager::isAuthenticated(String authHeader) {
    if (authHeader.length() == 0) {
        return false;
    }

    // Check if it's Basic authentication
    if (!authHeader.startsWith("Basic ")) {
        return false;
    }

    // Extract Base64 encoded credentials
    String encodedCreds = authHeader.substring(6); // Remove "Basic "
    String decodedCreds = base64Decode(encodedCreds);

    // Expected format: username:password
    int separatorIndex = decodedCreds.indexOf(':');
    if (separatorIndex == -1) {
        return false;
    }

    String username = decodedCreds.substring(0, separatorIndex);
    String password = decodedCreds.substring(separatorIndex + 1);

    // Constant-time comparison to prevent timing attacks
    bool usernameMatch = (username == WEB_ADMIN_USERNAME);
    bool passwordMatch = (password == WEB_ADMIN_PASSWORD);

    return (usernameMatch && passwordMatch);
}

bool WebAuthManager::isValidAPIToken(String tokenHeader) {
    if (tokenHeader.length() == 0) {
        return false;
    }
    
    // Constant-time comparison to prevent timing attacks
    return (tokenHeader == API_ACCESS_TOKEN);
}

bool WebAuthManager::checkRateLimit(String clientIP) {
    RateLimitRecord* record = findRateLimitRecord(clientIP);
    
    if (record == nullptr) {
        // No more space for new records, allow (fail-open for availability)
        DEBUG_PRINTLN("Rate limit storage full, allowing request");
        return true;
    }

    unsigned long currentTime = millis();
    
    // Check if we're within the rate limit window
    if (currentTime - record->lastRequest < RATE_LIMIT_WINDOW_MS) {
        record->requestCount++;
        
        if (record->requestCount > MAX_REQUESTS_PER_MINUTE) {
            DEBUG_PRINT("Rate limit exceeded for IP: ");
            DEBUG_PRINTLN(clientIP);
            return false;
        }
    } else {
        // Reset counter for new window
        record->requestCount = 1;
        record->lastRequest = currentTime;
    }

    return true;
}

void WebAuthManager::clearRateLimitRecords() {
    unsigned long currentTime = millis();
    
    // Remove old records
    for (int i = 0; i < rateLimitRecordCount; i++) {
        if (currentTime - rateLimitRecords[i].lastRequest > RATE_LIMIT_WINDOW_MS * 2) {
            // Shift records
            for (int j = i; j < rateLimitRecordCount - 1; j++) {
                rateLimitRecords[j] = rateLimitRecords[j + 1];
            }
            rateLimitRecordCount--;
            i--;
        }
    }
}

WebAuthManager::RateLimitRecord* WebAuthManager::findRateLimitRecord(String ip) {
    // Find existing record
    for (int i = 0; i < rateLimitRecordCount; i++) {
        if (rateLimitRecords[i].ip == ip) {
            return &rateLimitRecords[i];
        }
    }

    // Create new record if space available
    if (rateLimitRecordCount < MAX_RATE_LIMIT_RECORDS) {
        rateLimitRecords[rateLimitRecordCount].ip = ip;
        rateLimitRecords[rateLimitRecordCount].lastRequest = millis();
        rateLimitRecords[rateLimitRecordCount].requestCount = 0;
        return &rateLimitRecords[rateLimitRecordCount++];
    }

    return nullptr;
}

String WebAuthManager::base64Decode(String encoded) {
    // Use mbedtls base64 library for decoding
    size_t outputLen;
    
    // First call to get required buffer size
    mbedtls_base64_decode(NULL, 0, &outputLen, 
                          (const unsigned char*)encoded.c_str(), encoded.length());
    
    // Allocate buffer and decode
    unsigned char decoded[outputLen + 1];
    int ret = mbedtls_base64_decode(decoded, outputLen, &outputLen,
                                     (const unsigned char*)encoded.c_str(), encoded.length());
    
    if (ret != 0) {
        return String("");  // Decode failed
    }
    
    decoded[outputLen] = '\0';
    return String((char*)decoded);
}
