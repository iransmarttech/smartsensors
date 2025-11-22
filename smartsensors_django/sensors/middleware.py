"""
Middleware for automatic request/response logging and security monitoring
"""
from django.utils import timezone
from django.http import JsonResponse
from sensors.logging_utils import SecurityLogger, RateLimiter
import time


class SecurityLoggingMiddleware:
    """Log all requests and responses for security monitoring"""
    
    def __init__(self, get_response):
        self.get_response = get_response
    
    def __call__(self, request):
        # Record start time
        request.start_time = timezone.now()
        
        # Check rate limiting
        allowed, count = RateLimiter.check_rate_limit(request, max_requests=100, time_window_seconds=60)
        if not allowed:
            return JsonResponse({
                'error': 'Rate limit exceeded',
                'message': 'Too many requests. Please try again later.'
            }, status=429)
        
        # Check IP blacklist
        client_ip = SecurityLogger.get_client_ip(request)
        if SecurityLogger.is_ip_blocked(client_ip):
            SecurityLogger.log_security_event(
                request, 'unauthorized_access', 'high',
                'Blocked IP attempted access'
            )
            return JsonResponse({
                'error': 'Access denied',
                'message': 'Your IP address has been blocked.'
            }, status=403)
        
        # Process request
        response = self.get_response(request)
        
        # Log the request/response
        SecurityLogger.log_api_access(request, response, request.start_time)
        
        return response
    
    def process_exception(self, request, exception):
        """Log exceptions"""
        import traceback
        SecurityLogger.log_system_error(
            level='error',
            module='middleware',
            function='process_exception',
            message=str(exception),
            stack_trace=traceback.format_exc(),
            request=request
        )


class SuspiciousPatternMiddleware:
    """Detect and log suspicious patterns in requests"""
    
    SUSPICIOUS_PATTERNS = [
        'union select', 'drop table', 'exec(', 'script>',
        '../', '..\\', '/etc/passwd', 'cmd.exe',
        '<script', 'javascript:', 'onerror=', 'onload=',
    ]
    
    def __init__(self, get_response):
        self.get_response = get_response
    
    def __call__(self, request):
        # Check for suspicious patterns
        self.check_suspicious_content(request)
        
        response = self.get_response(request)
        return response
    
    def check_suspicious_content(self, request):
        """Check request for suspicious patterns"""
        suspicious_found = []
        
        # Check URL
        path_lower = request.path.lower()
        for pattern in self.SUSPICIOUS_PATTERNS:
            if pattern in path_lower:
                suspicious_found.append(f"URL contains '{pattern}'")
        
        # Check POST body
        if request.method == 'POST' and hasattr(request, 'body'):
            try:
                body_str = request.body.decode('utf-8', errors='ignore').lower()
                for pattern in self.SUSPICIOUS_PATTERNS:
                    if pattern in body_str:
                        suspicious_found.append(f"Body contains '{pattern}'")
            except:
                pass
        
        # Check GET parameters
        for key, value in request.GET.items():
            value_lower = str(value).lower()
            for pattern in self.SUSPICIOUS_PATTERNS:
                if pattern in value_lower:
                    suspicious_found.append(f"GET param '{key}' contains '{pattern}'")
        
        # Log if suspicious patterns found
        if suspicious_found:
            event_type = 'sql_injection' if any('select' in s or 'drop' in s for s in suspicious_found) else \
                        'xss_attempt' if any('script' in s or 'javascript' in s for s in suspicious_found) else \
                        'suspicious_pattern'
            
            SecurityLogger.log_security_event(
                request,
                event_type=event_type,
                severity='high',
                description=f"Suspicious patterns detected: {', '.join(suspicious_found)}",
                request_data=request.path
            )
