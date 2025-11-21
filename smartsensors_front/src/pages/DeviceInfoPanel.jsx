import React from "react";
import { Wifi, Server, Clock, MapPin } from "lucide-react";

function DeviceInfoPanel({ deviceInfo }) {
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
