/**
 * Frontend Configuration
 * ======================
 * 
 * Central configuration for the React frontend application.
 * This file contains all environment-specific settings.
 * 
 * IMPORTANT: Update these settings based on your deployment environment.
 */

// ============================================================================
// API CONFIGURATION
// ============================================================================

/**
 * Django Backend URL
 * 
 * This is the URL where your Django backend is running.
 * Update this to match your server's IP address or domain.
 */
export const API_CONFIG = {
  // Base URL for Django backend
  // When accessing React from 192.168.1.4:5173, use same IP for API
  // BASE_URL: 'http://192.168.1.4:8000',  // Your laptop's Django server
  BASE_URL: 'http://192.168.4.2:8000',  // Your laptop's wifi IP

  // Alternative configurations (uncomment as needed):
  // BASE_URL: 'http://localhost:8000',              // Local development
  // BASE_URL: 'http://your-domain.com',             // Production domain
  // BASE_URL: 'https://your-domain.com',            // Production with SSL
  
  // API Endpoints
  ENDPOINTS: {
    SENSOR_DATA: '/data',              // Get all sensor data
    SEND_SENSOR_DATA: '/api/sensors',      // ESP32 sends data here
    FRONTEND_LOG: '/api/log/frontend',     // Send frontend logs
    LOGS: '/api/logs',                     // Get logs
    LOGS_VIEWER: '/logs',                  // Web log viewer
  },
  
  // Request timeout (milliseconds)
  TIMEOUT: 10000,  // 10 seconds
  
  // Retry configuration
  MAX_RETRIES: 3,
  RETRY_DELAY: 1000,  // 1 second between retries
};


// ============================================================================
// POLLING CONFIGURATION
// ============================================================================

/**
 * Data polling intervals
 * How often to fetch new data from the backend (milliseconds)
 */
export const POLLING_CONFIG = {
  // Main dashboard data refresh
  SENSOR_DATA_INTERVAL: 2000,  // 2 seconds
  
  // Device info refresh
  DEVICE_INFO_INTERVAL: 2000,  // 2 seconds
  
  // Gas sensors refresh
  GAS_SENSORS_INTERVAL: 2000,  // 2 seconds
  
  // Minimum interval (safety limit)
  MIN_INTERVAL: 1000,  // Don't poll faster than 1 second
};


// ============================================================================
// LOGGING CONFIGURATION
// ============================================================================

/**
 * Frontend logging settings
 */
export const LOGGING_CONFIG = {
  // Enable/disable logging
  ENABLED: true,
  
  // Log levels to capture
  LEVELS: {
    INFO: true,
    WARN: true,
    ERROR: true,
  },
  
  // Send logs to backend
  SEND_TO_BACKEND: true,
  
  // Only send errors to backend (saves bandwidth)
  SEND_ONLY_ERRORS: true,
  
  // Maximum logs to store in localStorage
  MAX_LOCAL_LOGS: 100,
  
  // Auto-clear logs after (milliseconds)
  AUTO_CLEAR_AFTER: 24 * 60 * 60 * 1000,  // 24 hours
  
  // Log API calls
  LOG_API_CALLS: true,
  
  // Log component lifecycle
  LOG_COMPONENT_LIFECYCLE: true,
};


// ============================================================================
// DISPLAY CONFIGURATION
// ============================================================================

/**
 * UI display settings
 */
export const DISPLAY_CONFIG = {
  // Date/time format
  DATE_FORMAT: 'fa-IR',  // Persian locale
  
  // Number format
  NUMBER_FORMAT: 'fa-IR',
  
  // Decimal places for sensor readings
  DECIMAL_PLACES: {
    temperature: 1,
    humidity: 1,
    pm25: 1,
    pm10: 1,
    voltage: 2,
    concentration: 2,
  },
  
  // Chart configuration
  CHART: {
    MAX_DATA_POINTS: 20,  // Maximum points to show on chart
    UPDATE_INTERVAL: 2000,  // Chart update interval (ms)
    ANIMATION_DURATION: 300,  // Animation duration (ms)
  },
  
  // Toast notifications
  TOAST: {
    DURATION: 3000,  // 3 seconds
    POSITION: 'top-right',
  },
};


// ============================================================================
// SENSOR THRESHOLDS
// ============================================================================

/**
 * Warning thresholds for sensor readings
 * Used to display alerts when values exceed limits
 */
export const SENSOR_THRESHOLDS = {
  pm25: {
    good: 12,
    moderate: 35,
    unhealthy: 55,
    veryUnhealthy: 150,
    hazardous: 250,
  },
  pm10: {
    good: 54,
    moderate: 154,
    unhealthy: 254,
    veryUnhealthy: 354,
    hazardous: 424,
  },
  so2: {
    safe: 0.5,
    warning: 2.0,
    danger: 5.0,
  },
  tvoc: {
    safe: 220,
    warning: 660,
    danger: 2200,
  },
  lel: {
    safe: 10,
    warning: 20,
    danger: 40,
  },
};


// ============================================================================
// DEFAULT VALUES
// ============================================================================

/**
 * Default sensor values (shown when no data available)
 */
export const DEFAULT_VALUES = {
  airQuality: {
    pm25: 0,
    pm10: 0,
  },
  zphs01b: {
    pm25: 0,
    pm10: 0,
  },
  mr007: {
    voltage: 0,
    rawValue: 0,
    lel_concentration: 0,
  },
  me4_so2: {
    voltage: 0,
    rawValue: 0,
    current_ua: 0,
    so2_concentration: 0,
  },
  ze40: {
    tvoc_ppb: 0,
    tvoc_ppm: 0,
    dac_voltage: 0,
    dac_ppm: 0,
    uart_data_valid: false,
    analog_data_valid: false,
  },
  device: {
    ip_address: 'نامشخص',
    network_mode: 'نامشخص',
    timestamp: null,
  },
};


// ============================================================================
// ERROR HANDLING
// ============================================================================

/**
 * Error handling configuration
 */
export const ERROR_CONFIG = {
  // Show error notifications
  SHOW_NOTIFICATIONS: true,
  
  // Retry failed requests
  AUTO_RETRY: true,
  
  // Fallback to cached data
  USE_CACHED_DATA: true,
  
  // Cache expiration time (milliseconds)
  CACHE_EXPIRATION: 30000,  // 30 seconds
  
  // Error messages (Persian)
  MESSAGES: {
    NETWORK_ERROR: 'خطا در اتصال به سرور',
    TIMEOUT_ERROR: 'زمان انتظار تمام شد',
    SERVER_ERROR: 'خطای سرور',
    NO_DATA: 'داده‌ای دریافت نشد',
  },
};


// ============================================================================
// DEVELOPMENT CONFIGURATION
// ============================================================================

/**
 * Development-only settings
 * These should be disabled in production
 */
export const DEV_CONFIG = {
  // Enable debug mode
  DEBUG: true,  // Set to false in production
  
  // Show console logs
  CONSOLE_LOGS: true,
  
  // Use mock data (for testing without backend)
  USE_MOCK_DATA: false,
  
  // Mock data (used when USE_MOCK_DATA is true)
  MOCK_DATA: {
    airQuality: { pm25: 45, pm10: 78 },
    zphs01b: { pm25: 42, pm10: 75 },
    mr007: { voltage: 3.3, rawValue: 2450, lel_concentration: 15.2 },
    me4_so2: { voltage: 3.3, rawValue: 1800, current_ua: 45.6, so2_concentration: 2.1 },
    ze40: { tvoc_ppb: 350, tvoc_ppm: 0.35, dac_voltage: 1.8, dac_ppm: 0.42, uart_data_valid: true, analog_data_valid: true },
    ip_address: '192.168.1.150',
    network_mode: 'Ethernet',
  },
};


// ============================================================================
// PERFORMANCE CONFIGURATION
// ============================================================================

/**
 * Performance optimization settings
 */
export const PERFORMANCE_CONFIG = {
  // Enable service worker (for PWA)
  ENABLE_SERVICE_WORKER: false,
  
  // Lazy load components
  LAZY_LOAD: true,
  
  // Debounce user input (milliseconds)
  INPUT_DEBOUNCE: 300,
  
  // Throttle scroll events (milliseconds)
  SCROLL_THROTTLE: 100,
};


// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

/**
 * Get full API URL for an endpoint
 * @param {string} endpoint - Endpoint path
 * @returns {string} Full URL
 */
export const getApiUrl = (endpoint = '') => {
  return `${API_CONFIG.BASE_URL}${endpoint}`;
};

/**
 * Get sensor data endpoint URL
 * @returns {string} Full URL
 */
export const getSensorDataUrl = () => {
  return getApiUrl(API_CONFIG.ENDPOINTS.SENSOR_DATA);
};

/**
 * Get frontend log endpoint URL
 * @returns {string} Full URL
 */
export const getLogUrl = () => {
  return getApiUrl(API_CONFIG.ENDPOINTS.FRONTEND_LOG);
};

/**
 * Check if running in development mode
 * @returns {boolean}
 */
export const isDevelopment = () => {
  return DEV_CONFIG.DEBUG || import.meta.env.DEV;
};

/**
 * Check if running in production mode
 * @returns {boolean}
 */
export const isProduction = () => {
  return !isDevelopment();
};

/**
 * Get polling interval for a data type
 * @param {string} type - Data type (sensor_data, device_info, gas_sensors)
 * @returns {number} Interval in milliseconds
 */
export const getPollingInterval = (type) => {
  const intervals = {
    'sensor_data': POLLING_CONFIG.SENSOR_DATA_INTERVAL,
    'device_info': POLLING_CONFIG.DEVICE_INFO_INTERVAL,
    'gas_sensors': POLLING_CONFIG.GAS_SENSORS_INTERVAL,
  };
  
  const interval = intervals[type] || POLLING_CONFIG.SENSOR_DATA_INTERVAL;
  return Math.max(interval, POLLING_CONFIG.MIN_INTERVAL);
};

/**
 * Get air quality status based on PM2.5 value
 * @param {number} pm25 - PM2.5 value
 * @returns {object} Status object with label and color
 */
export const getAirQualityStatus = (pm25) => {
  const thresholds = SENSOR_THRESHOLDS.pm25;
  
  if (pm25 <= thresholds.good) {
    return { label: 'خوب', color: '#00e400', level: 'good' };
  } else if (pm25 <= thresholds.moderate) {
    return { label: 'متوسط', color: '#ffff00', level: 'moderate' };
  } else if (pm25 <= thresholds.unhealthy) {
    return { label: 'ناسالم', color: '#ff7e00', level: 'unhealthy' };
  } else if (pm25 <= thresholds.veryUnhealthy) {
    return { label: 'بسیار ناسالم', color: '#ff0000', level: 'very_unhealthy' };
  } else {
    return { label: 'خطرناک', color: '#8f3f97', level: 'hazardous' };
  }
};


// ============================================================================
// CONFIGURATION VALIDATION
// ============================================================================

/**
 * Validate configuration on startup
 */
export const validateConfig = () => {
  const errors = [];
  const warnings = [];
  
  // Check API URL
  if (API_CONFIG.BASE_URL.includes('192.168.1.100')) {
    warnings.push('API_CONFIG.BASE_URL is set to default. Update to your Django server IP.');
  }
  
  // Check if using http in production
  if (isProduction() && API_CONFIG.BASE_URL.startsWith('http://')) {
    warnings.push('Using HTTP in production. Consider using HTTPS for security.');
  }
  
  // Check debug mode
  if (isProduction() && DEV_CONFIG.DEBUG) {
    warnings.push('DEBUG mode is enabled in production!');
  }
  
  // Check polling intervals
  if (POLLING_CONFIG.SENSOR_DATA_INTERVAL < 1000) {
    warnings.push('Sensor data polling interval is less than 1 second. This may cause performance issues.');
  }
  
  // Print results
  if (errors.length > 0 || warnings.length > 0) {
    console.group('⚙️ Configuration Validation');
    
    if (errors.length > 0) {
      console.error('❌ Errors:');
      errors.forEach(error => console.error(`  - ${error}`));
    }
    
    if (warnings.length > 0) {
      console.warn('⚠️ Warnings:');
      warnings.forEach(warning => console.warn(`  - ${warning}`));
    }
    
    console.groupEnd();
  } else {
    console.log('✅ Configuration validated successfully');
  }
  
  return { errors, warnings };
};


// ============================================================================
// PRINT CONFIGURATION (for debugging)
// ============================================================================

/**
 * Print configuration summary to console
 */
export const printConfig = () => {
  console.group('📋 Frontend Configuration');
  console.log('API Base URL:', API_CONFIG.BASE_URL);
  console.log('Environment:', isDevelopment() ? 'Development' : 'Production');
  console.log('Debug Mode:', DEV_CONFIG.DEBUG);
  console.log('Logging Enabled:', LOGGING_CONFIG.ENABLED);
  console.log('Polling Interval:', POLLING_CONFIG.SENSOR_DATA_INTERVAL, 'ms');
  console.groupEnd();
};


// Run validation on import (only in development)
if (isDevelopment()) {
  validateConfig();
  if (DEV_CONFIG.CONSOLE_LOGS) {
    printConfig();
  }
}


// ============================================================================
// EXPORT DEFAULT CONFIG
// ============================================================================

export default {
  API: API_CONFIG,
  POLLING: POLLING_CONFIG,
  LOGGING: LOGGING_CONFIG,
  DISPLAY: DISPLAY_CONFIG,
  THRESHOLDS: SENSOR_THRESHOLDS,
  DEFAULTS: DEFAULT_VALUES,
  ERROR: ERROR_CONFIG,
  DEV: DEV_CONFIG,
  PERFORMANCE: PERFORMANCE_CONFIG,
};
