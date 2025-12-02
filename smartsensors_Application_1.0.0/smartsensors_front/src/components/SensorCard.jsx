import React from "react";
import {
  Activity,
  AlertTriangle,
  Thermometer,
  Droplet,
  Gauge,
  Zap,
  BarChart3,
  Wind,
  Factory,
  Sun,
  Cloud,
  Eye,
} from "lucide-react";

function SensorCard({ title, value, unit, icon, status = "normal" }) {
  const statusColors = {
    // normal: "border-green-200 bg-green-50",
    // warning: "border-yellow-200 bg-yellow-50",
    // danger: "border-red-200 bg-red-50",
  };

  const iconComponents = {
    activity: <Activity className="w-6 h-6 text-secondary" />,
    alert: <AlertTriangle className="w-6 h-6 text-secondary" />,
    thermometer: <Thermometer className="w-6 h-6 text-secondary" />,
    droplet: <Droplet className="w-6 h-6 text-secondary" />,
    gauge: <Gauge className="w-6 h-6 text-secondary" />,
    zap: <Zap className="w-6 h-6 text-secondary" />,
    chart: <BarChart3 className="w-6 h-6 text-secondary" />,
    wind: <Wind className="w-6 h-6 text-secondary" />,
    factory: <Factory className="w-6 h-6 text-secondary" />,
    sun: <Sun className="w-6 h-6 text-secondary" />,
    cloud: <Cloud className="w-6 h-6 text-secondary" />,
    eye: <Eye className="w-6 h-6 text-secondary" />,
  };

  return (
    <div
      className={`bg-fildes rounded-lg p-4 shadow-sm ${statusColors[status]}`}
    >
      <div className="flex items-center justify-between">
        <div>
          <h3 className="text-sm font-medium text-white">{title}</h3>
          <p className="text-2xl font-bold text-white mt-1">
            {value} <span className="text-sm font-normal">{unit}</span>
          </p>
        </div>
        <div>
          {iconComponents[icon] || (
            <Activity className="w-6 h-6 text-gray-600" />
          )}
        </div>
      </div>
    </div>
  );
}

export default SensorCard;
