#ifndef WEB_AUTH_H
#define WEB_AUTH_H

#include <Arduino.h>

/**
 * Web Authentication Manager
 * Provides HTTP Basic Authentication and API token validation
 * for securing web interface and API endpoints
 */
class WebAuthManager {
public:
    /**
     * Check if request has valid authentication credentials
     * Implements HTTP Basic Authentication (RFC 7617)
     * @param authHeader Authorization header string
     * @return true if authenticated, false otherwise
     */
    static bool isAuthenticated(String authHeader);

    /**
     * Validate API access token from request header
     * @param tokenHeader X-API-Token header string
     * @return true if valid token, false otherwise
     */
    static bool isValidAPIToken(String tokenHeader);

    /**
     * Rate limiting: Check if client IP exceeded request limit
     * @param clientIP IP address to check
     * @return true if allowed, false if rate limited
     */
    static bool checkRateLimit(String clientIP);

    /**
     * Clear rate limit records (call periodically)
     */
    static void clearRateLimitRecords();

    /**
     * Initialize authentication manager
     */
    static void init();

private:
    // Rate limiting storage
    struct RateLimitRecord {
        String ip;
        unsigned long lastRequest;
        uint16_t requestCount;
    };

    static const uint8_t MAX_RATE_LIMIT_RECORDS = 10;
    static const uint16_t MAX_REQUESTS_PER_MINUTE = 60;
    static const unsigned long RATE_LIMIT_WINDOW_MS = 60000; // 1 minute

    static RateLimitRecord rateLimitRecords[MAX_RATE_LIMIT_RECORDS];
    static uint8_t rateLimitRecordCount;

    /**
     * Decode Base64 string (for HTTP Basic Auth)
     */
    static String base64Decode(String encoded);

    /**
     * Find or create rate limit record for IP
     */
    static RateLimitRecord* findRateLimitRecord(String ip);
};

#endif
