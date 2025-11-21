import React from "react";
import SensorGroup from "../components/SensorGroup";
import SystemStatus from "../components/SystemStatus";

function GasSensorsPanel({ mr007Data, me4so2Data, ze40Data }) {
  // داده‌های تستی برای نمایش
  const defaultData = {
    mr007Data: {
      voltage: 3.3,
      rawValue: 2450,
      lel_concentration: 15.2,
    },
    me4so2Data: {
      voltage: 3.3,
      rawValue: 1800,
      current_ua: 45.6,
      so2_concentration: 2.1,
    },
    ze40Data: {
      tvoc_ppb: 350,
      tvoc_ppm: 0.35,
      dac_voltage: 1.8,
      dac_ppm: 0.42,
      uart_data_valid: true,
      analog_data_valid: true,
    },
  };

  // استفاده از داده‌های تستی اگر داده واقعی نبود
  const data = {
    mr007Data: mr007Data || defaultData.mr007Data,
    me4so2Data: me4so2Data || defaultData.me4so2Data,
    ze40Data: ze40Data || defaultData.ze40Data,
  };

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
          value: data.mr007Data.voltage,
          unit: "V",
          icon: "zap",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "مقدار خام",
          value: data.mr007Data.rawValue,
          unit: "",
          icon: "gauge",
          color: "#59ac77",
        },
        {
          title: "غلظت LEL",
          value: data.mr007Data.lel_concentration,
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
          value: data.me4so2Data.voltage,
          unit: "V",
          icon: "zap",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "مقدار خام",
          value: data.me4so2Data.rawValue,
          unit: "",
          icon: "gauge",
          color: "#59ac77",
        },
        {
          title: "جریان",
          value: data.me4so2Data.current_ua,
          unit: "μA",
          icon: "activity",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "غلظت SO2",
          value: data.me4so2Data.so2_concentration,
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
          value: data.ze40Data.tvoc_ppb,
          unit: "ppb",
          icon: "chart",
          color: "#59ac77",
        },
        {
          title: "TVOC (ppm)",
          value: data.ze40Data.tvoc_ppm,
          unit: "ppm",
          icon: "chart",
          color: "#59ac77",
          precision: 2,
        },
        {
          title: "ولتاژ DAC",
          value: data.ze40Data.dac_voltage,
          unit: "V",
          icon: "zap",
          color: "#59ac77",
          precision: 1,
        },
        {
          title: "DAC (ppm)",
          value: data.ze40Data.dac_ppm,
          unit: "ppm",
          icon: "gauge",
          color: "#59ac77",
          precision: 2,
        },
      ],
      statusData: data.ze40Data,
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
