"""
Django Backend Configuration
=============================

Central configuration file for the Smart Sensors Django backend.
Modify these settings based on your deployment environment.
"""

# ============================================================================
# NETWORK CONFIGURATION
# ============================================================================

# Django Server Settings
DJANGO_HOST = '0.0.0.0'  # Listen on all interfaces
DJANGO_PORT = 8000        # Default Django port

# External Access
# Set this to your server's IP address or domain name
# This is used by ESP32 and Frontend to communicate with Django
DJANGO_URL = 'http://192.168.1.4:8000'  # Your laptop's IP on the network

# Alternative configurations (uncomment as needed):
# DJANGO_URL = 'http://localhost:8000'              # Local development
# DJANGO_URL = 'http://192.168.1.100:8000'          # Local network
# DJANGO_URL = 'http://your-domain.com'             # Production domain
# DJANGO_URL = 'https://your-domain.com'            # Production with SSL


# ============================================================================
# CORS CONFIGURATION
# ============================================================================

# Frontend URLs that can access this backend
CORS_ALLOWED_ORIGINS = [
    'http://localhost:5173',      # Vite dev server on localhost
    'http://127.0.0.1:5173',      # Vite dev server on 127.0.0.1
    'http://192.168.1.4:5173',    # Vite dev server on your laptop's LAN IP
    'http://localhost:3000',      # Alternative frontend port
]

# For development only - allows all origins (DISABLE IN PRODUCTION!)
CORS_ALLOW_ALL_ORIGINS = True  # Set to False in production

# Allowed hosts for Django
ALLOWED_HOSTS = [
    'localhost',
    '127.0.0.1',
    '192.168.1.4',    # Your laptop's IP
    '192.168.1.3',    # ESP32 board IP
    '192.168.1.0',    # Entire LAN subnet
]


# ============================================================================
# ESP32 CONFIGURATION
# ============================================================================

# Expected ESP32 IP range (for logging/monitoring)
ESP32_IP_RANGE = '192.168.1.0/24'  # Adjust to your network

# ESP32 timeout settings (seconds)
ESP32_TIMEOUT = 30  # How long to wait for ESP32 data before considering it offline


# ============================================================================
# SECURITY CONFIGURATION
# ============================================================================

# Rate Limiting
RATE_LIMIT_MAX_REQUESTS = 100  # Max requests per window
RATE_LIMIT_WINDOW_SECONDS = 60  # Time window in seconds

# Auto IP Blocking
AUTO_BLOCK_VIOLATIONS_THRESHOLD = 5  # Number of violations before auto-block
AUTO_BLOCK_DURATION_HOURS = 24       # How long to block IP (hours)

# Security Event Severities that trigger auto-blocking
AUTO_BLOCK_SEVERITIES = ['high', 'critical']


# ============================================================================
# LOGGING CONFIGURATION
# ============================================================================

# Log retention periods (days)
LOG_RETENTION = {
    'api_access': 90,        # API access logs
    'auth_success': 365,     # Successful authentications
    'auth_failed': None,     # Failed auth (keep forever)
    'security': None,        # Security events (keep forever)
    'esp32': 90,             # ESP32 connection logs
    'errors': None,          # System errors (keep forever)
    'frontend': 30,          # Frontend logs
}

# Enable/disable different log types
LOGGING_ENABLED = {
    'api_access': True,
    'authentication': True,
    'security': True,
    'esp32': True,
    'system_errors': True,
    'frontend': True,
}


# ============================================================================
# DATABASE CONFIGURATION
# ============================================================================

# Default: SQLite (for development)
# For production, consider PostgreSQL or MySQL

DATABASE_ENGINE = 'sqlite'  # Options: 'sqlite', 'postgresql', 'mysql'

# SQLite settings (default)
SQLITE_DB_PATH = 'db.sqlite3'

# PostgreSQL settings (if using PostgreSQL)
POSTGRES_CONFIG = {
    'NAME': 'smartsensors_db',
    'USER': 'postgres',
    'PASSWORD': 'your_password',
    'HOST': 'localhost',
    'PORT': '5432',
}

# MySQL settings (if using MySQL)
MYSQL_CONFIG = {
    'NAME': 'smartsensors_db',
    'USER': 'root',
    'PASSWORD': 'your_password',
    'HOST': 'localhost',
    'PORT': '3306',
}


# ============================================================================
# API CONFIGURATION
# ============================================================================

# API Endpoints
API_PREFIX = '/api'  # Base path for all API endpoints

# API Response settings
API_TIMEOUT = 30  # Timeout for API responses (seconds)
API_MAX_PAYLOAD_SIZE = 1024 * 1024  # 1MB max payload


# ============================================================================
# SENSOR DATA CONFIGURATION
# ============================================================================

# Default sensor values (used when no data received)
DEFAULT_SENSOR_DATA = {
    'air_quality': {
        'pm25': 0.0,
        'pm10': 0.0,
    },
    'zphs01b': {
        'pm25': 0.0,
        'pm10': 0.0,
    },
    'mr007': {
        'voltage': 0.0,
        'rawValue': 0,
        'lel_concentration': 0.0,
    },
    'me4_so2': {
        'voltage': 0.0,
        'rawValue': 0,
        'current_ua': 0.0,
        'so2_concentration': 0.0,
    },
    'ze40': {
        'tvoc_ppb': 0,
        'tvoc_ppm': 0.0,
        'dac_voltage': 0.0,
        'dac_ppm': 0.0,
        'uart_data_valid': False,
        'analog_data_valid': False,
    },
}

# Data validation ranges (min, max)
SENSOR_RANGES = {
    'pm25': (0, 500),       # μg/m³
    'pm10': (0, 500),       # μg/m³
    'voltage': (0, 5),      # Volts
    'so2_ppm': (0, 100),    # ppm
    'tvoc_ppb': (0, 10000), # ppb
}


# ============================================================================
# DEVELOPMENT/PRODUCTION MODE
# ============================================================================

# Set to False in production!
DEBUG_MODE = True

# Secret key (CHANGE THIS IN PRODUCTION!)
# Generate new key: python -c "from django.core.management.utils import get_random_secret_key; print(get_random_secret_key())"
SECRET_KEY = 'django-insecure-change-this-in-production-xyz123'


# ============================================================================
# NOTIFICATIONS (Future Enhancement)
# ============================================================================

# Email settings for security alerts
EMAIL_ENABLED = False
EMAIL_CONFIG = {
    'host': 'smtp.gmail.com',
    'port': 587,
    'username': 'your-email@gmail.com',
    'password': 'your-app-password',
    'from_email': 'alerts@smartsensors.com',
    'admin_emails': ['admin@example.com'],
}

# Alert settings
SEND_ALERTS_FOR = {
    'critical_security_events': True,
    'esp32_offline': True,
    'system_errors': False,
}


# ============================================================================
# HELPER FUNCTIONS
# ============================================================================

def get_django_url():
    """Get the Django URL for external access"""
    return DJANGO_URL.rstrip('/')

def get_api_url(endpoint=''):
    """Get full API URL for an endpoint"""
    return f"{get_django_url()}{API_PREFIX}/{endpoint.lstrip('/')}"

def is_production():
    """Check if running in production mode"""
    return not DEBUG_MODE

def get_cors_origins():
    """Get list of allowed CORS origins"""
    if CORS_ALLOW_ALL_ORIGINS:
        return ['*']
    return CORS_ALLOWED_ORIGINS


# ============================================================================
# CONFIGURATION VALIDATION
# ============================================================================

def validate_config():
    """Validate configuration settings"""
    errors = []
    warnings = []
    
    # Check Django URL
    if DJANGO_URL == 'http://192.168.1.100:8000':
        warnings.append("Django URL is set to default. Update DJANGO_URL to your server IP.")
    
    # Check secret key
    if 'change-this' in SECRET_KEY.lower():
        errors.append("SECRET_KEY must be changed for production!")
    
    # Check debug mode
    if DEBUG_MODE and not is_production():
        warnings.append("DEBUG_MODE is enabled. Disable in production!")
    
    # Check CORS
    if CORS_ALLOW_ALL_ORIGINS and is_production():
        errors.append("CORS_ALLOW_ALL_ORIGINS must be False in production!")
    
    return errors, warnings


# ============================================================================
# EXPORT CONFIGURATION (for use in settings.py)
# ============================================================================

CONFIG = {
    'DJANGO_HOST': DJANGO_HOST,
    'DJANGO_PORT': DJANGO_PORT,
    'DJANGO_URL': DJANGO_URL,
    'CORS_ALLOWED_ORIGINS': CORS_ALLOWED_ORIGINS,
    'CORS_ALLOW_ALL_ORIGINS': CORS_ALLOW_ALL_ORIGINS,
    'ALLOWED_HOSTS': ALLOWED_HOSTS,
    'DEBUG_MODE': DEBUG_MODE,
    'SECRET_KEY': SECRET_KEY,
    'RATE_LIMIT_MAX_REQUESTS': RATE_LIMIT_MAX_REQUESTS,
    'RATE_LIMIT_WINDOW_SECONDS': RATE_LIMIT_WINDOW_SECONDS,
    'AUTO_BLOCK_VIOLATIONS_THRESHOLD': AUTO_BLOCK_VIOLATIONS_THRESHOLD,
    'AUTO_BLOCK_DURATION_HOURS': AUTO_BLOCK_DURATION_HOURS,
}


if __name__ == '__main__':
    # Print configuration summary
    print("=" * 70)
    print("SMART SENSORS - Django Configuration")
    print("=" * 70)
    print(f"\nDjango URL: {DJANGO_URL}")
    print(f"Debug Mode: {DEBUG_MODE}")
    print(f"CORS Allow All: {CORS_ALLOW_ALL_ORIGINS}")
    print(f"Allowed Hosts: {', '.join(ALLOWED_HOSTS)}")
    print(f"\nRate Limiting: {RATE_LIMIT_MAX_REQUESTS} requests / {RATE_LIMIT_WINDOW_SECONDS}s")
    print(f"Auto-block after: {AUTO_BLOCK_VIOLATIONS_THRESHOLD} violations")
    
    # Validate configuration
    errors, warnings = validate_config()
    
    if errors:
        print("\n⚠️  ERRORS:")
        for error in errors:
            print(f"  ❌ {error}")
    
    if warnings:
        print("\n⚠️  WARNINGS:")
        for warning in warnings:
            print(f"  ⚠️  {warning}")
    
    if not errors and not warnings:
        print("\n✅ Configuration looks good!")
    
    print("=" * 70)
