import React from "react";
import SensorCard from "./SensorCard";
import SensorStatus from "./SensorStatus";
import {
  Flame,
  Zap,
  Gauge,
  Activity,
  AlertTriangle,
  Eye,
  ChartBar,
} from "lucide-react";

const iconComponents = {
  Flame,
  Zap,
  Gauge,
  Activity,
  AlertTriangle,
  Eye,
  ChartBar,
};

function SensorGroup({ title, subtitle, icon, sensors, statusData }) {
  const IconComponent = iconComponents[icon];

  return (
    <div className="bg-box rounded-xl p-6">
      {/* هدر گروه سنسور */}
      <div className="flex items-center gap-3 mb-6">
        <div
          className={`w-12 h-12 rounded-xl flex items-center justify-center  bg-secondary/30 text-secondary`}
        >
          <IconComponent className="w-6 h-6" />
        </div>
        <div>
          <h3 className="text-xl font-Vazirmatn-bold text-white">{title}</h3>
          <p className="text-white text-sm font-Vazirmatn mt-1">{subtitle}</p>
        </div>
      </div>

      {/* سنسورها */}
      <div
        className={`grid grid-cols-1 gap-4 ${
          sensors.length <= 3 ? "md:grid-cols-3" : "md:grid-cols-4"
        }`}
      >
        {sensors.map((sensor, index) => (
          <SensorCard
            key={index}
            title={sensor.title}
            value={sensor.value}
            unit={sensor.unit}
            icon={sensor.icon}
            color={sensor.color}
            precision={sensor.precision}
          />
        ))}
      </div>

      {/* وضعیت سنسور (فقط برای ZE40) */}
      {statusData && (
        <div className="mt-6 pt-6 border-t border-gray-200">
          <SensorStatus
            uartValid={statusData.uart_data_valid}
            analogValid={statusData.analog_data_valid}
          />
        </div>
      )}
    </div>
  );
}

export default SensorGroup;
