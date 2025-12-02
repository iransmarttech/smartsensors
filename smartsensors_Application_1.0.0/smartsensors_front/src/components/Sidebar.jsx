// src/components/layout/Sidebar.jsx
import React, { useState, useEffect } from "react";
import { Link, useLocation } from "react-router-dom";
import { Wind, Search, Monitor, Satellite, Menu, X , LayoutDashboard } from "lucide-react";

function Sidebar() {
  const [isMobileMenuOpen, setIsMobileMenuOpen] = useState(false);
  const location = useLocation();

  const tabs = [
    {
      id: "dashboard",
      label: "داشبورد",
      icon: <LayoutDashboard className="w-5 h-5" />,
      path: "/dashboard",
    },
    {
      id: "air-quality",
      label: "کیفیت هوا",
      icon: <Wind className="w-5 h-5" />,
      path: "/air-quality",
    },
    {
      id: "gas-sensors",
      label: "سنسورهای گاز",
      icon: <Search className="w-5 h-5" />,
      path: "/gas-sensors",
    },
    {
      id: "device-info",
      label: "اطلاعات دستگاه",
      icon: <Monitor className="w-5 h-5" />,
      path: "/device-info",
    },

  ];

  // بستن منوی موبایل با کلید Esc
  useEffect(() => {
    const handleEsc = (event) => {
      if (event.keyCode === 27) {
        setIsMobileMenuOpen(false);
      }
    };
    window.addEventListener("keydown", handleEsc);

    return () => {
      window.removeEventListener("keydown", handleEsc);
    };
  }, []);

  // بستن منوی موبایل هنگام تغییر مسیر
  useEffect(() => {
    setIsMobileMenuOpen(false);
  }, [location.pathname]);

  const toggleMobileMenu = () => {
    setIsMobileMenuOpen(!isMobileMenuOpen);
  };

  const isActiveTab = (path) => {
    return location.pathname === path;
  };

  const renderMenuItems = () => {
    return tabs.map((tab) => (
      <li key={tab.id}>
        <Link
          to={tab.path}
          className={`w-full text-right px-4 py-3 rounded-lg transition-all duration-200 flex items-center font-Vazirmatn-medium ${
            isActiveTab(tab.path)
              ? "bg-secondary/20 text-white border-r-4 border-green-400 shadow-lg backdrop-blur-sm"
              : "text-gray-300 hover:bg-secondary/10 hover:text-white hover:border-r-2 hover:border-secondary/50"
          }`}
        >
          <span className="ml-3 shrink-0">{tab.icon}</span>
          <span className="font-Vazirmatn-medium">{tab.label}</span>
        </Link>
      </li>
    ));
  };

  return (
    <>
      {/* دسکتاپ */}
      <div className="hidden xl:flex w-72 bg-primary  shadow-2xl flex-col h-screen sticky top-0 z-40 font-Vazirmatn">
        {/* هدر سایدبار */}
        <div className="p-4 border-b border-secondary">
          <p className="font-Vazirmatn-bold text-white pt-2 pr-1">منو سامانه</p>
        </div>

        {/* منوی اصلی */}
        <nav className="flex-1 p-4">
          <ul className="space-y-2">{renderMenuItems()}</ul>
        </nav>

        {/* فوتر سایدبار */}
        <div className="p-4 mt-auto border-t border-secondary/30 bg-primary/80 backdrop-blur-sm">
          <div className="text-center">
            <p className="text-gray-400 text-xs font-Vazirmatn">نسخه ۱.۰.۰</p>
            <p className="text-gray-500 text-[10px] mt-1 font-Vazirmatn">
              سیستم مانیتورینگ هوشمند
            </p>
          </div>
        </div>
      </div>

      {/* موبایل و تبلت */}
      <div className="xl:hidden font-Vazirmatn">
        {/* دکمه منو در موبایل */}
        <button
          onClick={toggleMobileMenu}
          className="fixed top-4 right-4 z-50 w-12 h-12 bg-secondary   text-white rounded-xl shadow-lg flex items-center justify-center hover:from-secondary hover:to-secondary transition-all duration-300 border border-secondary/30 backdrop-blur-sm"
          aria-label="باز کردن منو"
        >
          <Menu className="w-6 h-6" />
        </button>

        {/* منوی موبایل */}
        <div
          className={`fixed inset-y-0 right-0 w-80 max-w-full bg-primary  z-50 transform transition-transform duration-300 ease-in-out flex flex-col ${
            isMobileMenuOpen ? "translate-x-0" : "translate-x-full"
          }`}
        >
          {/* هدر منوی موبایل */}
          <div className="p-4 border-b border-secondary/50 flex justify-between items-center">
            <div className="flex items-center gap-3">
              {/* <div className="w-10 h-10 bg-white rounded-full flex items-center justify-center border-2 border-secondary"> */}
              {/* <Satellite className="w-5 h-5 text-green-400" /> */}
              {/* <img src="./Images/logo.png" alt="Logo" /> */}
              {/* </div> */}
              <div>
                <h2 className="text-white font-Vazirmatn-bold text-lg pr-1 pt-2">
                  منو سامانه
                </h2>
              </div>
            </div>
            <button
              onClick={toggleMobileMenu}
              className="w-10 h-10 bg-secondary/20 text-white rounded-lg flex items-center justify-center hover:bg-secondary/30 transition-all duration-200 border border-secondary/30"
              aria-label="بستن منو"
            >
              <X className="w-5 h-5" />
            </button>
          </div>

          {/* محتوای منو */}
          <nav className="flex-1 overflow-y-auto p-4">
            <ul className="space-y-2">{renderMenuItems()}</ul>
          </nav>

          {/* فوتر */}
          <div className="p-4 border-t border-secondary/30 bg-primary/80 backdrop-blur-sm">
            <div className="text-center">
              <p className="text-gray-400 text-xs font-Vazirmatn">نسخه ۱.۰.۰</p>
              <p className="text-gray-500 text-[10px] mt-1 font-Vazirmatn">
                سیستم مانیتورینگ هوشمند
              </p>
            </div>
          </div>
        </div>

        {/* Overlay */}
        {isMobileMenuOpen && (
          <div
            className="fixed inset-0 bg-black/60 z-40  backdrop-blur-xs"
            onClick={toggleMobileMenu}
          />
        )}
      </div>
    </>
  );
}

export default Sidebar;
