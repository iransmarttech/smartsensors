/**
 * Frontend Logging Utility
 * Logs events locally and sends critical logs to Django backend
 */

import { getLogUrl, LOGGING_CONFIG } from '../config';

const API_LOG_URL = getLogUrl();

class Logger {
  constructor() {
    this.enabled = LOGGING_CONFIG.ENABLED;
    this.sendToBackend = LOGGING_CONFIG.SEND_TO_BACKEND;
    this.sendOnlyErrors = LOGGING_CONFIG.SEND_ONLY_ERRORS;
    this.logBuffer = [];
    this.maxBufferSize = LOGGING_CONFIG.MAX_LOCAL_LOGS;
  }

  /**
   * Log info message
   */
  info(component, message, data = null) {
    this._log("info", component, message, data);
    console.log(`[INFO] [${component}]`, message, data || "");
  }

  /**
   * Log warning message
   */
  warn(component, message, data = null) {
    this._log("warning", component, message, data);
    console.warn(`[WARN] [${component}]`, message, data || "");
  }

  /**
   * Log error message
   */
  error(component, message, error = null) {
    const errorStack = error?.stack || null;
    this._log("error", component, message, { error: error?.message, stack: errorStack });
    console.error(`[ERROR] [${component}]`, message, error || "");
    
    // Always send errors to backend
    if (this.sendToBackend) {
      this._sendToBackend("error", component, message, errorStack);
    }
  }

  /**
   * Log API call
   */
  logAPICall(endpoint, method, status, responseTime) {
    const message = `API ${method} ${endpoint} - Status: ${status} - Time: ${responseTime}ms`;
    this._log("info", "API", message, { endpoint, method, status, responseTime });
    
    // Log errors to backend
    if (status >= 400 && this.sendToBackend) {
      this._sendToBackend("warning", "API", message, null);
    }
  }

  /**
   * Log user action
   */
  logAction(component, action, details = null) {
    const message = `User action: ${action}`;
    this._log("info", component, message, details);
    console.log(`[ACTION] [${component}]`, action, details || "");
  }

  /**
   * Internal logging method
   */
  _log(level, component, message, data) {
    if (!this.enabled) return;

    const logEntry = {
      timestamp: new Date().toISOString(),
      level,
      component,
      message,
      data,
      url: window.location.href,
      userAgent: navigator.userAgent,
    };

    // Add to buffer
    this.logBuffer.push(logEntry);

    // Maintain buffer size
    if (this.logBuffer.length > this.maxBufferSize) {
      this.logBuffer.shift();
    }

    // Store in localStorage for persistence
    try {
      localStorage.setItem("app_logs", JSON.stringify(this.logBuffer));
    } catch (e) {
      console.warn("Failed to store logs in localStorage", e);
    }
  }

  /**
   * Send log to Django backend
   */
  async _sendToBackend(level, component, message, error_stack) {
    // Only send if enabled
    if (!this.sendToBackend) return;
    
    // Only send errors if configured
    if (this.sendOnlyErrors && level !== 'error') return;
    
    try {
      await fetch(API_LOG_URL, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({
          level,
          component,
          message,
          error_stack,
          url: window.location.href,
        }),
      });
    } catch (e) {
      // Silently fail - don't want logging to break the app
      console.warn("Failed to send log to backend", e);
    }
  }

  /**
   * Get all logs from buffer
   */
  getLogs(level = null) {
    if (!level) return this.logBuffer;
    return this.logBuffer.filter((log) => log.level === level);
  }

  /**
   * Clear log buffer
   */
  clearLogs() {
    this.logBuffer = [];
    try {
      localStorage.removeItem("app_logs");
    } catch (e) {
      console.warn("Failed to clear logs from localStorage", e);
    }
  }

  /**
   * Export logs as JSON
   */
  exportLogs() {
    const dataStr = JSON.stringify(this.logBuffer, null, 2);
    const dataBlob = new Blob([dataStr], { type: "application/json" });
    const url = URL.createObjectURL(dataBlob);
    const link = document.createElement("a");
    link.href = url;
    link.download = `logs_${new Date().toISOString()}.json`;
    link.click();
    URL.revokeObjectURL(url);
  }

  /**
   * Load logs from localStorage
   */
  loadLogs() {
    try {
      const stored = localStorage.getItem("app_logs");
      if (stored) {
        this.logBuffer = JSON.parse(stored);
      }
    } catch (e) {
      console.warn("Failed to load logs from localStorage", e);
    }
  }
}

// Create singleton instance
const logger = new Logger();
logger.loadLogs();

// Global error handler
window.addEventListener("error", (event) => {
  logger.error("Global", "Unhandled error", {
    message: event.message,
    filename: event.filename,
    lineno: event.lineno,
    colno: event.colno,
    error: event.error,
  });
});

// Promise rejection handler
window.addEventListener("unhandledrejection", (event) => {
  logger.error("Global", "Unhandled promise rejection", {
    reason: event.reason,
  });
});

export default logger;
