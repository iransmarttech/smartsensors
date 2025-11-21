import React from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
} from "recharts";

function RealTimeChart({ data, title }) {
  return (
    <div className=" p-6">
      <h3 className="text-sm font-Vazirmatn-bold text-white mb-3">{title}</h3>
      <div className="h-32">
        <ResponsiveContainer width="100%" height="100%">
          <LineChart data={data}>
            <CartesianGrid strokeDasharray="3 3" stroke="#" />
            <XAxis
              dataKey="time"
              tick={{ fontSize: 12 }}
              tickFormatter={(value) =>
                new Date(value).toLocaleTimeString("fa-IR", {
                  minute: "2-digit",
                  second: "2-digit",
                })
              }
            />
            <YAxis tick={{ fontSize: 12 }} />
            <Tooltip
              labelFormatter={(value) =>
                `زمان: ${new Date(value).toLocaleTimeString("fa-IR")}`
              }
              formatter={(value) => [value, "مقدار"]}
            />
            <Line
              type="monotone"
              dataKey="value"
              stroke="#59ac77"
              strokeWidth={1.5}
              dot={true}
              isAnimationActive={true}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );
}

export default RealTimeChart;
