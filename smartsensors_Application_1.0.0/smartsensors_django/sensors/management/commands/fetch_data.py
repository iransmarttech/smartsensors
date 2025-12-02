import requests
from django.core.management.base import BaseCommand
from sensors.models import AirQuality, MR007, ME4SO2, ZE40, DeviceInfo

class Command(BaseCommand):
    help = 'Fetch sensor data from device API'

    def handle(self, *args, **kwargs):
        url = 'http://192.168.43.113/data'
        response = requests.get(url, timeout=5)
        data = response.json()

        aq = data.get('air_quality')
        mr = data.get('mr007')
        me4 = data.get('me4_so2')
        ze40 = {
            'tvoc_ppb': data.get('tvoc_ppb'),
            'tvoc_ppm': data.get('tvoc_ppm'),
            'dac_voltage': data.get('dac_voltage'),
            'dac_ppm': data.get('dac_ppm'),
            'uart_data_valid': data.get('uart_data_valid'),
            'analog_data_valid': data.get('analog_data_valid'),
        }
        info = {
            'ip_address': data.get('ip_address'),
            'network_mode': data.get('network_mode'),
        }

        if aq:
            AirQuality.objects.create(**aq)
        if mr:
            MR007.objects.create(**mr)
        if me4:
            ME4SO2.objects.create(**me4)
        ZE40.objects.create(**ze40)
        DeviceInfo.objects.create(**info)