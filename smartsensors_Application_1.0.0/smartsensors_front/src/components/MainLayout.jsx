// src/components/layout/MainLayout.jsx
import { Outlet, useLocation } from "react-router-dom";
import Sidebar from "./Sidebar";
import Header from "./Header";
import { motion, AnimatePresence } from "framer-motion";

export default function MainLayout() {
  const location = useLocation();
  return (
    <div className="flex h-screen bg-primary">
      {/* منوی سمت راست - ثابت */}
      <Sidebar />

      {/* محتوای سمت چپ */}
      <div className="flex-1 flex flex-col">
        {/* هدر */}
        <Header />

        {/* محتوای صفحات - با Outlet تغییر می‌کنه */}
        <main className="flex-1 p-4 mobile:p-6 overflow-auto">
          <AnimatePresence mode="wait">
            <motion.div
              key={location.pathname}
              initial={{ opacity: 0, y: 20 }}
              animate={{ opacity: 1, y: 0 }}
              transition={{ duration: 1.5 }}
              className="h-full"
            >
              <Outlet />
            </motion.div>
          </AnimatePresence>
        </main>
      </div>
    </div>
  );
}
