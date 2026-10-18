#include <Arduino.h>

#include "web/handler.hpp"
#include "web/helpers.hpp"
#include "web/api.hpp"
#include "ac/controller.hpp"
#include "secrets.hpp"

WebServerT web_server(80);

static void connect_to_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < WEB_WIFI_STOP_RETRY_AFTER) {
    delay(500);
    Serial.print(".");
  }

  if(WiFi.status() == WL_CONNECTED) {
    Serial.print("Got IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("Failed to connect to WiFi, retrying in background...");
  }
}

void web_setup() {
  connect_to_wifi();

#ifdef ESP32
  const char *APIHeaders[] = { "X-Api-Key" };
  web_server.collectHeaders(APIHeaders, 1);
#elif defined(ESP8266)
  web_server.collectHeaders("X-Api-Key");
#endif

  web_api_setup();
  web_server.begin();
}

void web_loop() {
  web_server.handleClient();
}