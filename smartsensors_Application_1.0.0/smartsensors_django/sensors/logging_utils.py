"""
Logging utilities for comprehensive system monitoring and security
"""
from django.utils import timezone
from django.db import models
from sensors.models import (
    APIAccessLog, AuthenticationLog, SecurityEvent, 
    ESP32ConnectionLog, SystemErrorLog, FrontendLog, IPBlacklist
)
import traceback
import json


class SecurityLogger:
    """Centralized security logging"""
    
    @staticmethod
    def log_api_access(request, response, start_time):
        """Log API access with timing information"""
        try:
            response_time = int((timezone.now() - start_time).total_seconds() * 1000)
            
            APIAccessLog.objects.create(
                ip_address=SecurityLogger.get_client_ip(request),
                user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
                method=request.method,
                endpoint=request.path,
                status_code=response.status_code,
                response_time_ms=response_time,
                request_body_size=len(request.body) if hasattr(request, 'body') else 0,
                response_body_size=len(response.content) if hasattr(response, 'content') else 0,
            )
        except Exception as e:
            # Don't let logging failures break the application
            print(f"Failed to log API access: {e}")
    
    @staticmethod
    def log_authentication(request, auth_type, status, username=None, failure_reason=None):
        """Log authentication attempts"""
        try:
            AuthenticationLog.objects.create(
                ip_address=SecurityLogger.get_client_ip(request),
                user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
                auth_type=auth_type,
                username=username,
                status=status,
                failure_reason=failure_reason,
            )
        except Exception as e:
            print(f"Failed to log authentication: {e}")
    
    @staticmethod
    def log_security_event(request, event_type, severity, description, request_data=None):
        """Log security events"""
        try:
            # Sanitize request data - remove sensitive information
            sanitized_data = None
            if request_data:
                sanitized_data = str(request_data)[:1000]
            
            SecurityEvent.objects.create(
                ip_address=SecurityLogger.get_client_ip(request),
                user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
                event_type=event_type,
                severity=severity,
                description=description,
                request_path=request.path,
                request_method=request.method,
                request_data=sanitized_data,
            )
            
            # Auto-block IPs for critical security events
            if severity == 'critical':
                SecurityLogger.check_and_block_ip(request)
                
        except Exception as e:
            print(f"Failed to log security event: {e}")
    
    @staticmethod
    def log_esp32_connection(device_ip, network_mode, sensors_included, payload_size, 
                            processing_time_ms=None, errors=None):
        """Log ESP32 device connections"""
        try:
            ESP32ConnectionLog.objects.create(
                device_ip=device_ip,
                network_mode=network_mode,
                data_received=errors is None,
                sensors_included=sensors_included,
                payload_size=payload_size,
                processing_time_ms=processing_time_ms,
                errors=errors,
            )
        except Exception as e:
            print(f"Failed to log ESP32 connection: {e}")
    
    @staticmethod
    def log_system_error(level, module, message, function=None, stack_trace=None, 
                        request=None):
        """Log system errors and exceptions"""
        try:
            user_ip = None
            request_path = None
            
            if request:
                user_ip = SecurityLogger.get_client_ip(request)
                request_path = request.path
            
            SystemErrorLog.objects.create(
                level=level,
                module=module,
                function=function,
                message=message,
                stack_trace=stack_trace,
                request_path=request_path,
                user_ip=user_ip,
            )
        except Exception as e:
            print(f"Failed to log system error: {e}")
    
    @staticmethod
    def log_frontend_event(request, level, component, message, error_stack=None):
        """Log frontend events"""
        try:
            FrontendLog.objects.create(
                level=level,
                user_ip=SecurityLogger.get_client_ip(request),
                user_agent=request.META.get('HTTP_USER_AGENT', '')[:500],
                component=component,
                message=message,
                error_stack=error_stack,
                url=request.META.get('HTTP_REFERER', '')[:500],
            )
        except Exception as e:
            print(f"Failed to log frontend event: {e}")
    
    @staticmethod
    def get_client_ip(request):
        """Extract real client IP address (handle proxies)"""
        x_forwarded_for = request.META.get('HTTP_X_FORWARDED_FOR')
        if x_forwarded_for:
            ip = x_forwarded_for.split(',')[0].strip()
        else:
            ip = request.META.get('REMOTE_ADDR')
        return ip
    
    @staticmethod
    def check_and_block_ip(request):
        """Check if IP should be blocked based on violation history"""
        ip = SecurityLogger.get_client_ip(request)
        
        # Count recent security violations (last hour)
        one_hour_ago = timezone.now() - timezone.timedelta(hours=1)
        violation_count = SecurityEvent.objects.filter(
            ip_address=ip,
            timestamp__gte=one_hour_ago,
            severity__in=['high', 'critical']
        ).count()
        
        # Auto-block if more than 5 violations in an hour
        if violation_count >= 5:
            IPBlacklist.objects.update_or_create(
                ip_address=ip,
                defaults={
                    'reason': f'Auto-blocked: {violation_count} security violations in 1 hour',
                    'blocked_by': 'auto',
                    'violations_count': violation_count,
                    'is_active': True,
                    'expires_at': timezone.now() + timezone.timedelta(hours=24),
                }
            )
    
    @staticmethod
    def is_ip_blocked(ip):
        """Check if an IP is blocked"""
        try:
            blocked = IPBlacklist.objects.filter(
                ip_address=ip,
                is_active=True
            ).filter(
                models.Q(expires_at__isnull=True) | models.Q(expires_at__gt=timezone.now())
            ).exists()
            return blocked
        except:
            return False


class RateLimiter:
    """Rate limiting with logging"""
    
    @staticmethod
    def check_rate_limit(request, max_requests=100, time_window_seconds=60):
        """
        Check if request should be rate limited
        Returns: (allowed: bool, current_count: int)
        """
        ip = SecurityLogger.get_client_ip(request)
        
        # Check if IP is blocked
        if SecurityLogger.is_ip_blocked(ip):
            SecurityLogger.log_security_event(
                request, 'unauthorized_access', 'high',
                'Blocked IP attempting access'
            )
            return False, 0
        
        # Count recent requests
        time_threshold = timezone.now() - timezone.timedelta(seconds=time_window_seconds)
        recent_requests = APIAccessLog.objects.filter(
            ip_address=ip,
            timestamp__gte=time_threshold
        ).count()
        
        if recent_requests >= max_requests:
            SecurityLogger.log_security_event(
                request, 'rate_limit', 'medium',
                f'Rate limit exceeded: {recent_requests} requests in {time_window_seconds}s'
            )
            return False, recent_requests
        
        return True, recent_requests


def log_exception(request=None):
    """Decorator to log exceptions"""
    def decorator(func):
        def wrapper(*args, **kwargs):
            try:
                return func(*args, **kwargs)
            except Exception as e:
                stack = traceback.format_exc()
                SecurityLogger.log_system_error(
                    level='error',
                    module=func.__module__,
                    function=func.__name__,
                    message=str(e),
                    stack_trace=stack,
                    request=request if request else (args[0] if args else None)
                )
                raise
        return wrapper
    return decorator
