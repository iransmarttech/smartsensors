import React from "react";
import { CheckCircle, XCircle, Activity } from "lucide-react";

const StatusBadge = ({ isValid, label }) => (
  <div
    className={`flex items-center gap-2 px-3 py-2 rounded-lg border ${
      isValid
        ? "bg-green-50 border-green-200 text-green-700"
        : "bg-red-50 border-red-200 text-red-700"
    }`}
  >
    {isValid ? (
      <CheckCircle className="w-4 h-4" />
    ) : (
      <XCircle className="w-4 h-4" />
    )}
    <span className="text-sm font-Vazirmatn-medium">{label}</span>
  </div>
);

function SensorStatus({ uartValid, analogValid }) {
  return (
    <div className="flex flex-col sm:flex-row gap-6">
      <div className="flex-1">
        <p className="text-sm font-Vazirmatn-medium text-gray-600 mb-3">
          وضعیت داده‌ها
        </p>
        <div className="flex gap-3">
          <StatusBadge isValid={uartValid} label="داده UART" />
          <StatusBadge isValid={analogValid} label="داده آنالوگ" />
        </div>
      </div>

      <div className="flex-1">
        <p className="text-sm font-Vazirmatn-medium text-gray-600 mb-3">
          وضعیت سنسور
        </p>
        <div className="flex gap-3">
          <div className="flex items-center gap-2 px-3 py-2 bg-green-50 border border-green-200 rounded-lg text-green-700">
            <CheckCircle className="w-4 h-4" />
            <span className="text-sm font-Vazirmatn-medium">فعال</span>
          </div>
          <div className="flex items-center gap-2 px-3 py-2 bg-blue-50 border border-blue-200 rounded-lg text-blue-700">
            <Activity className="w-4 h-4" />
            <span className="text-sm font-Vazirmatn-medium">آنلاین</span>
          </div>
        </div>
      </div>
    </div>
  );
}

export default SensorStatus;
