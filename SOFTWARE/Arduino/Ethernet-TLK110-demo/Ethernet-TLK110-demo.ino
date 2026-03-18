//Ethernet demo, based on the original demo from the Espressif package
//Edited for Olimex ESP32-P4-DevKit-Lipo
//Uses HTTPs instead of HTTP and points to the olimex web-site

#include <ETH.h>
#include <WiFiClientSecure.h>

#ifndef ETH_PHY_MDC
#define ETH_PHY_TYPE ETH_PHY_TLK110
#if CONFIG_IDF_TARGET_ESP32
#define ETH_PHY_ADDR  31
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER 17
#define ETH_CLK_MODE  ETH_CLOCK_GPIO0_IN
#elif CONFIG_IDF_TARGET_ESP32P4
#define ETH_PHY_ADDR  1
#define ETH_PHY_MDC   31
#define ETH_PHY_MDIO  52
#define ETH_PHY_POWER 51
#define ETH_CLK_MODE  EMAC_CLK_EXT_IN
#endif
#endif

#define LED_PIN 2

static bool eth_connected = false;
static bool eth_has_ip = false;

// timing
unsigned long lastBlink = 0;
bool ledState = false;

// HTTPS client
WiFiClientSecure client;

// ================= EVENT HANDLER =================
void onEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      ETH.setHostname("esp32-ethernet");
      eth_connected = false;
      eth_has_ip = false;
      break;

    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      eth_connected = true;
      break;

    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.println("ETH Got IP");
      Serial.println(ETH);
      eth_has_ip = true;
      break;

    case ARDUINO_EVENT_ETH_LOST_IP:
      Serial.println("ETH Lost IP");
      eth_has_ip = false;
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      eth_has_ip = false;
      break;

    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      eth_has_ip = false;
      break;

    default:
      break;
  }
}

// ================= LED STATUS =================
void updateLED() {
  unsigned long now = millis();

  if (!eth_connected) {
    // fast blink (100ms)
    if (now - lastBlink > 100) {
      lastBlink = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }
  else if (eth_connected && !eth_has_ip) {
    // slow blink (500ms)
    if (now - lastBlink > 500) {
      lastBlink = now;
      ledState = !ledState;
      digitalWrite(LED_PIN, ledState);
    }
  }
  else if (eth_has_ip) {
    // solid ON
    digitalWrite(LED_PIN, HIGH);
  }
}

// ================= HTTPS TEST =================
void testHTTPS(const char *host) {
  Serial.print("\n[HTTPS] Connecting to ");
  Serial.println(host);

  client.setInsecure();  // skip cert validation (OK for testing)

  if (!client.connect(host, 443)) {
    Serial.println("[HTTPS] Connection failed");
    return;
  }

  Serial.println("[HTTPS] Connected");

  client.printf(
    "GET / HTTP/1.1\r\n"
    "Host: %s\r\n"
    "User-Agent: ESP32-P4\r\n"
    "Connection: close\r\n\r\n",
    host
  );

  // indicate activity
  digitalWrite(LED_PIN, HIGH);

  // wait for response
  unsigned long timeout = millis();
  while (client.connected() && !client.available()) {
    if (millis() - timeout > 5000) {
      Serial.println("[HTTPS] Timeout");
      client.stop();
      return;
    }
  }

  // read response
  while (client.available()) {
    Serial.write(client.read());
  }

  Serial.println("\n[HTTPS] Done, closing connection");
  client.stop();

  // short OFF pulse to indicate cycle complete
  digitalWrite(LED_PIN, LOW);
  delay(100);
  digitalWrite(LED_PIN, HIGH);
}

// ================= SETUP =================
void setup() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.begin(115200);

  Network.onEvent(onEvent);

  ETH.begin(
    ETH_PHY_TYPE,
    ETH_PHY_ADDR,
    ETH_PHY_MDC,
    ETH_PHY_MDIO,
    ETH_PHY_POWER,
    ETH_CLK_MODE
  );
}

// ================= LOOP =================
void loop() {
  updateLED();

  static unsigned long lastRequest = 0;

  if (eth_has_ip && millis() - lastRequest > 10000) {
    lastRequest = millis();

    // HTTPS request (no more 301 redirect)
    testHTTPS("www.olimex.com");
  }
}
