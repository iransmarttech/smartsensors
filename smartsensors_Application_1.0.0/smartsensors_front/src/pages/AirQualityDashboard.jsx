import React, { useState, useEffect } from "react";
import SensorCardWithChart from "../components/SensorCardWithChart";
import { BarChart3 } from "lucide-react";
import logger from "../utils/logger";
import { getSensorDataUrl, getPollingInterval } from "../config";

// API Configuration from centralized config
const API_BASE_URL = getSensorDataUrl();
const POLLING_INTERVAL = getPollingInterval('sensor_data');

function AirQualityDashboard() {
  const [data, setData] = useState({});
  const [history, setHistory] = useState([]); // تغییر: آرایه خالی
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  // Fetch data from Django API
  const fetchSensorData = async () => {
    const startTime = Date.now();
    
    try {
      logger.info("AirQualityDashboard", "Fetching sensor data from API");
      
      const response = await fetch(`${API_BASE_URL}`);
      const responseTime = Date.now() - startTime;
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      
      const result = await response.json();
      
      logger.logAPICall("/", "GET", response.status, responseTime);
      
      // Extract air quality data
      if (result.air_quality) {
        setData(result.air_quality);
        logger.info("AirQualityDashboard", "Air quality data updated", {
          pm25: result.air_quality.pm25,
          temperature: result.air_quality.temperature
        });
      }
      
      // Extract historical data for charts
      if (result.history && Array.isArray(result.history.air_quality)) {
        // مطمئن شویم که آرایه است
        setHistory(result.history.air_quality);
      } else {
        // اگر تاریخچه نداریم یا فرمت اشتباه است، آرایه خالی تنظیم کنیم
        setHistory([]);
      }
      
      setLoading(false);
      setError(null);
    } catch (err) {
      const responseTime = Date.now() - startTime;
      logger.error("AirQualityDashboard", "Failed to fetch sensor data", err);
      logger.logAPICall("/", "GET", 0, responseTime);
      
      console.error("Error fetching sensor data:", err);
      setError(err.message);
      setLoading(false);
    }
  };

  // تولید داده تاریخی از داده‌های واقعی با زمان واقعی
  const generateChartData = (field) => {
    // مطمئن شویم history یک آرایه است
    if (!Array.isArray(history) || history.length === 0) {
      return [];
    }
    
    // Get last 10 records and reverse to show oldest first
    return history
      .slice(0, 10)
      .reverse()
      .map((record) => ({
        time: record.time_with_seconds || record.timestamp || "00:00:00", // زمان از بک‌اند
        value: parseFloat(record[field] || 0),
      }));
  };

  // بروزرسانی هر 2 ثانیه
  useEffect(() => {
    logger.info("AirQualityDashboard", "Component mounted, starting data fetch");
    fetchSensorData();
    const interval = setInterval(fetchSensorData, POLLING_INTERVAL);
    return () => {
      logger.info("AirQualityDashboard", "Component unmounting, stopping data fetch");
      clearInterval(interval);
    };
  }, []);

  const sensors = [
    {
      title: "ذرات PM1",
      value: data.pm1 || 0,
      unit: "μg/m³",
      icon: "wind",
      field: "pm1",
    },
    {
      title: "ذرات PM2.5",
      value: data.pm25 || 0,
      unit: "μg/m³",
      icon: "wind",
      field: "pm25",
    },
    {
      title: "ذرات PM10",
      value: data.pm10 || 0,
      unit: "μg/m³",
      icon: "cloud",
      field: "pm10",
    },
    {
      title: "دی اکسید کربن",
      value: data.co2 || 0,
      unit: "ppm",
      icon: "factory",
      field: "co2",
    },
    {
      title: "گاز CO",
      value: data.co || 0,
      unit: "ppm",
      icon: "alert",
      field: "co",
    },
    {
      title: "ازن O3",
      value: data.o3 || 0,
      unit: "ppm",
      icon: "sun",
      field: "o3",
    },
    {
      title: "دی اکسید نیتروژن",
      value: data.no2 || 0,
      unit: "ppm",
      icon: "alert",
      field: "no2",
    },
    {
      title: "دما",
      value: data.temperature || 0,
      unit: "°C",
      icon: "thermometer",
      field: "temperature",
    },
    {
      title: "رطوبت",
      value: data.humidity || 0,
      unit: "%",
      icon: "droplet",
      field: "humidity",
    },
  ];

  return (
    <div className=" p-6">
      <div className="flex items-center justify-between mb-8">
        <h2 className="text-base mobile:text-lg sm:text-xl font-Vazirmatn-bold text-white flex items-center">
          <BarChart3 className="ml-3 w-7 h-7 text-secondary font-Vazirmatn" />
          کیفیت هوا - نمایش زنده
        </h2>
        <div className={`text-[12px] mobile:text-sm px-4 py-2 rounded-full border font-Vazirmatn-medium text-center ${
          loading ? 'text-yellow-600 bg-yellow-50 border-yellow-200' :
          error ? 'text-red-600 bg-red-50 border-red-200' :
          'text-green-600 bg-green-50 border-green-200'
        }`}>
          ● {loading ? 'در حال بارگذاری...' : error ? 'خطا در اتصال' : 'در حال به روزرسانی'}
        </div>
      </div>

      {error && (
        <div className="mb-6 p-4 bg-red-50 border border-red-200 rounded-lg text-red-600 font-Vazirmatn text-center">
          خطا در دریافت اطلاعات: {error}
        </div>
      )}

      <div className="grid grid-cols-1 lg:grid-cols-2 2xl:grid-cols-3 gap-6">
        {sensors.map((sensor) => (
          <SensorCardWithChart
            key={sensor.field}
            title={sensor.title}
            value={sensor.value}
            unit={sensor.unit}
            icon={sensor.icon}
            color={sensor.color}
            chartData={generateChartData(sensor.field)}
          />
        ))}
      </div>

      <div className="mt-8 p-4 bg-primary/5 rounded-lg border border-primary/10">
        <div className="text-center">
          <p className="text-sm text-gray-600 font-Vazirmatn">
            به روز رسانی هر ثانیه
          </p>
          <p className="text-xs text-gray-500 mt-1 font-Vazirmatn">
            آخرین به روزرسانی: {new Date().toLocaleTimeString("fa-IR")}
          </p>
        </div>
      </div>
    </div>
  );
}

export default AirQualityDashboard;