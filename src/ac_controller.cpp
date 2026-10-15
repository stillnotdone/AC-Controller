#include "ac_controller.hpp"
#include "midea.hpp"

#include <IRremote.hpp>

ACStatus ac_status = {AC_DEFAULT_TEMP, AC_SPEED_LOW, AC_MODE_FAN, false, false};

struct ACCommandQueue {
  ACCommand stack[AC_COMMAND_STACK_SIZE];
  size_t ptr;
} ac_command_queue;

bool ac_push_command(ACCommand command) {
  if (ac_command_queue.ptr >= AC_COMMAND_STACK_SIZE) return false;
  ac_command_queue.stack[ac_command_queue.ptr++] = command;
  return true;
}

bool ac_pop_command(ACCommand *command) {
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

bool can_change_speed() {
  return ac_status.mode == AC_MODE_COOL || ac_status.mode == AC_MODE_FAN;
}

bool can_change_temp() {
  return ac_status.mode != AC_MODE_FAN;
}

void ac_build_payload(uint8_t payload[6]) {
  uint8_t speed_bits = can_change_speed() ? ACSpeedBits[ac_status.speed] : 0;
  
  memset(payload, 0xFF, 6);

  payload[0] = AC_ENABLED_FRAME; // Toggle ON payload (default payload)
  payload[1] = AC_POWER_ON_MASK | (speed_bits << 3) | ACModeBits[ac_status.mode];
  
  if(can_change_temp())
    payload[2] = ac_status.temp + AC_TEMP_BYTE_OFFSET;
  else 
    payload[2] = AC_IGNORE_TEMP_BYTE; // temp ignored in fan mode

  payload[5] = midea_checksum(payload) | AC_POWER_ON_MASK; // matches remote payload, but ac accepts 0x00
}

void ac_apply_command(uint8_t payload[6], ACCommand command) {
  switch(command) {
    case AC_COMMAND_OFF: {
      ac_status.enabled = false;
      ac_status.energy_saver = true; // ac powers on with energy saver mode by default, keep synced
      payload[1] &= ~AC_POWER_ON_MASK; // set toggle 0
      payload[5] &= ~AC_POWER_ON_MASK;
      break;
    }
    case AC_COMMAND_TOGGLE_ENERGY_SAVER: {
      ac_status.energy_saver = !ac_status.energy_saver;
      payload[0] = AC_ENERGY_FRAME;   // energy toggle frame type
      payload[1] = 0x02; // idk
      payload[2] = 0xFF;
      payload[5] = midea_checksum(payload);
      break;
    }
    case AC_COMMAND_UPDATE: 
      ac_status.enabled = true; // send current state
      break;
    default: break;
  }
}

void ac_send_payload(uint8_t payload[6]) {
  uint16_t out[AC_IR_BUF_SIZE];
  size_t len = midea_encode(out, payload);
  IrSender.sendRaw(out, len, AC_IR_FREQ); 
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

void ac_setup() {
  IrSender.begin(IR_SEND_PIN);
}

bool ac_run_command(ACCommand command) {
  return ac_push_command(command);
}

void ac_set_mode(ACMode set_mode) {
  ac_status.mode = set_mode;
}

void ac_set_speed(ACSpeed set_speed) {
  if(!can_change_speed()) ac_status.mode = AC_DEFAULT_SPEED_MODE;
  ac_status.speed = set_speed;
}

void ac_set_temp(uint8_t set_temp) {
  if(!can_change_temp()) ac_status.mode = AC_DEFAULT_TEMP_MODE; 
  ac_status.temp = max(AC_MIN_TEMP, min(set_temp, AC_MAX_TEMP));
}

const ACStatus *ac_get_status() {
  return &ac_status;
}