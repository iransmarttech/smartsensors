from django.contrib import admin
from .models import (
    AirQuality, MR007, ME4SO2, ZE40, DeviceInfo,
    APIAccessLog, AuthenticationLog, SecurityEvent,
    ESP32ConnectionLog, SystemErrorLog, FrontendLog, IPBlacklist
)

# Sensor Data Models
@admin.register(AirQuality)
class AirQualityAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'pm25', 'pm10', 'co2', 'temperature', 'humidity')
    list_filter = ('timestamp',)
    date_hierarchy = 'timestamp'

@admin.register(MR007)
class MR007Admin(admin.ModelAdmin):
    list_display = ('timestamp', 'voltage', 'lel_concentration')
    list_filter = ('timestamp',)
    date_hierarchy = 'timestamp'

@admin.register(ME4SO2)
class ME4SO2Admin(admin.ModelAdmin):
    list_display = ('timestamp', 'voltage', 'so2_concentration')
    list_filter = ('timestamp',)
    date_hierarchy = 'timestamp'

@admin.register(ZE40)
class ZE40Admin(admin.ModelAdmin):
    list_display = ('timestamp', 'tvoc_ppb', 'tvoc_ppm', 'dac_voltage')
    list_filter = ('timestamp', 'uart_data_valid', 'analog_data_valid')
    date_hierarchy = 'timestamp'

@admin.register(DeviceInfo)
class DeviceInfoAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'ip_address', 'network_mode')
    list_filter = ('timestamp', 'network_mode')
    date_hierarchy = 'timestamp'

# Logging and Security Models
@admin.register(APIAccessLog)
class APIAccessLogAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'ip_address', 'method', 'endpoint', 'status_code', 'response_time_ms')
    list_filter = ('timestamp', 'method', 'status_code')
    search_fields = ('ip_address', 'endpoint', 'user_agent')
    date_hierarchy = 'timestamp'
    readonly_fields = ('timestamp', 'ip_address', 'user_agent', 'method', 'endpoint', 
                      'status_code', 'response_time_ms', 'request_body_size', 'response_body_size')

@admin.register(AuthenticationLog)
class AuthenticationLogAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'ip_address', 'auth_type', 'username', 'status')
    list_filter = ('timestamp', 'auth_type', 'status')
    search_fields = ('ip_address', 'username')
    date_hierarchy = 'timestamp'
    readonly_fields = ('timestamp', 'ip_address', 'user_agent', 'auth_type', 
                      'username', 'status', 'failure_reason')

@admin.register(SecurityEvent)
class SecurityEventAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'ip_address', 'event_type', 'severity', 'description')
    list_filter = ('timestamp', 'event_type', 'severity')
    search_fields = ('ip_address', 'description')
    date_hierarchy = 'timestamp'
    readonly_fields = ('timestamp', 'ip_address', 'user_agent', 'event_type', 
                      'severity', 'description', 'request_path', 'request_method', 'request_data')
    
    def has_delete_permission(self, request, obj=None):
        # Prevent accidental deletion of security logs
        return request.user.is_superuser

@admin.register(ESP32ConnectionLog)
class ESP32ConnectionLogAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'device_ip', 'network_mode', 'data_received', 'processing_time_ms')
    list_filter = ('timestamp', 'network_mode', 'data_received')
    search_fields = ('device_ip',)
    date_hierarchy = 'timestamp'
    readonly_fields = ('timestamp', 'device_ip', 'network_mode', 'data_received', 
                      'sensors_included', 'payload_size', 'processing_time_ms', 'errors')

@admin.register(SystemErrorLog)
class SystemErrorLogAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'level', 'module', 'function', 'message')
    list_filter = ('timestamp', 'level', 'module')
    search_fields = ('message', 'module', 'function')
    date_hierarchy = 'timestamp'
    readonly_fields = ('timestamp', 'level', 'module', 'function', 'message', 
                      'stack_trace', 'request_path', 'user_ip')

@admin.register(FrontendLog)
class FrontendLogAdmin(admin.ModelAdmin):
    list_display = ('timestamp', 'level', 'user_ip', 'component', 'message')
    list_filter = ('timestamp', 'level', 'component')
    search_fields = ('user_ip', 'message', 'component')
    date_hierarchy = 'timestamp'
    readonly_fields = ('timestamp', 'level', 'user_ip', 'user_agent', 
                      'component', 'message', 'error_stack', 'url')

@admin.register(IPBlacklist)
class IPBlacklistAdmin(admin.ModelAdmin):
    list_display = ('ip_address', 'blocked_at', 'reason', 'blocked_by', 'violations_count', 'is_active')
    list_filter = ('blocked_at', 'is_active', 'blocked_by')
    search_fields = ('ip_address', 'reason')
    date_hierarchy = 'blocked_at'
    
    actions = ['activate_block', 'deactivate_block']
    
    def activate_block(self, request, queryset):
        queryset.update(is_active=True)
        self.message_user(request, f"{queryset.count()} IPs activated.")
    activate_block.short_description = "Activate selected IP blocks"
    
    def deactivate_block(self, request, queryset):
        queryset.update(is_active=False)
        self.message_user(request, f"{queryset.count()} IPs deactivated.")
    deactivate_block.short_description = "Deactivate selected IP blocks"
