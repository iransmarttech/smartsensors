from django.db import models

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

class MR007(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    voltage = models.FloatField()
    rawValue = models.IntegerField()
    lel_concentration = models.FloatField()

class ME4SO2(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    voltage = models.FloatField()
    rawValue = models.IntegerField()
    current_ua = models.FloatField()
    so2_concentration = models.FloatField()

class ZE40(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    tvoc_ppb = models.FloatField()
    tvoc_ppm = models.FloatField()
    dac_voltage = models.FloatField()
    dac_ppm = models.FloatField()
    uart_data_valid = models.BooleanField()
    analog_data_valid = models.BooleanField()

class DeviceInfo(models.Model):
    timestamp = models.DateTimeField(auto_now_add=True)
    ip_address = models.CharField(max_length=32)
    network_mode = models.CharField(max_length=16)