import React, { useState, useEffect } from "react";
import SensorGroup from "../components/SensorGroup";
import SystemStatus from "../components/SystemStatus";
import logger from "../utils/logger";
import { getSensorDataUrl, getPollingInterval } from "../config";

// API Configuration from centralized config
const API_BASE_URL = getSensorDataUrl();
const POLLING_INTERVAL = getPollingInterval('gas_sensors');

function GasSensorsPanel() {
  const [mr007Data, setMr007Data] = useState(null);
  const [me4so2Data, setMe4so2Data] = useState(null);
  const [ze40Data, setZe40Data] = useState(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState(null);

  // Fetch data from Django API
  const fetchSensorData = async () => {
    const startTime = Date.now();
    try {
      logger.info('GasSensorsPanel', 'Fetching gas sensor data', { url: `${API_BASE_URL}` });
      
      const response = await fetch(`${API_BASE_URL}`);
      const responseTime = Date.now() - startTime;
      
      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }
      const result = await response.json();
      
      logger.logAPICall(`${API_BASE_URL}/data`, 'GET', response.status, responseTime);
      logger.info('GasSensorsPanel', 'Gas sensor data received', { 
        responseTime: `${responseTime}ms`,
        hasMR007: !!result.mr007,
        hasME4SO2: !!result.me4_so2,
        hasZE40: !!result.ze40
      });
      
      setMr007Data(result.mr007);
      setMe4so2Data(result.me4_so2);
      setZe40Data(result.ze40);
      
      setLoading(false);
      setError(null);
    } catch (err) {
      const responseTime = Date.now() - startTime;
      logger.error('GasSensorsPanel', 'Failed to fetch gas sensor data', err, { 
        url: `${API_BASE_URL}/data`,
        responseTime: `${responseTime}ms`
      });
      console.error("Error fetching sensor data:", err);
      setError(err.message);
      setLoading(false);
    }
  };

  // Fetch data every 2 seconds
  useEffect(() => {
    logger.info('GasSensorsPanel', 'Component mounted - initializing gas sensor monitoring');
    fetchSensorData();
    const interval = setInterval(fetchSensorData, POLLING_INTERVAL);
    return () => {
      logger.info('GasSensorsPanel', 'Component unmounting - cleaning up interval');
      clearInterval(interval);
    };
  }, []);

  if (loading) {
    return (
      <div className="p-6 text-center">
        <div className="text-white font-Vazirmatn">در حال بارگذاری...</div>
      </div>
    );
  }

  if (error) {
    return (
      <div className="p-6">
        <div className="p-4 bg-red-50 border border-red-200 rounded-lg text-red-600 font-Vazirmatn text-center">
          خطا در دریافت اطلاعات: {error}
        </div>
      </div>
    );
  }

  // تعریف سنسورها به صورت داینامیک
  const sensorGroups = [
    {
      id: "mr007",
      title: "سنسور MR007",
      subtitle: "گازهای قابل اشتعال",
      icon: "Flame",
      iconColor: "red",
      sensors: [
        {
          title: "ولتاژ",
          value: mr007Data?.voltage || 0,
          unit: "V",
          icon: "zap",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "مقدار خام",
          value: mr007Data?.rawValue || 0,
          unit: "",
          icon: "gauge",
          color: "#59ac77",
        },
        {
          title: "غلظت LEL",
          value: mr007Data?.lel_concentration || 0,
          unit: "%",
          icon: "activity",
          color: "#59ac77",
          precision: 1,
        },
      ],
    },
    {
      id: "me4so2",
      title: "سنسور ME4SO2",
      subtitle: "دی اکسید گوگرد",
      icon: "AlertTriangle",
      iconColor: "yellow",
      sensors: [
        {
          title: "ولتاژ",
          value: me4so2Data?.voltage || 0,
          unit: "V",
          icon: "zap",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "مقدار خام",
          value: me4so2Data?.rawValue || 0,
          unit: "",
          icon: "gauge",
          color: "#59ac77",
        },
        {
          title: "جریان",
          value: me4so2Data?.current_ua || 0,
          unit: "μA",
          icon: "activity",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "غلظت SO2",
          value: me4so2Data?.so2_concentration || 0,
          unit: "ppm",
          icon: "alert",
          color: "#59ac77",
          precision: 1,
        },
      ],
    },
    {
      id: "ze40",
      title: "سنسور ZE40",
      subtitle: "ترکیبات آلی فرار",
      icon: "Eye",
      iconColor: "blue",
      sensors: [
        {
          title: "TVOC (ppb)",
          value: ze40Data?.tvoc_ppb || 0,
          unit: "ppb",
          icon: "chart",
          color: "#59ac77",
        },
        {
          title: "TVOC (ppm)",
          value: ze40Data?.tvoc_ppm || 0,
          unit: "ppm",
          icon: "chart",
          color: "#59ac77",
          precision: 2,
        },
        {
          title: "ولتاژ DAC",
          value: ze40Data?.dac_voltage || 0,
          unit: "V",
          icon: "zap",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "DAC (ppm)",
          value: ze40Data?.dac_ppm || 0,
          unit: "ppm",
          icon: "gauge",
          color: "#59ac77",
          precision: 2,
        },
      ],
      statusData: ze40Data || {},
    },
  ];

  return (
    <div className="space-y-6">
      {sensorGroups.map((group) => (
        <SensorGroup
          key={group.id}
          title={group.title}
          subtitle={group.subtitle}
          icon={group.icon}
          iconColor={group.iconColor}
          sensors={group.sensors}
          statusData={group.statusData}
        />
      ))}

      <SystemStatus />
    </div>
  );
}

export default GasSensorsPanel;