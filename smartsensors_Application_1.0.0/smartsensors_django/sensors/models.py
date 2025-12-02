# from django.db import models

# class AirQuality(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     pm1 = models.IntegerField()
#     pm25 = models.IntegerField()
#     pm10 = models.IntegerField()
#     co2 = models.IntegerField()
#     voc = models.IntegerField()
#     ch2o = models.IntegerField()
#     co = models.FloatField()
#     o3 = models.FloatField()
#     no2 = models.FloatField()
#     temperature = models.FloatField()
#     humidity = models.IntegerField()

# class MR007(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     voltage = models.FloatField()
#     rawValue = models.IntegerField()
#     lel_concentration = models.FloatField()

# class ME4SO2(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     voltage = models.FloatField()
#     rawValue = models.IntegerField()
#     current_ua = models.FloatField()
#     so2_concentration = models.FloatField()

# class ZE40(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     tvoc_ppb = models.FloatField()
#     tvoc_ppm = models.FloatField()
#     dac_voltage = models.FloatField()
#     dac_ppm = models.FloatField()
#     uart_data_valid = models.BooleanField()
#     analog_data_valid = models.BooleanField()

# class DeviceInfo(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     ip_address = models.CharField(max_length=32)
#     network_mode = models.CharField(max_length=16)




# from django.db import models

# class AirQuality(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     pm1 = models.IntegerField()
#     pm25 = models.IntegerField()
#     pm10 = models.IntegerField()
#     co2 = models.IntegerField()
#     voc = models.IntegerField()
#     ch2o = models.IntegerField()
#     co = models.FloatField()
#     o3 = models.FloatField()
#     no2 = models.FloatField()
#     temperature = models.FloatField()
#     humidity = models.IntegerField()
    
#     def get_time_with_seconds(self):
#         """زمان با ثانیه: ساعت:دقیقه:ثانیه"""
#         return self.timestamp.strftime('%H:%M:%S') if self.timestamp else None
    
#     def get_date_time(self):
#         """تاریخ و زمان کامل"""
#         return self.timestamp.strftime('%Y-%m-%d %H:%M:%S') if self.timestamp else None
    
#     def get_time_only(self):
#         """فقط زمان بدون ثانیه"""
#         return self.timestamp.strftime('%H:%M') if self.timestamp else None

# class MR007(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     voltage = models.FloatField()
#     rawValue = models.IntegerField()
#     lel_concentration = models.FloatField()
    
#     def get_time_with_seconds(self):
#         return self.timestamp.strftime('%H:%M:%S') if self.timestamp else None
    
#     def get_date_time(self):
#         return self.timestamp.strftime('%Y-%m-%d %H:%M:%S') if self.timestamp else None

# class ME4SO2(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     voltage = models.FloatField()
#     rawValue = models.IntegerField()
#     current_ua = models.FloatField()
#     so2_concentration = models.FloatField()
    
#     def get_time_with_seconds(self):
#         return self.timestamp.strftime('%H:%M:%S') if self.timestamp else None
    
#     def get_date_time(self):
#         return self.timestamp.strftime('%Y-%m-%d %H:%M:%S') if self.timestamp else None

# class ZE40(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     tvoc_ppb = models.FloatField()
#     tvoc_ppm = models.FloatField()
#     dac_voltage = models.FloatField()
#     dac_ppm = models.FloatField()
#     uart_data_valid = models.BooleanField()
#     analog_data_valid = models.BooleanField()
    
#     def get_time_with_seconds(self):
#         return self.timestamp.strftime('%H:%M:%S') if self.timestamp else None
    
#     def get_date_time(self):
#         return self.timestamp.strftime('%Y-%m-%d %H:%M:%S') if self.timestamp else None

# class DeviceInfo(models.Model):
#     timestamp = models.DateTimeField(auto_now_add=True)
#     ip_address = models.CharField(max_length=32)
#     network_mode = models.CharField(max_length=16)
    
#     def get_time_with_seconds(self):
#         return self.timestamp.strftime('%H:%M:%S') if self.timestamp else None
    
#     def get_date_time(self):
#         return self.timestamp.strftime('%Y-%m-%d %H:%M:%S') if self.timestamp else None



from django.db import models
from django.utils import timezone

class AirQuality(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    pm1 = models.IntegerField()
    pm25 = models.IntegerField()
    pm10 = models.IntegerField()
    co2 = models.IntegerField()
    voc = models.IntegerField()
    ch2o = models.IntegerField()
    co = models.FloatField()
    o3 = models.FloatField()
    no2 = models.FloatField()
    temperature = models.FloatField()
    humidity = models.IntegerField()
    
    def get_time_with_seconds(self):
        """زمان با ثانیه: ساعت:دقیقه:ثانیه (ایران)"""
        if self.timestamp:
            # تبدیل به زمان ایران
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%H:%M:%S')
        return None
    
    def get_date_time(self):
        """تاریخ و زمان کامل (ایران)"""
        if self.timestamp:
            # تبدیل به زمان ایران
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%Y-%m-%d %H:%M:%S')
        return None
    
    def get_time_only(self):
        """فقط زمان بدون ثانیه (ایران)"""
        if self.timestamp:
            # تبدیل به زمان ایران
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%H:%M')
        return None

class MR007(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    voltage = models.FloatField()
    rawValue = models.IntegerField()
    lel_concentration = models.FloatField()
    
    def get_time_with_seconds(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%H:%M:%S')
        return None
    
    def get_date_time(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%Y-%m-%d %H:%M:%S')
        return None

class ME4SO2(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    voltage = models.FloatField()
    rawValue = models.IntegerField()
    current_ua = models.FloatField()
    so2_concentration = models.FloatField()
    
    def get_time_with_seconds(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%H:%M:%S')
        return None
    
    def get_date_time(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%Y-%m-%d %H:%M:%S')
        return None

class ZE40(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    tvoc_ppb = models.FloatField()
    tvoc_ppm = models.FloatField()
    dac_voltage = models.FloatField()
    dac_ppm = models.FloatField()
    uart_data_valid = models.BooleanField()
    analog_data_valid = models.BooleanField()
    
    def get_time_with_seconds(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%H:%M:%S')
        return None
    
    def get_date_time(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%Y-%m-%d %H:%M:%S')
        return None

class DeviceInfo(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    ip_address = models.CharField(max_length=32)
    network_mode = models.CharField(max_length=16)
    
    def get_time_with_seconds(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%H:%M:%S')
        return None
    
    def get_date_time(self):
        if self.timestamp:
            iran_time = timezone.localtime(self.timestamp)
            return iran_time.strftime('%Y-%m-%d %H:%M:%S')
        return None

# ============================================================================
# LOGGING AND SECURITY MONITORING MODELS
# ============================================================================

class APIAccessLog(models.Model):
    """Log all API access attempts"""
    timestamp = models.DateTimeField(auto_now_add=True)
    ip_address = models.GenericIPAddressField()
    user_agent = models.TextField(blank=True, null=True)
    method = models.CharField(max_length=10)  # GET, POST, PUT, DELETE
    endpoint = models.CharField(max_length=255)
    status_code = models.IntegerField()
    response_time_ms = models.IntegerField(null=True)
    request_body_size = models.IntegerField(default=0)
    response_body_size = models.IntegerField(default=0)
    
    class Meta:
        ordering = ['-timestamp']
        indexes = [
            models.Index(fields=['-timestamp']),
            models.Index(fields=['ip_address']),
            models.Index(fields=['status_code']),
        ]

class AuthenticationLog(models.Model):
    """Log all authentication attempts"""
    LOG_TYPES = [
        ('web_login', 'Web Login'),
        ('api_token', 'API Token'),
        ('web_access', 'Web Access'),
    ]
    
    STATUS_CHOICES = [
        ('success', 'Success'),
        ('failed', 'Failed'),
        ('blocked', 'Blocked'),
    ]
    
    timestamp = models.DateTimeField(auto_now_add=True)
    ip_address = models.GenericIPAddressField()
    user_agent = models.TextField(blank=True, null=True)
    auth_type = models.CharField(max_length=20, choices=LOG_TYPES)
    username = models.CharField(max_length=255, blank=True, null=True)
    status = models.CharField(max_length=10, choices=STATUS_CHOICES)
    failure_reason = models.CharField(max_length=255, blank=True, null=True)
    
    class Meta:
        ordering = ['-timestamp']
        indexes = [
            models.Index(fields=['-timestamp']),
            models.Index(fields=['ip_address']),
            models.Index(fields=['status']),
        ]

class SecurityEvent(models.Model):
    """Log security-related events"""
    EVENT_TYPES = [
        ('rate_limit', 'Rate Limit Exceeded'),
        ('invalid_token', 'Invalid API Token'),
        ('malformed_request', 'Malformed Request'),
        ('sql_injection', 'SQL Injection Attempt'),
        ('xss_attempt', 'XSS Attempt'),
        ('suspicious_pattern', 'Suspicious Pattern Detected'),
        ('unauthorized_access', 'Unauthorized Access Attempt'),
        ('csrf_failed', 'CSRF Validation Failed'),
    ]
    
    SEVERITY_LEVELS = [
        ('low', 'Low'),
        ('medium', 'Medium'),
        ('high', 'High'),
        ('critical', 'Critical'),
    ]
    
    timestamp = models.DateTimeField(auto_now_add=True)
    ip_address = models.GenericIPAddressField()
    user_agent = models.TextField(blank=True, null=True)
    event_type = models.CharField(max_length=30, choices=EVENT_TYPES)
    severity = models.CharField(max_length=10, choices=SEVERITY_LEVELS)
    description = models.TextField()
    request_path = models.CharField(max_length=255, blank=True, null=True)
    request_method = models.CharField(max_length=10, blank=True, null=True)
    request_data = models.TextField(blank=True, null=True)  # Sanitized request data
    
    class Meta:
        ordering = ['-timestamp']
        indexes = [
            models.Index(fields=['-timestamp']),
            models.Index(fields=['ip_address']),
            models.Index(fields=['severity']),
            models.Index(fields=['event_type']),
        ]

class ESP32ConnectionLog(models.Model):
    """Log ESP32 device connections and data submissions"""
    timestamp = models.DateTimeField(auto_now_add=True)
    device_ip = models.GenericIPAddressField()
    network_mode = models.CharField(max_length=16)
    data_received = models.BooleanField(default=True)
    sensors_included = models.JSONField()  # List of sensors in payload
    payload_size = models.IntegerField()
    processing_time_ms = models.IntegerField(null=True)
    errors = models.TextField(blank=True, null=True)
    
    class Meta:
        ordering = ['-timestamp']
        indexes = [
            models.Index(fields=['-timestamp']),
            models.Index(fields=['device_ip']),
        ]

class SystemErrorLog(models.Model):
    """Log system errors and exceptions"""
    ERROR_LEVELS = [
        ('debug', 'Debug'),
        ('info', 'Info'),
        ('warning', 'Warning'),
        ('error', 'Error'),
        ('critical', 'Critical'),
    ]
    
    timestamp = models.DateTimeField(auto_now_add=True)
    level = models.CharField(max_length=10, choices=ERROR_LEVELS)
    module = models.CharField(max_length=100)
    function = models.CharField(max_length=100, blank=True, null=True)
    message = models.TextField()
    stack_trace = models.TextField(blank=True, null=True)
    request_path = models.CharField(max_length=255, blank=True, null=True)
    user_ip = models.GenericIPAddressField(null=True, blank=True)
    
    class Meta:
        ordering = ['-timestamp']
        indexes = [
            models.Index(fields=['-timestamp']),
            models.Index(fields=['level']),
        ]

class FrontendLog(models.Model):
    """Log frontend events and errors"""
    LOG_LEVELS = [
        ('info', 'Info'),
        ('warning', 'Warning'),
        ('error', 'Error'),
    ]
    
    timestamp = models.DateTimeField(auto_now_add=True)
    level = models.CharField(max_length=10, choices=LOG_LEVELS)
    user_ip = models.GenericIPAddressField()
    user_agent = models.TextField(blank=True, null=True)
    component = models.CharField(max_length=100)
    message = models.TextField()
    error_stack = models.TextField(blank=True, null=True)
    url = models.CharField(max_length=500)
    
    class Meta:
        ordering = ['-timestamp']
        indexes = [
            models.Index(fields=['-timestamp']),
            models.Index(fields=['level']),
        ]

class IPBlacklist(models.Model):
    """Track and block malicious IP addresses"""
    ip_address = models.GenericIPAddressField(unique=True)
    blocked_at = models.DateTimeField(auto_now_add=True)
    reason = models.TextField()
    blocked_by = models.CharField(max_length=50)  # 'auto' or admin username
    violations_count = models.IntegerField(default=1)
    is_active = models.BooleanField(default=True)
    expires_at = models.DateTimeField(null=True, blank=True)
    
    class Meta:
        ordering = ['-blocked_at']
        indexes = [
            models.Index(fields=['ip_address']),
            models.Index(fields=['is_active']),
        ]