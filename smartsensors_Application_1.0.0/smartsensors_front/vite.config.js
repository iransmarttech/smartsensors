import { defineConfig } from "vite";
import react from "@vitejs/plugin-react-swc";
import tailwindcss from "@tailwindcss/vite";

// https://vite.dev/config/
export default defineConfig({
  plugins: [react(), tailwindcss()],
  server: {
    host: process.env.HOST || "0.0.0.0",
    port: Number(process.env.PORT) || 5173,
    hmr: {
      // When serving on LAN, set host to the machine IP or rely on the client
      // to connect back. Using 0.0.0.0 for host and leaving client to the
      // correct origin usually works; expose env override for tricky networks.
      host: process.env.HMR_HOST || undefined,
    },
  },
});
