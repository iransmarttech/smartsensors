from django.urls import path
from .views import (
    dashboard, data_api, receive_sensor_data,
    log_frontend_event, view_logs, api_logs_json
)

urlpatterns = [
    path('', dashboard, name='dashboard'),
    path('data', data_api, name='data_api'),
    path('api/sensors', receive_sensor_data, name='receive_sensor_data'),
    path('api/log/frontend', log_frontend_event, name='log_frontend_event'),
    path('logs', view_logs, name='view_logs'),
    path('api/logs', api_logs_json, name='api_logs_json'),
]