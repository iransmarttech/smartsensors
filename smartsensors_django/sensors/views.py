from django.shortcuts import render
from django.http import JsonResponse
from sensors.models import AirQuality, MR007, ME4SO2, ZE40, DeviceInfo

def dashboard(request):
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
