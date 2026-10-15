#include <Arduino.h>

#include "web_controller.hpp"
#include "ac_controller.hpp"
#include "secrets.hpp"

#ifdef ESP32
#include <WiFi.h>
#include <WebServer.h>
WebServer server(80);
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
#endif

void good_response() {
  server.send(200, "text/plain", "");
}

void error_response(int code, const char *message) {
  server.send(code, "text/plain", message);
}

void not_found() {
  error_response(404, "not found");
}

void update_page() {
  server.send_P(200, "text/html", WEB_UPDATE_PAGE);
}

bool is_rate_limited() {
  static unsigned long last_web_update = 0;
  unsigned long now = millis();
  if(now - last_web_update > WEB_UPDATE_COOLDOWN) {
    last_web_update = now;
    return false;
  }
  return true;
}

bool is_authorized() {
  if(server.hasHeader("X-Api-Key")) return server.header("X-Api-Key") == api_key;
  return false;
}

bool should_respond() {
  if(is_rate_limited()) {
    error_response(429, "too many requests");
    return false;
  }
  if(!is_authorized()) {
    error_response(401, "unauthorized");
    return false;
  }
  return true;
}

void do_command(ACCommand command) {
  bool success = ac_run_command(command);
  if(success)
    good_response();
  else
    error_response(503, "command queue full");
}

int resolve_name(const char * const* names, const char *input) {
  for(int i = 0; i < AC_NAME_COUNT; i++) {
    if(strcasecmp(input, names[i]) == 0) return i;
  }
  return -1;
}

void post_update_api() {
  if(!should_respond()) return;
  int mode = -1;
  int speed = -1;
  int temp = -1;

  if(server.hasArg("mode")) {
    mode = resolve_name(ACModeNames, server.arg("mode").c_str());
    if(mode == -1) return error_response(400, "invalid mode");
  }

  if(server.hasArg("speed")) {
    speed = resolve_name(ACSpeedNames, server.arg("speed").c_str());
    if(speed == -1) return error_response(400, "invalid speed");
  }

  if(server.hasArg("temp")) {
    temp = server.arg("temp").toInt();
    if(temp == 0) return error_response(400, "invalid temp");
    temp = max((int)AC_MIN_TEMP, min(temp, (int)AC_MAX_TEMP));
  }

  // apply speed and temp first, mode last, so an explicit mode wins
  if(speed != -1) ac_set_speed((ACSpeed)speed);
  if(temp != -1)  ac_set_temp((uint8_t)temp);
  if(mode != -1)  ac_set_mode((ACMode)mode);

  do_command(AC_COMMAND_UPDATE);
}

void post_sync() {
  if(!should_respond()) return;
  ac_sync_state(true);
  good_response();
}

void toggle_energy_saver() {
  if(!should_respond()) return;
  do_command(AC_COMMAND_TOGGLE_ENERGY_SAVER);
}

void toggle_off() {
  if(!should_respond()) return;
  do_command(AC_COMMAND_OFF);
}

void get_ac_status() {
  const ACStatus *stat = ac_get_status();
  char status[128];

  snprintf(status, sizeof(status),
    "{\"mode\":\"%s\",\"speed\":\"%s\",\"temp\":%d, \"enabled\":%s,\"energy_saver\":%s}",
    ACModeNames[stat->mode],
    ACSpeedNames[stat->speed],
    stat->temp,
    stat->enabled ? "true" : "false",
    stat->energy_saver ? "true" : "false"
  );

  server.send(200, "application/json", status);
}

void connect_to_wifi() {
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

void web_controller_setup() {
  connect_to_wifi();

#ifdef ESP32
  const char *APIHeaders[] = { "X-Api-Key" };
  server.collectHeaders(APIHeaders, 1);
#elif defined(ESP8266)
  server.collectHeaders("X-Api-Key");
#endif

  server.onNotFound(not_found);
  server.on("/", HTTP_GET, update_page);
  server.on("/api/sync", HTTP_POST, post_sync);
  server.on("/api/status", HTTP_GET, get_ac_status);
  server.on("/api/update", HTTP_POST, post_update_api);
  server.on("/api/toggle_off", HTTP_POST, toggle_off);
  server.on("/api/toggle_energy_saver", HTTP_POST, toggle_energy_saver);

  server.begin();
}

void web_controller_loop() {
  server.handleClient();
}