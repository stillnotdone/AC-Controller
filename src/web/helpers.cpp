#include "web/handler.hpp"
#include "web/html.hpp"
#include "ac/controller.hpp"
#include "secrets.hpp"
#include "config.hpp"

const char *web_bool2string(bool b) {
  return b ? "true" : "false";
}

void web_good_response() {
  web_server.send(200, "text/plain", "");
}

void web_error_response(int code, const char *message) {
  web_server.send(code, "text/plain", message);
}

void web_not_found() {
  web_error_response(404, "not found");
}

void web_update_page() {
  web_server.send_P(200, "text/html", WEB_UPDATER_HTML);
}

static bool is_rate_limited() {
  static unsigned long last_web_update = 0;
  unsigned long now = millis();
  if(now - last_web_update > WEB_UPDATE_FREQUENCY) {
    last_web_update = now;
    return false;
  }
  return true;
}

static bool is_authorized() {
  if(web_server.hasHeader(WEB_API_KEY_HEADER)) return web_server.header(WEB_API_KEY_HEADER) == api_key;
  return false;
}

bool web_should_respond() {
  if(is_rate_limited()) {
    web_error_response(429, "too many requests");
    return false;
  }
  if(!is_authorized()) {
    web_error_response(401, "unauthorized");
    return false;
  }
  return true;
}

void web_do_command(ACCommand command) {
  bool success = ac_run_command(command);
  if(success)
    web_good_response();
  else
    web_error_response(503, "command queue full");
}