import React, { useState, useEffect } from "react";
import SensorCardWithChart from "../components/SensorCardWithChart";
import { BarChart3 } from "lucide-react";

function AirQualityDashboard() {
  const [data, setData] = useState({});

  // تابع ساده برای تولید داده رندوم
  const generateRandomData = () => {
    return {
      pm1: Math.floor(Math.random() * 50) + 10,
      pm25: Math.floor(Math.random() * 100) + 20,
      pm10: Math.floor(Math.random() * 150) + 30,
      co2: Math.floor(Math.random() * 500) + 400,
      co: (Math.random() * 10).toFixed(1),
      o3: (Math.random() * 0.1).toFixed(2),
      no2: (Math.random() * 0.05).toFixed(3),
      temperature: (Math.random() * 40 + 10).toFixed(1),
      humidity: Math.floor(Math.random() * 60) + 30,
    };
  };

  // تولید داده تاریخی ساده
  const generateChartData = (field) => {
    const dataPoints = [];

    for (let i = 0; i < 10; i++) {
      let value;

      if (field === "temperature") {
        value = (Math.random() * 40 + 10).toFixed(1);
      } else if (field === "co") {
        value = (Math.random() * 10).toFixed(1);
      } else if (field === "o3") {
        value = (Math.random() * 0.1).toFixed(2);
      } else if (field === "no2") {
        value = (Math.random() * 0.05).toFixed(3);
      } else if (field === "pm1") {
        value = Math.floor(Math.random() * 50) + 10;
      } else if (field === "pm25") {
        value = Math.floor(Math.random() * 100) + 20;
      } else if (field === "pm10") {
        value = Math.floor(Math.random() * 150) + 30;
      } else if (field === "co2") {
        value = Math.floor(Math.random() * 500) + 400;
      } else {
        value = Math.floor(Math.random() * 60) + 30;
      }

      dataPoints.push({
        time: `${i + 1}`,
        value: parseFloat(value),
      });
    }

    return dataPoints;
  };

  // بروزرسانی هر ثانیه
  useEffect(() => {
    const interval = setInterval(() => {
      setData(generateRandomData());
    }, 1000);

    return () => clearInterval(interval);
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
        <div className="text-[12px] mobile:text-sm text-green-600 bg-green-50 px-4 py-2 rounded-full border border-green-200 font-Vazirmatn-medium text-center">
          ● در حال به روزرسانی
        </div>
      </div>

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
