// import React from "react";
// import {
//   LineChart,
//   Line,
//   XAxis,
//   YAxis,
//   CartesianGrid,
//   Tooltip,
//   ResponsiveContainer,
// } from "recharts";

// function RealTimeChart({ data, title }) {
//   return (
//     <div className=" p-6">
//       <h3 className="text-sm font-Vazirmatn-bold text-white mb-3">{title}</h3>
//       <div className="h-32">
//         <ResponsiveContainer width="100%" height="100%">
//           <LineChart data={data}>
//             <CartesianGrid strokeDasharray="3 3" stroke="#" />
//             <XAxis
//               dataKey="time"
//               tick={{ fontSize: 12 }}
//               tickFormatter={(value) => value} // زمان مستقیماً از بک‌اند می‌آید
//             />
//             <YAxis tick={{ fontSize: 12 }} />
//             <Tooltip
//               labelFormatter={(value) => `زمان: ${value}`}
//               formatter={(value) => [value, "مقدار"]}
//             />
//             <Line
//               type="monotone"
//               dataKey="value"
//               stroke="#59ac77"
//               strokeWidth={1.7}
//               dot={true}
//               isAnimationActive={false}
//             />
//           </LineChart>
//         </ResponsiveContainer>
//       </div>
//     </div>
//   );
// }

// export default RealTimeChart;








import React from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Area,
  ReferenceLine,
  ReferenceArea,
  Label,
} from "recharts";

function RealTimeChart({ data, title }) {
  // محاسبه حداقل و حداکثر برای رنگ‌بندی بهتر
  const values = data.map(item => item.value);
  const minValue = Math.min(...values);
  const maxValue = Math.max(...values);
  
  // رنگ‌های گرادیان برای Area
  const gradientId = `gradient-${title.replace(/\s+/g, '-')}`;
  
  // اگر دیتا خالی بود، یک پیام نمایش بده
  if (!data || data.length === 0) {
    return (
      <div className="p-6">
        <h3 className="text-sm font-Vazirmatn-bold text-white mb-3">{title}</h3>
        <div className="h-32 flex items-center justify-center bg-box rounded-lg">
          <p className="text-gray-400 font-Vazirmatn text-sm">
            داده‌ای برای نمایش وجود ندارد
          </p>
        </div>
      </div>
    );
  }

  return (
    <div className="p-6">
      <h3 className="text-sm font-Vazirmatn-bold text-white mb-3">{title}</h3>
      <div className="h-32">
        <ResponsiveContainer width="100%" height="100%">
          <LineChart
            data={data}
            margin={{ top: 10, right: 10, left: 0, bottom: 10 }}
          >
            <defs>
              <linearGradient id={gradientId} x1="0" y1="0" x2="0" y2="1">
                <stop offset="5%" stopColor="#59ac77" stopOpacity={0.8}/>
                <stop offset="95%" stopColor="#59ac77" stopOpacity={0}/>
              </linearGradient>
            </defs>
            
            <CartesianGrid 
              strokeDasharray="3 3" 
              stroke="#2d3748" 
              vertical={false}
            />
            
            <XAxis
              dataKey="time"
              tick={{ fontSize: 10, fill: "#a0aec0" }}
              axisLine={{ stroke: "#4a5568" }}
              tickLine={{ stroke: "#4a5568" }}
              tickFormatter={(value) => {
                // فقط ساعت و دقیقه نشان بده اگر زمان کامل است
                if (value && value.includes(':')) {
                  const parts = value.split(':');
                  if (parts.length >= 2) {
                    return `${parts[0]}:${parts[1]}`;
                  }
                }
                return value;
              }}
            />
            
            <YAxis
              tick={{ fontSize: 10, fill: "#a0aec0" }}
              axisLine={{ stroke: "#4a5568" }}
              tickLine={{ stroke: "#4a5568" }}
              domain={['auto', 'auto']}
              tickFormatter={(value) => {
                // اعداد بزرگ را خلاصه کن (مثلا 1000 -> 1k)
                if (value >= 1000) return `${(value/1000).toFixed(1)}k`;
                if (value >= 100) return value.toFixed(0);
                return value.toFixed(1);
              }}
            />
            
            <Tooltip
              contentStyle={{
                backgroundColor: '#2d3748',
                border: '1px solid #4a5568',
                borderRadius: '6px',
                color: '#fff',
                fontFamily: 'Vazirmatn, sans-serif',
              }}
              labelStyle={{ color: '#cbd5e0', marginBottom: '5px' }}
              itemStyle={{ color: '#68d391' }}
              labelFormatter={(value) => (
                <div className="font-Vazirmatn text-xs">
                  ⏰ زمان: {value}
                </div>
              )}
              formatter={(value, name) => [
                <span className="font-bold text-green-300" key="value">
                  {Number(value).toLocaleString('fa-IR')}
                </span>,
                <span className="text-gray-300 font-Vazirmatn text-xs" key="name">
                  مقدار
                </span>
              ]}
              cursor={{ stroke: '#4a5568', strokeWidth: 1 }}
            />
            
            {/* Area با گرادیان زیر خط */}
            <Area
              type="monotone"
              dataKey="value"
              stroke="none"
              fill={`url(#${gradientId})`}
              fillOpacity={0.3}
            />
            
            {/* خط اصلی */}
            <Line
              type="monotone"
              dataKey="value"
              stroke="#59ac77"
              strokeWidth={2}
              dot={{
                stroke: '#59ac77',
                strokeWidth: 2,
                fill: '#1a202c',
                r: 4,
              }}
              activeDot={{
                stroke: '#68d391',
                strokeWidth: 3,
                fill: '#1a202c',
                r: 6,
              }}
              // isAnimationActive={true}
              // animationDuration={100}
              // animationEasing="ease-in-out"
            />
            
            {/* خط میانگین (اختیاری) */}
            {data.length > 2 && (
              <ReferenceLine
                y={values.reduce((a, b) => a + b, 0) / values.length}
                stroke="#ed8936"
                strokeWidth={1}
                strokeDasharray="3 3"
                label={{
                  value: '',
                  position: 'insideTopRight',
                  fill: '#ed8936',
                  fontSize: 10,
                }}
              />
            )}
            
            {/* Highlight نقاط بالا (اختیاری) */}
            {maxValue > (values.reduce((a, b) => a + b, 0) / values.length) * 1.5 && (
              <ReferenceArea
                y1={values.reduce((a, b) => a + b, 0) / values.length * 1.5}
                y2={maxValue}
                fill="#fc8181"
                fillOpacity={0.1}
                stroke="none"
              />
            )}
          </LineChart>
        </ResponsiveContainer>
      </div>
      
      {/* اطلاعات اضافی زیر نمودار */}
      <div className="flex justify-between items-center mt-2 text-xs text-gray-400 font-Vazirmatn">
        <div className="flex items-center">
          <div className="w-3 h-3 rounded-full bg-[#59ac77] mr-1 ml-2"></div>
          <span>مقدار فعلی: {data[data.length - 1]?.value.toFixed(1) || 0}</span>
        </div>
        <div className="text-left">
          <div className="flex items-center">
            <div className="w-3 h-0.5 bg-[#ed8936] mr-1 ml-2"></div>
            <span>میانگین: {(values.reduce((a, b) => a + b, 0) / values.length).toFixed(1)}</span>
          </div>
        </div>
      </div>
    </div>
  );
}

export default React.memo(RealTimeChart);