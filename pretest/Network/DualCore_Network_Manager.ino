#include <WiFi.h>
#include <Ethernet.h>
#include <SPI.h>

// Hardware Configuration
#define ETH_CS_PIN   15
#define ETH_RST_PIN  5
#define ETH_SCK_PIN  14
#define ETH_MISO_PIN 12
#define ETH_MOSI_PIN 13

// Network Credentials
const char* WIFI_SSID = "YourWiFiSSID";
const char* WIFI_PASS = "YourWiFiPassword";
const char* AP_SSID   = "ESP32-Fallback-AP";
const char* AP_PASS   = "password123";

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Global Variables
bool ethConnected = false;
bool wifiConnected = false;
bool apActive = false;

EthernetServer ethServer(80);
WiFiServer wifiServer(80);

// Ethernet Task (Core 0)
void ethernetTask(void *pvParameters) {
  Serial.println("[C0] Initializing Ethernet...");
  
  // Hardware reset
  pinMode(ETH_RST_PIN, OUTPUT);
  digitalWrite(ETH_RST_PIN, HIGH);
  delay(100);
  digitalWrite(ETH_RST_PIN, LOW);
  delay(100);
  digitalWrite(ETH_RST_PIN, HIGH);
  delay(500);

  // Initialize SPI and Ethernet
  SPI.begin(ETH_SCK_PIN, ETH_MISO_PIN, ETH_MOSI_PIN, ETH_CS_PIN);
  Ethernet.init(ETH_CS_PIN);

  // Start Ethernet connection
  if (Ethernet.begin(mac)) {
    Serial.print("[C0] Ethernet connected. IP: ");
    Serial.println(Ethernet.localIP());
    ethConnected = true;
    ethServer.begin();
  } else {
    Serial.println("[C0] Ethernet failed to connect");
    ethConnected = false;
  }

  // Serve requests if Ethernet connected
  while (ethConnected) {
    EthernetClient client = ethServer.available();
    if (client) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Hello World (Ethernet)");
      client.stop();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

// WiFi Task (Core 1)
void wifiTask(void *pvParameters) {
  // Wait for Ethernet outcome
  vTaskDelay(5000 / portTICK_PERIOD_MS);

  if (!ethConnected) {
    Serial.println("[C1] Starting WiFi...");
    
    // Try WiFi connection
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    for (int i = 0; i < 20; i++) {
      if (WiFi.status() == WL_CONNECTED) {
        wifiConnected = true;
        Serial.print("[C1] WiFi connected. IP: ");
        Serial.println(WiFi.localIP());
        wifiServer.begin();
        break;
      }
      vTaskDelay(500 / portTICK_PERIOD_MS);
    }

    // Fallback to AP mode if WiFi failed
    if (!wifiConnected) {
      Serial.println("[C1] WiFi failed. Starting AP...");
      WiFi.softAP(AP_SSID, AP_PASS);
      apActive = true;
      Serial.print("[C1] AP started. IP: ");
      Serial.println(WiFi.softAPIP());
      wifiServer.begin();
    }
  }

  // Serve requests if using WiFi/AP
  while (!ethConnected) {
    WiFiClient client = wifiServer.available();
    if (client) {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/plain");
      client.println("Connection: close");
      client.println();
      client.println("Hello World (WiFi)");
      client.stop();
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
  vTaskDelete(NULL);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Create tasks for both cores
  xTaskCreatePinnedToCore(
    ethernetTask,
    "Eth_Task",
    10000,
    NULL,
    1,
    NULL,
    0
  );

  xTaskCreatePinnedToCore(
    wifiTask,
    "Wifi_Task",
    10000,
    NULL,
    1,
    NULL,
    1
  );
}

void loop() {
  // Empty - tasks run independently
}