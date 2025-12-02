import React from "react";
import { Leaf, Calendar } from "lucide-react";

function Header() {
  return (
    <header className="bg-primary text-white shadow-lg ">
      <div className="container mx-auto px-4 py-4">
        <div className="flex items-center justify-center md:justify-around lg:justify-between ">
          {/* قسمت سمت راست - لوگو و عنوان */}
          <div className="flex items-center lg:pr-15 xl:pr-0">
            <div className="bg-white p-2 rounded-full ml-4 hidden sm:inline">
              <img src="./Images/logo.png" alt="Logo" className="w-12 h-12" />
            </div>
            <div className="text-right pr-10 mobile:pr-0">
              <h1 className="text-xl font-Vazirmatn-bold">
                سامانه پایش محیط زیست
              </h1>
              <p className="text-sm text-gray-300 font-Vazirmatn mt-1">
                شرکت هوشمند فناوران برتر ایرانیان
              </p>
            </div>
          </div>

          {/* قسمت سمت چپ - تاریخ */}
          <div className="hidden md:flex items-center gap-4">
            <div className="flex items-center text-sm font-Vazirmatn-medium bg-primary-dark px-3 py-2 rounded-lg">
              <Calendar className="ml-2 w-4 h-4" />
              {new Date().toLocaleDateString("fa-IR")}
            </div>
          </div>
        </div>
      </div>
    </header>
  );
}

export default Header;
