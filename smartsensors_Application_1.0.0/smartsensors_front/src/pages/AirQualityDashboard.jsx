// import React, { useState, useEffect } from "react";
// import SensorCardWithChart from "../components/SensorCardWithChart";
// import { BarChart3 } from "lucide-react";
// import logger from "../utils/logger";
// import { getSensorDataUrl, getPollingInterval } from "../config";

// // API Configuration from centralized config
// const API_BASE_URL = getSensorDataUrl();
// const POLLING_INTERVAL = getPollingInterval('sensor_data');

// function AirQualityDashboard() {
//   const [data, setData] = useState({});
//   const [history, setHistory] = useState([]); // تغییر: آرایه خالی
//   const [loading, setLoading] = useState(true);
//   const [error, setError] = useState(null);

//   // Fetch data from Django API
//   const fetchSensorData = async () => {
//     const startTime = Date.now();
//     try {
//       logger.info("AirQualityDashboard", "Fetching sensor data from API");
      
//       const response = await fetch(`${API_BASE_URL}`);
//       const responseTime = Date.now() - startTime;
      
//       if (!response.ok) {
//         throw new Error(`HTTP error! status: ${response.status}`);
//       }
      
//       const result = await response.json();
      
//       logger.logAPICall("/", "GET", response.status, responseTime);
      
//       // Extract air quality data
//       if (result.air_quality) {
//         setData(result.air_quality);
//         logger.info("AirQualityDashboard", "Air quality data updated", {
//           pm25: result.air_quality.pm25,
//           temperature: result.air_quality.temperature
//         });
//       }
      
//       // Extract historical data for charts
//       if (result.history && Array.isArray(result.history.air_quality)) {
//         // مطمئن شویم که آرایه است
//         setHistory(result.history.air_quality);
//       } else {
//         // اگر تاریخچه نداریم یا فرمت اشتباه است، آرایه خالی تنظیم کنیم
//         setHistory([]);
//       }
      
//       setLoading(false);
//       setError(null);
//     } catch (err) {
//       const responseTime = Date.now() - startTime;
//       logger.error("AirQualityDashboard", "Failed to fetch sensor data", err);
//       logger.logAPICall("/", "GET", 0, responseTime);
      
//       console.error("Error fetching sensor data:", err);
//       setError(err.message);
//       setLoading(false);
//     }
//   };

//   // تولید داده تاریخی از داده‌های واقعی با زمان واقعی
//   const generateChartData = (field) => {
//     // مطمئن شویم history یک آرایه است
//     if (!Array.isArray(history) || history.length === 0) {
//       return [];
//     }
    
//     // Get last 10 records and reverse to show oldest first
//     return history
//       .slice(0, 10)
//       .reverse()
//       .map((record) => ({
//         time: record.time_with_seconds || record.timestamp || "00:00:00", // زمان از بک‌اند
//         value: parseFloat(record[field] || 0),
//       }));
//   };

//   // بروزرسانی هر 2 ثانیه
//   useEffect(() => {
//     logger.info("AirQualityDashboard", "Component mounted, starting data fetch");
//     fetchSensorData();
//     const interval = setInterval(fetchSensorData, POLLING_INTERVAL);
//     return () => {
//       logger.info("AirQualityDashboard", "Component unmounting, stopping data fetch");
//       clearInterval(interval);
//     };
//   }, []);

//   const sensors = [
//     {
//       title: "ذرات PM1",
//       value: data.pm1 || 0,
//       unit: "μg/m³",
//       icon: "wind",
//       field: "pm1",
//     },
//     {
//       title: "ذرات PM2.5",
//       value: data.pm25 || 0,
//       unit: "μg/m³",
//       icon: "wind",
//       field: "pm25",
//     },
//     {
//       title: "ذرات PM10",
//       value: data.pm10 || 0,
//       unit: "μg/m³",
//       icon: "cloud",
//       field: "pm10",
//     },
//     {
//       title: "دی اکسید کربن",
//       value: data.co2 || 0,
//       unit: "ppm",
//       icon: "factory",
//       field: "co2",
//     },
//     {
//       title: "VOC",
//       value: data.voc || 0,
//       unit: "ppm",
//       icon: "factory",
//       field: "co2",
//     },
//     {
//       title: "CH2O",
//       value: data.ch2o || 0,
//       unit: "ppm",
//       icon: "factory",
//       field: "co2",
//     },
//     {
//       title: "گاز CO",
//       value: data.co || 0,
//       unit: "ppm",
//       icon: "alert",
//       field: "co",
//     },
//     {
//       title: "ازن O3",
//       value: data.o3 || 0,
//       unit: "ppm",
//       icon: "sun",
//       field: "o3",
//     },
//     {
//       title: "دی اکسید نیتروژن",
//       value: data.no2 || 0,
//       unit: "ppm",
//       icon: "alert",
//       field: "no2",
//     },
//     {
//       title: "دما",
//       value: data.temperature || 0,
//       unit: "°C",
//       icon: "thermometer",
//       field: "temperature",
//     },
//     {
//       title: "رطوبت",
//       value: data.humidity || 0,
//       unit: "%",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "غلظت LEL",
//       value: data.humidity || 0,
//       unit: "%",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "غلظت SO2",
//       value: data.humidity || 0,
//       unit: "ppm",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "TVOC (ppm)",
//       value: data.humidity || 0,
//       unit: "ppm",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "TVOC (ppb)",
//       value: data.humidity || 0,
//       unit: "ppb",
//       icon: "droplet",
//       field: "humidity",
//     },
//   ];

//   return (
//     <div className=" p-6">
//       <div className="flex items-center justify-between mb-8">
//         <h2 className="text-base mobile:text-lg sm:text-xl font-Vazirmatn-bold text-white flex items-center">
//           <BarChart3 className="ml-3 w-7 h-7 text-secondary font-Vazirmatn" />
//           کیفیت هوا - نمایش زنده
//         </h2>
//         <div className={`text-[12px] mobile:text-sm px-4 py-2 rounded-full border font-Vazirmatn-medium text-center ${
//           loading ? 'text-yellow-600 bg-yellow-50 border-yellow-200' :
//           error ? 'text-red-600 bg-red-50 border-red-200' :
//           'text-green-600 bg-green-50 border-green-200'
//         }`}>
//           ● {loading ? 'در حال بارگذاری...' : error ? 'خطا در اتصال' : 'در حال به روزرسانی'}
//         </div>
//       </div>

//       {error && (
//         <div className="mb-6 p-4 bg-red-50 border border-red-200 rounded-lg text-red-600 font-Vazirmatn text-center">
//           خطا در دریافت اطلاعات: {error}
//         </div>
//       )}

//       <div className="grid grid-cols-1 lg:grid-cols-2 2xl:grid-cols-3 gap-6">
//         {sensors.map((sensor) => (
//           <SensorCardWithChart
//             key={sensor.field}
//             title={sensor.title}
//             value={sensor.value}
//             unit={sensor.unit}
//             icon={sensor.icon}
//             color={sensor.color}
//             chartData={generateChartData(sensor.field)}
//           />
//         ))}
//       </div>

//       <div className="mt-8 p-4 bg-primary/5 rounded-lg border border-primary/10">
//         <div className="text-center">
//           <p className="text-sm text-gray-600 font-Vazirmatn">
//             به روز رسانی هر ثانیه
//           </p>
//           <p className="text-xs text-gray-500 mt-1 font-Vazirmatn">
//             آخرین به روزرسانی: {new Date().toLocaleTimeString("fa-IR")}
//           </p>
//         </div>
//       </div>
//     </div>
//   );
// }

// export default AirQualityDashboard;









// import React, { useState, useEffect } from "react";
// import SensorCardWithChart from "../components/SensorCardWithChart";
// import { BarChart3 } from "lucide-react";
// import logger from "../utils/logger";
// import { getSensorDataUrl, getPollingInterval } from "../config";

// // API Configuration from centralized config
// const API_BASE_URL = getSensorDataUrl();
// const POLLING_INTERVAL = getPollingInterval('sensor_data');

// function AirQualityDashboard() {
//   const [data, setData] = useState({});
//   const [history, setHistory] = useState([]); // تغییر: آرایه خالی
//   const [mr007Data, setMr007Data] = useState(null);
//   const [me4so2Data, setMe4so2Data] = useState(null);
//   const [ze40Data, setZe40Data] = useState(null);
//   const [loading, setLoading] = useState(true);
//   const [error, setError] = useState(null);

//   // Fetch data from Django API
//   const fetchSensorData = async () => {
//     const startTime = Date.now();
//     try {
//       logger.info("AirQualityDashboard", "Fetching sensor data from API");
      
//       const response = await fetch(`${API_BASE_URL}`);
//       const responseTime = Date.now() - startTime;
      
//       if (!response.ok) {
//         throw new Error(`HTTP error! status: ${response.status}`);
//       }
      
//       const result = await response.json();
      
//       logger.logAPICall("/", "GET", response.status, responseTime);
      
//       // Extract air quality data
//       if (result.air_quality) {
//         setData(result.air_quality);
//         logger.info("AirQualityDashboard", "Air quality data updated", {
//           pm25: result.air_quality.pm25,
//           temperature: result.air_quality.temperature
//         });
//       }
      
//       // Extract gas sensors data
//       setMr007Data(result.mr007 || null);
//       setMe4so2Data(result.me4_so2 || null);
//       setZe40Data(result.ze40 || null);
      
//       // Extract historical data for charts
//       if (result.history && Array.isArray(result.history.air_quality)) {
//         // مطمئن شویم که آرایه است
//         setHistory(result.history.air_quality);
//       } else {
//         // اگر تاریخچه نداریم یا فرمت اشتباه است، آرایه خالی تنظیم کنیم
//         setHistory([]);
//       }
      
//       setLoading(false);
//       setError(null);
//     } catch (err) {
//       const responseTime = Date.now() - startTime;
//       logger.error("AirQualityDashboard", "Failed to fetch sensor data", err);
//       logger.logAPICall("/", "GET", 0, responseTime);
      
//       console.error("Error fetching sensor data:", err);
//       setError(err.message);
//       setLoading(false);
//     }
//   };

//   // تولید داده تاریخی از داده‌های واقعی با زمان واقعی
//   const generateChartData = (field) => {
//     // مطمئن شویم history یک آرایه است
//     if (!Array.isArray(history) || history.length === 0) {
//       return [];
//     }
    
//     // Get last 10 records and reverse to show oldest first
//     return history
//       .slice(0, 10)
//       .reverse()
//       .map((record) => ({
//         time: record.time_with_seconds || record.timestamp || "00:00:00", // زمان از بک‌اند
//         value: parseFloat(record[field] || 0),
//       }));
//   };

//   // بروزرسانی هر 2 ثانیه
//   useEffect(() => {
//     logger.info("AirQualityDashboard", "Component mounted, starting data fetch");
//     fetchSensorData();
//     const interval = setInterval(fetchSensorData, POLLING_INTERVAL);
//     return () => {
//       logger.info("AirQualityDashboard", "Component unmounting, stopping data fetch");
//       clearInterval(interval);
//     };
//   }, []);

//   const sensors = [
//     {
//       title: "ذرات PM1",
//       value: data.pm1 || 0,
//       unit: "μg/m³",
//       icon: "wind",
//       field: "pm1",
//     },
//     {
//       title: "ذرات PM2.5",
//       value: data.pm25 || 0,
//       unit: "μg/m³",
//       icon: "wind",
//       field: "pm25",
//     },
//     {
//       title: "ذرات PM10",
//       value: data.pm10 || 0,
//       unit: "μg/m³",
//       icon: "cloud",
//       field: "pm10",
//     },
//     {
//       title: "دی اکسید کربن",
//       value: data.co2 || 0,
//       unit: "ppm",
//       icon: "factory",
//       field: "co2",
//     },
//     {
//       title: "VOC",
//       value: data.voc || 0,
//       unit: "ppm",
//       icon: "factory",
//       field: "co2",
//     },
//     {
//       title: "CH2O",
//       value: data.ch2o || 0,
//       unit: "ppm",
//       icon: "factory",
//       field: "co2",
//     },
//     {
//       title: "گاز CO",
//       value: data.co || 0,
//       unit: "ppm",
//       icon: "alert",
//       field: "co",
//     },
//     {
//       title: "ازن O3",
//       value: data.o3 || 0,
//       unit: "ppm",
//       icon: "sun",
//       field: "o3",
//     },
//     {
//       title: "دی اکسید نیتروژن",
//       value: data.no2 || 0,
//       unit: "ppm",
//       icon: "alert",
//       field: "no2",
//     },
//     {
//       title: "دما",
//       value: data.temperature || 0,
//       unit: "°C",
//       icon: "thermometer",
//       field: "temperature",
//     },
//     {
//       title: "رطوبت",
//       value: data.humidity || 0,
//       unit: "%",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "غلظت LEL",
//       value: mr007Data?.lel_concentration || 0,
//       unit: "%",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "غلظت SO2",
//       value: me4so2Data?.so2_concentration || 0,
//       unit: "ppm",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "TVOC (ppm)",
//       value: ze40Data?.tvoc_ppm || 0,
//       unit: "ppm",
//       icon: "droplet",
//       field: "humidity",
//     },
//     {
//       title: "TVOC (ppb)",
//       value: ze40Data?.tvoc_ppb || 0,
//       unit: "ppb",
//       icon: "droplet",
//       field: "humidity",
//     },
//   ];

//   return (
//     <div className=" p-6">
//       <div className="flex items-center justify-between mb-8">
//         <h2 className="text-base mobile:text-lg sm:text-xl font-Vazirmatn-bold text-white flex items-center">
//           <BarChart3 className="ml-3 w-7 h-7 text-secondary font-Vazirmatn" />
//           کیفیت هوا - نمایش زنده
//         </h2>
//         <div className={`text-[12px] mobile:text-sm px-4 py-2 rounded-full border font-Vazirmatn-medium text-center ${
//           loading ? 'text-yellow-600 bg-yellow-50 border-yellow-200' :
//           error ? 'text-red-600 bg-red-50 border-red-200' :
//           'text-green-600 bg-green-50 border-green-200'
//         }`}>
//           ● {loading ? 'در حال بارگذاری...' : error ? 'خطا در اتصال' : 'در حال به روزرسانی'}
//         </div>
//       </div>

//       {error && (
//         <div className="mb-6 p-4 bg-red-50 border border-red-200 rounded-lg text-red-600 font-Vazirmatn text-center">
//           خطا در دریافت اطلاعات: {error}
//         </div>
//       )}

//       <div className="grid grid-cols-1 lg:grid-cols-2 2xl:grid-cols-3 gap-6">
//         {sensors.map((sensor) => (
//           <SensorCardWithChart
//             key={sensor.field}
//             title={sensor.title}
//             value={sensor.value}
//             unit={sensor.unit}
//             icon={sensor.icon}
//             color={sensor.color}
//             chartData={generateChartData(sensor.field)}
//           />
//         ))}
//       </div>

//       <div className="mt-8 p-4 bg-primary/5 rounded-lg border border-primary/10">
//         <div className="text-center">
//           <p className="text-sm text-gray-600 font-Vazirmatn">
//             به روز رسانی هر ثانیه
//           </p>
//           <p className="text-xs text-gray-500 mt-1 font-Vazirmatn">
//             آخرین به روزرسانی: {new Date().toLocaleTimeString("fa-IR")}
//           </p>
//         </div>
//       </div>
//     </div>
//   );
// }

// export default AirQualityDashboard;








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
  const [history, setHistory] = useState({
    air_quality: [],
    mr007: [],
    me4_so2: [],
    ze40: []
  });
  const [mr007Data, setMr007Data] = useState(null);
  const [me4so2Data, setMe4so2Data] = useState(null);
  const [ze40Data, setZe40Data] = useState(null);
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
      
      // Extract gas sensors data
      setMr007Data(result.mr007 || null);
      setMe4so2Data(result.me4_so2 || null);
      setZe40Data(result.ze40 || null);
      
      // Extract ALL historical data for charts
      if (result.history) {
        setHistory({
          air_quality: Array.isArray(result.history.air_quality) ? result.history.air_quality : [],
          mr007: Array.isArray(result.history.mr007) ? result.history.mr007 : [],
          me4_so2: Array.isArray(result.history.me4_so2) ? result.history.me4_so2 : [],
          ze40: Array.isArray(result.history.ze40) ? result.history.ze40 : []
        });
      } else {
        setHistory({
          air_quality: [],
          mr007: [],
          me4_so2: [],
          ze40: []
        });
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
  const generateChartData = (field, sensorType = 'air_quality') => {
    // تعیین کنید کدام تاریخچه را استفاده کنیم
    let historyData = [];
    
    switch(sensorType) {
      case 'mr007':
        historyData = history.mr007;
        break;
      case 'me4_so2':
        historyData = history.me4_so2;
        break;
      case 'ze40':
        historyData = history.ze40;
        break;
      default:
        historyData = history.air_quality;
    }
    
    // مطمئن شویم که آرایه است
    if (!Array.isArray(historyData) || historyData.length === 0) {
      return [];
    }
    
    // Get last 10 records and reverse to show oldest first
    return historyData
      .slice(0, 10)
      .reverse()
      .map((record) => ({
        time: record.time_with_seconds || record.timestamp || "00:00:00",
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
      sensorType: 'air_quality',
    },
    {
      title: "ذرات PM2.5",
      value: data.pm25 || 0,
      unit: "μg/m³",
      icon: "wind",
      field: "pm25",
      sensorType: 'air_quality',
    },
    {
      title: "ذرات PM10",
      value: data.pm10 || 0,
      unit: "μg/m³",
      icon: "cloud",
      field: "pm10",
      sensorType: 'air_quality',
    },
    {
      title: "دی اکسید کربن",
      value: data.co2 || 0,
      unit: "ppm",
      icon: "factory",
      field: "co2",
      sensorType: 'air_quality',
    },
    {
      title: "VOC",
      value: data.voc || 0,
      unit: "ppm",
      icon: "factory",
      field: "voc",
      sensorType: 'air_quality',
    },
    {
      title: "CH2O",
      value: data.ch2o || 0,
      unit: "ppm",
      icon: "factory",
      field: "ch2o",
      sensorType: 'air_quality',
    },
    {
      title: "گاز CO",
      value: data.co || 0,
      unit: "ppm",
      icon: "alert",
      field: "co",
      sensorType: 'air_quality',
    },
    {
      title: "ازن O3",
      value: data.o3 || 0,
      unit: "ppm",
      icon: "sun",
      field: "o3",
      sensorType: 'air_quality',
    },
    {
      title: "دی اکسید نیتروژن",
      value: data.no2 || 0,
      unit: "ppm",
      icon: "alert",
      field: "no2",
      sensorType: 'air_quality',
    },
    {
      title: "دما",
      value: data.temperature || 0,
      unit: "°C",
      icon: "thermometer",
      field: "temperature",
      sensorType: 'air_quality',
    },
    {
      title: "رطوبت",
      value: data.humidity || 0,
      unit: "%",
      icon: "droplet",
      field: "humidity",
      sensorType: 'air_quality',
    },
    {
      title: "غلظت LEL",
      value: mr007Data?.lel_concentration || 0,
      unit: "%",
      icon: "flame",
      field: "lel_concentration",
      sensorType: 'mr007',
    },
    {
      title: "غلظت SO2",
      value: me4so2Data?.so2_concentration || 0,
      unit: "ppm",
      icon: "alert-triangle",
      field: "so2_concentration",
      sensorType: 'me4_so2',
    },
    {
      title: "TVOC (ppm)",
      value: ze40Data?.tvoc_ppm || 0,
      unit: "ppm",
      icon: "chart",
      field: "tvoc_ppm",
      sensorType: 'ze40',
    },
    {
      title: "TVOC (ppb)",
      value: ze40Data?.tvoc_ppb || 0,
      unit: "ppb",
      icon: "chart",
      field: "tvoc_ppb",
      sensorType: 'ze40',
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
            key={`${sensor.sensorType}-${sensor.field}`}
            title={sensor.title}
            value={sensor.value}
            unit={sensor.unit}
            icon={sensor.icon}
            color={sensor.color}
            chartData={generateChartData(sensor.field, sensor.sensorType)}
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