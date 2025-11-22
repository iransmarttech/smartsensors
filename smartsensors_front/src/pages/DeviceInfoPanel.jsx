import React, { useState, useEffect } from "react";
import { Wifi, Server, Clock, MapPin } from "lucide-react";
import logger from "../utils/logger";
import { getSensorDataUrl, getPollingInterval } from "../config";

// API Configuration from centralized config
const API_BASE_URL = getSensorDataUrl();
const POLLING_INTERVAL = getPollingInterval('device_info');

function DeviceInfoPanel() {
  const [deviceInfo, setDeviceInfo] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  // Fetch data from Django API
  const fetchDeviceInfo = async () => {
    const startTime = Date.now();
    try {
      logger.info('DeviceInfoPanel', 'Fetching device information', { url: `${API_BASE_URL}/data` });
      
      const response = await fetch(`${API_BASE_URL}/data`);
      const responseTime = Date.now() - startTime;
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const result = await response.json();
      
      logger.logAPICall(`${API_BASE_URL}/data`, 'GET', response.status, responseTime);
      logger.info('DeviceInfoPanel', 'Device information received', { 
        responseTime: `${responseTime}ms`,
        ip: result.ip_address,
        networkMode: result.network_mode
      });
      
      setDeviceInfo({
        ip_address: result.ip_address,
        network_mode: result.network_mode,
        timestamp: new Date().toISOString(),
      });
      
      setLoading(false);
      setError(null);
    } catch (err) {
      const responseTime = Date.now() - startTime;
      logger.error('DeviceInfoPanel', 'Failed to fetch device information', err, { 
        url: `${API_BASE_URL}/data`,
        responseTime: `${responseTime}ms`
      });
      console.error("Error fetching device info:", err);
      setError(err.message);
      setLoading(false);
    }
  };

  // Fetch data every 2 seconds
  useEffect(() => {
    logger.info('DeviceInfoPanel', 'Component mounted - initializing device monitoring');
    fetchDeviceInfo();
    const interval = setInterval(fetchDeviceInfo, POLLING_INTERVAL);
    return () => {
      logger.info('DeviceInfoPanel', 'Component unmounting - cleaning up interval');
      clearInterval(interval);
    };
  }, []);

  if (loading) {
    return (
      <div className="bg-box rounded-lg shadow-lg p-6">
        <div className="text-white font-Vazirmatn text-center">در حال بارگذاری...</div>
      </div>
    );
  }

  if (error) {
    return (
      <div className="bg-box rounded-lg shadow-lg p-6">
        <div className="p-4 bg-red-50 border border-red-200 rounded-lg text-red-600 font-Vazirmatn text-center">
          خطا در دریافت اطلاعات: {error}
        </div>
      </div>
    );
  }
  return (
    <div className="bg-box rounded-lg shadow-lg p-6">
      <h2 className="text-xl font-bold text-white mb-6 flex items-center">
        <Server className="ml-2 w-6 h-6" />
        اطلاعات دستگاه
      </h2>

      <div className="grid grid-cols-1 md:grid-cols-2 gap-6">
        <div className="flex items-center p-4 bg-fildes  rounded-lg">
          <MapPin className="ml-3 w-5 h-5 text-secondary" />
          <div>
            <p className="text-sm text-white font-Vazirmatn">آدرس IP</p>
            <p className="font-Vazirmatn-semibold text-white">
              {deviceInfo?.ip_address || "نامشخص"}
            </p>
          </div>
        </div>

        <div className="flex items-center p-4 bg-fildes rounded-lg">
          <Wifi className="ml-3 w-5 h-5 text-secondary" />
          <div>
            <p className="text-sm text-white font-Vazirmatn">حالت شبکه</p>
            <p className="font-Vazirmatn-semibold text-white">
              {deviceInfo?.network_mode || "نامشخص"}
            </p>
          </div>
        </div>

        <div className="flex items-center p-4 bg-fildes rounded-lg">
          <Clock className="ml-3 w-5 h-5 text-secondary" />
          <div>
            <p className="text-sm text-white font-Vazirmatn">آخرین بروزرسانی</p>
            <p className="font-Vazirmatn-semibold text-white">
              {deviceInfo?.timestamp
                ? new Date(deviceInfo.timestamp).toLocaleString("fa-IR")
                : "نامشخص"}
            </p>
          </div>
        </div>
      </div>
    </div>
  );
}

export default DeviceInfoPanel;
