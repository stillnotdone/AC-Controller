#include "ac/controller.hpp"
#include "web/handler.hpp"
#include "web/helpers.hpp"

static int resolve_name(const char * const* names, const char *input) {
  for(int i = 0; i < AC_NAME_COUNT; i++) {
    if(strcasecmp(input, names[i]) == 0) return i;
  }
  return -1;
}

void api_post_update() {
  if(!web_should_respond()) return;
  int mode = -1;
  int speed = -1;
  int temp = -1;

  if(web_server.hasArg("mode")) {
    mode = resolve_name(ACModeNames, web_server.arg("mode").c_str());
    if(mode == -1) return web_error_response(400, "invalid mode");
  }

  if(web_server.hasArg("speed")) {
    speed = resolve_name(ACSpeedNames, web_server.arg("speed").c_str());
    if(speed == -1) return web_error_response(400, "invalid speed");
  }

  if(web_server.hasArg("temp")) {
    temp = web_server.arg("temp").toInt();
    if(temp == 0) return web_error_response(400, "invalid temp");
    temp = max((int)AC_MIN_TEMP, min(temp, (int)AC_MAX_TEMP));
  }

  // apply speed and temp first, mode last, so an explicit mode wins
  if(speed != -1) ac_set_speed((ACSpeed)speed);
  if(temp != -1)  ac_set_temp((uint8_t)temp);
  if(mode != -1)  ac_set_mode((ACMode)mode);

  web_do_command(AC_COMMAND_UPDATE);
}

void api_post_sync() {
  if(!web_should_respond()) return;
  ac_sync_state(true);
  web_good_response();
}

void api_post_toggle_energy_saver() {
  if(!web_should_respond()) return;
  web_do_command(AC_COMMAND_TOGGLE_ENERGY_SAVER);
}

void api_post_ac_off() {
  if(!web_should_respond()) return;
  web_do_command(AC_COMMAND_OFF);
}

void api_get_status() {
  char status[128];

  snprintf(status, sizeof(status),
    "{\"mode\":\"%s\",\"speed\":\"%s\",\"temp\":%d, \"enabled\":%s,\"energy_saver\":%s}",
    ACModeNames[ac_status.mode],
    ACSpeedNames[ac_status.speed],
    ac_status.temp,
    web_bool2string(ac_status.enabled),
    web_bool2string(ac_status.energy_saver)
  );

  web_server.send(200, "application/json", status);
}