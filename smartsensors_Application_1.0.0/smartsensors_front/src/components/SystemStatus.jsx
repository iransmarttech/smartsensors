import React from "react";

function SystemStatus() {
  return (
    <div className="bg-primary/5 rounded-xl border border-primary/10 p-4">
      <div className="text-center">
        <p className="text-sm text-gray-600 font-Vazirmatn">
          آخرین بروزرسانی: {new Date().toLocaleTimeString("fa-IR")}
        </p>
        <p className="text-xs text-gray-500 mt-1 font-Vazirmatn">
          تمام سنسورها در حالت فعال و آنلاین هستند
        </p>
      </div>
    </div>
  );
}

export default SystemStatus;
