import React from "react";
import RealTimeChart from "./RealTimeChart";
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

function SensorCardWithChart({ title, value, unit, icon, chartData }) {
  const iconComponents = {
    activity: <Activity className="w-5 h-5 text-secondary" />,
    alert: <AlertTriangle className="w-5 h-5 text-secondary" />,
    thermometer: <Thermometer className="w-5 h-5 text-secondary" />,
    droplet: <Droplet className="w-5 h-5 text-secondary" />,
    gauge: <Gauge className="w-5 h-5 text-secondary" />,
    zap: <Zap className="w-5 h-5 text-secondary" />,
    chart: <BarChart3 className="w-5 h-5 text-secondary" />,
    wind: <Wind className="w-5 h-5 text-secondary" />,
    factory: <Factory className="w-5 h-5 text-secondary" />,
    sun: <Sun className="w-5 h-5 text-secondary" />,
    cloud: <Cloud className="w-5 h-5 text-secondary" />,
    eye: <Eye className="w-5 h-5 text-secondary" />,
  };

  return (
    <div className="bg-box rounded-lg shadow-lg   overflow-hidden">
      {/* هدر کارت */}
      <div className="flex items-center justify-between p-4  ">
        <div className="flex items-center">
          {iconComponents[icon]}
          <h3 className="text-sm font-medium text-white mr-2">{title}</h3>
        </div>
        <div className="text-right">
          <p className="text-2xl font-bold text-white">
            {value}{" "}
            <span className="text-sm font-normal text-white">{unit}</span>
          </p>
        </div>
      </div>

      {/* نمودار */}
      <div className="p-3">
        <RealTimeChart data={chartData} title={`نمودار ${title}`} />
      </div>
    </div>
  );
}

export default SensorCardWithChart;
