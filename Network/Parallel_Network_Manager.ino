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
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// Global Variables
bool ethConnected = false;
bool wifiConnected = false;

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
    Serial.println("[C0] Ethernet connection failed");
    ethConnected = false;
  }

  // Serve Ethernet requests
  while (true) {
    if (ethConnected) {
      EthernetClient client = ethServer.available();
      if (client) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Hello from Ethernet!");
        client.stop();
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

// WiFi Task (Core 1)
void wifiTask(void *pvParameters) {
  Serial.println("[C1] Initializing WiFi...");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  
  // Wait for connection
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("\n[C1] WiFi connected. IP: ");
    Serial.println(WiFi.localIP());
    wifiConnected = true;
    wifiServer.begin();
  } else {
    Serial.println("\n[C1] WiFi connection failed");
    wifiConnected = false;
  }

  // Serve WiFi requests
  while (true) {
    if (wifiConnected) {
      WiFiClient client = wifiServer.available();
      if (client) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/plain");
        client.println("Connection: close");
        client.println();
        client.println("Hello from WiFi!");
        client.stop();
      }
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Dual Network ESP32 - Starting both Ethernet and WiFi");

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
  // Display connection status periodically
  static unsigned long lastStatusTime = 0;
  if (millis() - lastStatusTime > 10000) {
    Serial.println("\n=== Network Status ===");
    Serial.print("Ethernet: ");
    Serial.println(ethConnected ? "Connected" : "Disconnected");
    Serial.print("WiFi: ");
    Serial.println(wifiConnected ? "Connected" : "Disconnected");
    Serial.println("====================");
    lastStatusTime = millis();
  }
  
  delay(1000);
}