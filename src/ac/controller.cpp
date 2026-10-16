#include "ac/controller.hpp"
#include "ac/midea.hpp"
#include "ac/payload.hpp"

ACStatus ac_status = {AC_DEFAULT_TEMP, AC_SPEED_LOW, AC_MODE_FAN, false, false};

static struct ACCommandQueue {
  ACCommand stack[AC_COMMAND_STACK_SIZE];
  size_t ptr;
} ac_command_queue;

static bool ac_push_command(ACCommand command) {
  if (ac_command_queue.ptr >= AC_COMMAND_STACK_SIZE) return false;
  ac_command_queue.stack[ac_command_queue.ptr++] = command;
  return true;
}

static bool ac_pop_command(ACCommand *command) {
  if (ac_command_queue.ptr == 0) return false;
  *command = ac_command_queue.stack[--ac_command_queue.ptr];
  return true;
}

void ac_sync_state(bool restore_old_state) {
  ac_command_queue.ptr = 0; // drop any commands to sync safely

  bool is_enabled = ac_status.enabled;
  bool is_energy_saver = ac_status.energy_saver;

  if(restore_old_state) {
    if(!is_energy_saver) ac_push_command(AC_COMMAND_TOGGLE_ENERGY_SAVER);
    if(is_enabled) ac_push_command(AC_COMMAND_UPDATE);
  }
  // sync energy saver and enabled state (energy saver = 1, enabled = 0)
  // this runs first because it's a stack queue
  ac_push_command(AC_COMMAND_OFF);
}

void ac_loop() {
  static unsigned long last_command_time = 0;
  if(millis() - last_command_time < AC_COMMAND_FREQ) return;
  last_command_time = millis();

  ACCommand command;
  if(!ac_pop_command(&command)) return;

  uint8_t payload[6];

  ac_build_payload(payload);
  ac_apply_command(payload, command);
  ac_send_payload(payload);

  Serial.printf("Sent IR: %s mode=%s speed=%s temp=%d enabled=%s energy_saver=%s\n", ACCommandNames[command], ACModeNames[ac_status.mode], ACSpeedNames[ac_status.speed], ac_status.temp, ac_status.enabled ? "true" : "false", ac_status.energy_saver ? "true" : "false");
}

bool ac_run_command(ACCommand command) {
  return ac_push_command(command);
}

void ac_set_mode(ACMode set_mode) {
  ac_status.mode = set_mode;
}

void ac_set_speed(ACSpeed set_speed) {
  if(!ac_can_change_speed()) ac_status.mode = AC_DEFAULT_SPEED_MODE;
  ac_status.speed = set_speed;
}

void ac_set_temp(uint8_t set_temp) {
  if(!ac_can_change_temp()) ac_status.mode = AC_DEFAULT_TEMP_MODE; 
  ac_status.temp = max(AC_MIN_TEMP, min(set_temp, AC_MAX_TEMP));
}

bool ac_can_change_speed() {
  return ac_status.mode == AC_MODE_COOL || ac_status.mode == AC_MODE_FAN;
}

bool ac_can_change_temp() {
  return ac_status.mode != AC_MODE_FAN;
}