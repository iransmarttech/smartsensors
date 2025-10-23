from django.urls import path
from .views import dashboard, data_api

urlpatterns = [
    path('', dashboard, name='dashboard'),
    path('data', data_api, name='data_api'),
]