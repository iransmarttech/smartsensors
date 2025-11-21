// src/App.jsx
import React from "react";
import { BrowserRouter as Router, Routes, Route } from "react-router-dom";
import MainLayout from "./components/MainLayout";
import AirQualityDashboard from "./pages/AirQualityDashboard";
import GasSensorsPanel from "./pages/GasSensorsPanel";
import DeviceInfoPanel from "./pages/DeviceInfoPanel";

function App() {
  return (
    <Router>
      <Routes>
        <Route path="/" element={<MainLayout />}>
          <Route index element={<AirQualityDashboard />} />
          <Route path="air-quality" element={<AirQualityDashboard />} />
          <Route path="gas-sensors" element={<GasSensorsPanel />} />
          <Route path="device-info" element={<DeviceInfoPanel />} />
        </Route>
      </Routes>
    </Router>
  );
}

export default App;
