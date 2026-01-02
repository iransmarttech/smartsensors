import React from "react";
import { 
  Globe,
  Cpu, 
  Shield, 
  Cloud,
  Wind, 
  Flame,
  AlertTriangle,
  Eye,
  Zap,
  Factory,
  Users
} from "lucide-react";

function Dashboard() {
  return (
    <div className="p-6 font-Vazirmatn">
      {/* هدر */}
      <div className="bg-box rounded-lg shadow-lg p-6 mb-8">
        <div className="flex items-center gap-4 mb-6">
          <div className="text-right">
            <h1 className="text-2xl font-Vazirmatn-bold text-white mb-2">
              شرکت هوشمند فناوران برتر ایرانیان
            </h1>
            <p className="text-secondary font-Vazirmatn">
              شماره ثبت: ۵۰۴۷
            </p>
          </div>
        </div>

        <div className="p-4 bg-fildes rounded-lg">
          <p className="text-white text-justify leading-relaxed font-Vazirmatn">
            شرکت هوشمند فناوران برتر ایرانیان با شماره ثبت ۵۰۴۷، فعال در حوزه‌های تخصصی 
            <span className="text-secondary font-Vazirmatn-semibold"> اینترنت اشیاء (IoT)</span> و 
            <span className="text-secondary font-Vazirmatn-semibold"> هوش مصنوعی (AI)</span> می‌باشد.
            بخشی از فعالیت‌های این شرکت به پروژه‌های محیط زیستی و صنعتی اختصاص یافته است. 
            این شرکت با بهره‌گیری از فناوری‌های روز دنیا و تیم متخصص خود، در زمینه طراحی و پیاده‌سازی 
            سیستم‌های پایش هوشمند محیطی فعالیت می‌نماید. تمرکز اصلی بر ارائه راه‌حل‌های نوین 
            در حوزه سنجش کیفیت هوا، مانیتورینگ صنعتی و سیستم‌های امنیتی مبتنی بر سنسورهای پیشرفته است.
          </p>
        </div>
      </div>

      {/* بخش فیلم و سنسورها کنار هم */}
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-6 mb-8">
        {/* سنسورها - سمت چپ */}
        <div className="bg-box rounded-lg shadow-lg p-6 order-2 lg:order-1">
          <h2 className="text-xl font-Vazirmatn-bold text-white mb-6 flex items-center">
            <Factory className="ml-2 w-6 h-6 text-secondary" />
            سنسورهای استفاده شده
          </h2>
          
          <div className="space-y-4">
            {/* سنسور 1 */}
            <div className="flex items-center p-4 bg-fildes rounded-lg">
              <div className="ml-3 p-3 bg-blue-500/10 rounded-lg">
                <Wind className="w-5 h-5 text-blue-400" />
              </div>
              <div className="text-right flex-1">
                <p className="text-sm text-white font-Vazirmatn">سنسور ذرات معلق </p>
                <p className="font-Vazirmatn-semibold text-white text-sm">
                  اندازه‌گیری PM1، PM2.5، PM10 - سنجش آلودگی هوا
                </p>
              </div>
            </div>

            {/* سنسور 2 */}
            <div className="flex items-center p-4 bg-fildes rounded-lg">
              <div className="ml-3 p-3 bg-red-500/10 rounded-lg">
                <Flame className="w-5 h-5 text-red-400" />
              </div>
              <div className="text-right flex-1">
                <p className="text-sm text-white font-Vazirmatn">سنسور گاز قابل اشتعال </p>
                <p className="font-Vazirmatn-semibold text-white text-sm">
                  تشخیص گازهای قابل اشتعال - ایمنی صنعتی
                </p>
              </div>
            </div>

            {/* سنسور 3 */}
            <div className="flex items-center p-4 bg-fildes rounded-lg">
              <div className="ml-3 p-3 bg-yellow-500/10 rounded-lg">
                <AlertTriangle className="w-5 h-5 text-yellow-400" />
              </div>
              <div className="text-right flex-1">
                <p className="text-sm text-white font-Vazirmatn">سنسور دی اکسید گوگرد </p>
                <p className="font-Vazirmatn-semibold text-white text-sm">
                  اندازه‌گیری SO₂ - پایش آلاینده‌های صنعتی
                </p>
              </div>
            </div>

            {/* سنسور 4 */}
            <div className="flex items-center p-4 bg-fildes rounded-lg">
              <div className="ml-3 p-3 bg-green-500/10 rounded-lg">
                <Eye className="w-5 h-5 text-green-400" />
              </div>
              <div className="text-right flex-1">
                <p className="text-sm text-white font-Vazirmatn">سنسور ترکیبات آلی </p>
                <p className="font-Vazirmatn-semibold text-white text-sm">
                  اندازه‌گیری TVOC - سنجش کیفیت هوای داخلی
                </p>
              </div>
            </div>
          </div>
        </div>

        {/* فیلم - سمت راست */}
        <div className="bg-box rounded-lg shadow-lg p-6 order-1 lg:order-2">
        <h2 className="text-xl font-Vazirmatn-bold text-white mb-6 flex items-center">
        <Zap className="ml-2 w-6 h-6 text-secondary" />
         معرفی برد سامانه پایش محیط زیست
       </h2>
  
  {/* بخش فیلم با قابلیت پخش خودکار و لایه شیشه‌ای */}
  <div className="aspect-video rounded-lg overflow-hidden relative">
    {/* لایه شیشه‌ای سیاه با شفافیت */}
    <div className="absolute inset-0 bg-black/30 z-10 pointer-events-none"></div>
    
    <video 
      className="w-full h-full object-cover"
      autoPlay
      muted
      loop
      playsInline
      controlsList="nofullscreen nodownload"
      disablePictureInPicture
    >
      <source src="/videos/sensors.mp4" type="video/mp4" />
      <source src="/videos/sensors.webm" type="video/webm" />
      مرورگر شما از پخش ویدیو پشتیبانی نمی‌کند.
    </video>
  </div>
  
  <div className="mt-4 text-center">
    <p className="text-gray-400 text-sm font-Vazirmatn">
      معرفی برد الکترونیکی و سنسورهای نصب شده
    </p>
  </div>
</div>
      </div>

      {/* قابلیت‌های سیستم */}
      <div className="bg-box rounded-lg shadow-lg p-6">
        <h2 className="text-xl font-Vazirmatn-bold text-white mb-6 flex items-center">
          <Cpu className="ml-2 w-6 h-6 text-secondary" />
          قابلیت‌های سیستم
        </h2>
        
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          <div className="flex items-center p-4 bg-fildes rounded-lg">
            <div className="ml-3">
              <Cloud className="w-5 h-5 text-primary" />
            </div>
            <div className="text-right flex-1">
              <p className="text-sm text-white font-Vazirmatn">دقت بالا در اندازه‌گیری</p>
              <p className="font-Vazirmatn-semibold text-white">
                سنجش دقیق عناصر مختلف
              </p>
            </div>
          </div>

          <div className="flex items-center p-4 bg-fildes rounded-lg">
            <div className="ml-3">
              <Cpu className="w-5 h-5 text-blue-400" />
            </div>
            <div className="text-right flex-1">
              <p className="text-sm text-white font-Vazirmatn">نمایش گرافیکی داده‌ها</p>
              <p className="font-Vazirmatn-semibold text-white">
                نمودارهای تحلیلی زنده
              </p>
            </div>
          </div>

          <div className="flex items-center p-4 bg-fildes rounded-lg">
            <div className="ml-3">
              <Shield className="w-5 h-5 text-green-400" />
            </div>
            <div className="text-right flex-1">
              <p className="text-sm text-white font-Vazirmatn">مانیتورینگ سلامت سنسورها</p>
              <p className="font-Vazirmatn-semibold text-white">
                تشخیص عملکرد صحیح سنسورها
              </p>
            </div>
          </div>

          <div className="flex items-center p-4 bg-fildes rounded-lg">
            <div className="ml-3">
              <Users className="w-5 h-5 text-yellow-400" />
            </div>
            <div className="text-right flex-1">
              <p className="text-sm text-white font-Vazirmatn">پشتیبانی فنی</p>
              <p className="font-Vazirmatn-semibold text-white">
                تیم پشتیبانی تخصصی
              </p>
            </div>
          </div>
        </div>
      </div>

      {/* فوتر */}
      <div className="mt-8 pt-6 border-t border-gray-800">
        <div className="text-center">
          <p className="text-gray-500 text-sm font-Vazirmatn">
            © {new Date().getFullYear()} شرکت هوشمند فناوران برتر ایرانیان - شماره ثبت ۵۰۴۷
          </p>
          <p className="text-gray-600 text-xs mt-1 font-Vazirmatn">
            تخصص در اینترنت اشیاء و هوش مصنوعی
          </p>
        </div>
      </div>
    </div>
  );
}

export default Dashboard;