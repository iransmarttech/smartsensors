from django.shortcuts import render
from django.http import JsonResponse
from django.views.decorators.csrf import csrf_exempt
from django.views.decorators.http import require_http_methods
from django.utils import timezone
import json
import traceback
from sensors.models import AirQuality, MR007, ME4SO2, ZE40, DeviceInfo
from sensors.logging_utils import SecurityLogger


def dashboard(request):
    """Render dashboard with latest sensor data"""
    try:
        aq = AirQuality.objects.order_by('-timestamp').first()
        mr = MR007.objects.order_by('-timestamp').first()
        me4 = ME4SO2.objects.order_by('-timestamp').first()
        ze40 = ZE40.objects.order_by('-timestamp').first()
        info = DeviceInfo.objects.order_by('-timestamp').first()
        
        return render(request, 'dashboard.html', {
            'aq': aq, 'mr': mr, 'me4': me4, 'ze40': ze40, 'info': info
        })
    except Exception as e:
        SecurityLogger.log_system_error(
            level='error',
            module='views',
            function='dashboard',
            message=str(e),
            stack_trace=traceback.format_exc(),
            request=request
        )
        raise
    aq = AirQuality.objects.order_by('-timestamp').first()
    mr = MR007.objects.order_by('-timestamp').first()
    me4 = ME4SO2.objects.order_by('-timestamp').first()
    ze40 = ZE40.objects.order_by('-timestamp').first()
    info = DeviceInfo.objects.order_by('-timestamp').first()
    return render(request, 'dashboard.html', {
        'aq': aq, 'mr': mr, 'me4': me4, 'ze40': ze40, 'info': info
    })

# /data API endpoint
def data_api(request):
    """Serve sensor data to frontend"""
    try:
        # Latest values
        aq = AirQuality.objects.order_by('-timestamp').first()
        mr = MR007.objects.order_by('-timestamp').first()
        me4 = ME4SO2.objects.order_by('-timestamp').first()
        ze40 = ZE40.objects.order_by('-timestamp').first()
        info = DeviceInfo.objects.order_by('-timestamp').first()

        # Historical data (last 50 records for each sensor)
        history = {
            'air_quality': list(AirQuality.objects.order_by('-timestamp')[:50].values()),
            'mr007': list(MR007.objects.order_by('-timestamp')[:50].values()),
            'me4_so2': list(ME4SO2.objects.order_by('-timestamp')[:50].values()),
            'ze40': list(ZE40.objects.order_by('-timestamp')[:50].values()),
            'device_info': list(DeviceInfo.objects.order_by('-timestamp')[:50].values()),
        }

        result = {
            'air_quality': aq and {
                'pm1': aq.pm1,
                'pm25': aq.pm25,
                'pm10': aq.pm10,
                'co2': aq.co2,
                'voc': aq.voc,
                'ch2o': aq.ch2o,
                'co': aq.co,
                'o3': aq.o3,
                'no2': aq.no2,
                'temperature': aq.temperature,
                'humidity': aq.humidity,
            },
            'mr007': mr and {
                'voltage': mr.voltage,
                'rawValue': mr.rawValue,
                'lel_concentration': mr.lel_concentration,
            },
            'me4_so2': me4 and {
                'voltage': me4.voltage,
                'rawValue': me4.rawValue,
                'current_ua': me4.current_ua,
                'so2_concentration': me4.so2_concentration,
            },
            'ze40': ze40 and {
                'tvoc_ppb': ze40.tvoc_ppb,
                'tvoc_ppm': ze40.tvoc_ppm,
                'dac_voltage': ze40.dac_voltage,
                'dac_ppm': ze40.dac_ppm,
                'uart_data_valid': ze40.uart_data_valid,
                'analog_data_valid': ze40.analog_data_valid,
            },
            'ip_address': info.ip_address if info else None,
            'network_mode': info.network_mode if info else None,
            'history': history,
        }
        return JsonResponse(result)
    except Exception as e:
        SecurityLogger.log_system_error(
            level='error',
            module='views',
            function='data_api',
            message=str(e),
            stack_trace=traceback.format_exc(),
            request=request
        )
        return JsonResponse({'error': 'Internal server error'}, status=500)

# POST endpoint to receive data from ESP32
@csrf_exempt
@require_http_methods(["POST"])
def receive_sensor_data(request):
    """Receive and store sensor data from ESP32"""
    start_time = timezone.now()
    device_ip = SecurityLogger.get_client_ip(request)
    payload_size = len(request.body)
    sensors_included = []
    errors = []
    
    try:
        data = json.loads(request.body)
        
        # Track which sensors sent data
        if 'air_quality' in data and data['air_quality']:
            sensors_included.append('air_quality')
        if 'mr007' in data and data['mr007']:
            sensors_included.append('mr007')
        if 'me4_so2' in data and data['me4_so2']:
            sensors_included.append('me4_so2')
        if 'ze40' in data and data['ze40']:
            sensors_included.append('ze40')
        
        # Store Air Quality data if present
        if 'air_quality' in data and data['air_quality']:
            try:
                aq_data = data['air_quality']
                AirQuality.objects.create(
                    pm1=aq_data.get('pm1', 0),
                    pm25=aq_data.get('pm25', 0),
                    pm10=aq_data.get('pm10', 0),
                    co2=aq_data.get('co2', 0),
                    voc=aq_data.get('voc', 0),
                    ch2o=aq_data.get('ch2o', 0),
                    co=aq_data.get('co', 0.0),
                    o3=aq_data.get('o3', 0.0),
                    no2=aq_data.get('no2', 0.0),
                    temperature=aq_data.get('temperature', 0.0),
                    humidity=aq_data.get('humidity', 0)
                )
            except Exception as e:
                errors.append(f"AirQuality: {str(e)}")
        
        # Store MR007 data if present
        if 'mr007' in data and data['mr007']:
            try:
                mr_data = data['mr007']
                MR007.objects.create(
                    voltage=mr_data.get('voltage', 0.0),
                    rawValue=mr_data.get('rawValue', 0),
                    lel_concentration=mr_data.get('lel_concentration', 0.0)
                )
            except Exception as e:
                errors.append(f"MR007: {str(e)}")
        
        # Store ME4-SO2 data if present
        if 'me4_so2' in data and data['me4_so2']:
            try:
                me4_data = data['me4_so2']
                ME4SO2.objects.create(
                    voltage=me4_data.get('voltage', 0.0),
                    rawValue=me4_data.get('rawValue', 0),
                    current_ua=me4_data.get('current_ua', 0.0),
                    so2_concentration=me4_data.get('so2_concentration', 0.0)
                )
            except Exception as e:
                errors.append(f"ME4SO2: {str(e)}")
        
        # Store ZE40 data if present
        if 'ze40' in data and data['ze40']:
            try:
                ze40_data = data['ze40']
                ZE40.objects.create(
                    tvoc_ppb=ze40_data.get('tvoc_ppb', 0.0),
                    tvoc_ppm=ze40_data.get('tvoc_ppm', 0.0),
                    dac_voltage=ze40_data.get('dac_voltage', 0.0),
                    dac_ppm=ze40_data.get('dac_ppm', 0.0),
                    uart_data_valid=ze40_data.get('uart_data_valid', False),
                    analog_data_valid=ze40_data.get('analog_data_valid', True)
                )
            except Exception as e:
                errors.append(f"ZE40: {str(e)}")
        
        # Store device info if present
        if 'ip_address' in data or 'network_mode' in data:
            try:
                DeviceInfo.objects.create(
                    ip_address=data.get('ip_address', device_ip),
                    network_mode=data.get('network_mode', 'unknown')
                )
            except Exception as e:
                errors.append(f"DeviceInfo: {str(e)}")
        
        # Calculate processing time
        processing_time = int((timezone.now() - start_time).total_seconds() * 1000)
        
        # Log ESP32 connection
        SecurityLogger.log_esp32_connection(
            device_ip=device_ip,
            network_mode=data.get('network_mode', 'unknown'),
            sensors_included=sensors_included,
            payload_size=payload_size,
            processing_time_ms=processing_time,
            errors='; '.join(errors) if errors else None
        )
        
        if errors:
            return JsonResponse({
                'status': 'partial_success', 
                'message': 'Some data stored with errors',
                'errors': errors
            }, status=207)
        
        return JsonResponse({
            'status': 'success', 
            'message': 'Data stored successfully',
            'sensors_received': sensors_included,
            'processing_time_ms': processing_time
        })
    
    except json.JSONDecodeError as e:
        error_msg = f"Invalid JSON: {str(e)}"
        SecurityLogger.log_security_event(
            request, 'malformed_request', 'medium',
            error_msg
        )
        SecurityLogger.log_esp32_connection(
            device_ip=device_ip,
            network_mode='unknown',
            sensors_included=[],
            payload_size=payload_size,
            errors=error_msg
        )
        return JsonResponse({'status': 'error', 'message': error_msg}, status=400)
    
    except Exception as e:
        error_msg = str(e)
        SecurityLogger.log_system_error(
            level='error',
            module='views',
            function='receive_sensor_data',
            message=error_msg,
            stack_trace=traceback.format_exc(),
            request=request
        )
        SecurityLogger.log_esp32_connection(
            device_ip=device_ip,
            network_mode=data.get('network_mode', 'unknown') if 'data' in locals() else 'unknown',
            sensors_included=sensors_included,
            payload_size=payload_size,
            errors=error_msg
        )
        return JsonResponse({'status': 'error', 'message': error_msg}, status=500)


# Frontend logging endpoint
@csrf_exempt
@require_http_methods(["POST"])
def log_frontend_event(request):
    """Receive logs from frontend"""
    try:
        data = json.loads(request.body)
        
        SecurityLogger.log_frontend_event(
            request=request,
            level=data.get('level', 'info'),
            component=data.get('component', 'unknown'),
            message=data.get('message', ''),
            error_stack=data.get('error_stack')
        )
        
        return JsonResponse({'status': 'success'})
    except Exception as e:
        return JsonResponse({'status': 'error', 'message': str(e)}, status=500)


# Log viewing endpoints (for admin/monitoring)
def view_logs(request):
    """View system logs (requires authentication in production)"""
    try:
        log_type = request.GET.get('type', 'api')
        limit = int(request.GET.get('limit', 100))
        
        from sensors.models import (
            APIAccessLog, AuthenticationLog, SecurityEvent,
            ESP32ConnectionLog, SystemErrorLog, FrontendLog
        )
        
        logs = {
            'api': APIAccessLog.objects.all()[:limit],
            'auth': AuthenticationLog.objects.all()[:limit],
            'security': SecurityEvent.objects.all()[:limit],
            'esp32': ESP32ConnectionLog.objects.all()[:limit],
            'errors': SystemErrorLog.objects.all()[:limit],
            'frontend': FrontendLog.objects.all()[:limit],
        }
        
        selected_logs = logs.get(log_type, logs['api'])
        
        return render(request, 'logs_viewer.html', {
            'logs': selected_logs,
            'log_type': log_type,
            'available_types': logs.keys()
        })
    except Exception as e:
        SecurityLogger.log_system_error(
            level='error',
            module='views',
            function='view_logs',
            message=str(e),
            stack_trace=traceback.format_exc(),
            request=request
        )
        return JsonResponse({'error': str(e)}, status=500)


def api_logs_json(request):
    """Get logs as JSON for API access"""
    try:
        log_type = request.GET.get('type', 'security')
        limit = int(request.GET.get('limit', 100))
        severity = request.GET.get('severity')
        ip = request.GET.get('ip')
        
        from sensors.models import (
            APIAccessLog, AuthenticationLog, SecurityEvent,
            ESP32ConnectionLog, SystemErrorLog, FrontendLog
        )
        
        if log_type == 'security':
            queryset = SecurityEvent.objects.all()
            if severity:
                queryset = queryset.filter(severity=severity)
            if ip:
                queryset = queryset.filter(ip_address=ip)
            logs = list(queryset[:limit].values())
            
        elif log_type == 'api':
            queryset = APIAccessLog.objects.all()
            if ip:
                queryset = queryset.filter(ip_address=ip)
            logs = list(queryset[:limit].values())
            
        elif log_type == 'auth':
            queryset = AuthenticationLog.objects.all()
            if ip:
                queryset = queryset.filter(ip_address=ip)
            logs = list(queryset[:limit].values())
            
        elif log_type == 'esp32':
            queryset = ESP32ConnectionLog.objects.all()
            if ip:
                queryset = queryset.filter(device_ip=ip)
            logs = list(queryset[:limit].values())
            
        elif log_type == 'errors':
            queryset = SystemErrorLog.objects.all()
            if ip:
                queryset = queryset.filter(user_ip=ip)
            logs = list(queryset[:limit].values())
            
        elif log_type == 'frontend':
            queryset = FrontendLog.objects.all()
            if ip:
                queryset = queryset.filter(user_ip=ip)
            logs = list(queryset[:limit].values())
        else:
            logs = []
        
        return JsonResponse({
            'logs': logs,
            'count': len(logs),
            'type': log_type
        })
    except Exception as e:
        SecurityLogger.log_system_error(
            level='error',
            module='views',
            function='api_logs_json',
            message=str(e),
            stack_trace=traceback.format_exc(),
            request=request
        )
        return JsonResponse({'error': str(e)}, status=500)
