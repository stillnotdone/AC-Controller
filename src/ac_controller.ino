#include "ac_controller.h"
#include "midea.h"

#include <IRremote.hpp>

uint8_t temp = AC_DEFAULT_TEMP;
ACSpeed speed = AC_SPEED_LOW;
ACMode mode = AC_MODE_FAN;
bool enabled = false;
bool energy_saver = false;

ACCommand command_stack[AC_STACK_SIZE];
size_t stack_ptr = 0;

bool ac_push_command(ACCommand command) {
  if (stack_ptr >= AC_STACK_SIZE) {
    return false; // Stack full
  }

  command_stack[stack_ptr++] = command;
  return true;
}

bool ac_pop_command(ACCommand *command) {
  if (stack_ptr == 0) {
    return false; // Stack empty
  }

  *command = command_stack[--stack_ptr];
  return true;
}

void ac_update_state() {
  ac_push_command(AC_COMMAND_UPDATE);
}

void ac_sync_state(bool restore_old_state) {
  bool is_enabled = enabled;
  bool is_energy_saver = energy_saver;

  if(restore_old_state) {
    if(is_enabled) ac_push_command(AC_COMMAND_UPDATE);
    if(is_energy_saver) ac_push_command(AC_COMMAND_TOGGLE_ENERGY_SAVER);
  }
  ac_push_command(AC_COMMAND_OFF); // sync energy saver and enabled state
}

bool can_change_speed() {
  return mode == AC_MODE_COOL || mode == AC_MODE_FAN;
}

bool can_change_temp() {
  return mode != AC_MODE_FAN;
}

void ac_build_payload(uint8_t payload[6]) {
  payload[0] = 0xA1; // Toggle ON payload (default payload)
  payload[1] = AC_POWER_ON_MASK;
  payload[2] = temp + AC_TEMP_BYTE_OFFSET;
  payload[3] = 0xFF;
  payload[4] = 0xFF;
  payload[5] = AC_POWER_ON_MASK;

  if(can_change_speed()) {
    const uint8_t *offs = mode == AC_MODE_COOL ? ACSpeedCoolOffsets[speed] : ACSpeedFanOffsets[speed];
    payload[1] = offs[0];
    payload[5] = offs[1];
  } else {
    // toggle mode bits
    payload[1] = (AC_POWER_ON_MASK & ~AC_PAYLOAD_1_MODE_MASK) | ACModeOffsets[mode][1];
    payload[5] = (AC_POWER_ON_MASK & ~AC_PAYLOAD_5_MODE_MASK) | ACModeOffsets[mode][0];
  }

  if(!can_change_temp()) payload[2] = 0x7E; // temp ignored in fan mode
}

void ac_apply_command(uint8_t payload[6], ACCommand command) {
  switch(command) {
    case AC_COMMAND_OFF: {
      enabled = false;
      energy_saver = true; // ac powers on with energy saver mode by default, keep synced
      payload[1] &= ~AC_POWER_ON_MASK; // set toggle 0
      payload[5] &= ~AC_POWER_ON_MASK;
      break;
    }
    case AC_COMMAND_TOGGLE_ENERGY_SAVER: {
      energy_saver = !energy_saver;
      payload[0] = 0xA2;   // energy toggle frame type
      payload[1] = 0x02;
      payload[2] = 0xFF;
      payload[5] = 0x7E;
      break;
    }
    case AC_COMMAND_UPDATE: 
      enabled = true; // send current state
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

  Serial.printf("Sent IR: %s mode=%s speed=%s temp=%d enabled=%s energy_saver=%s\n", ACCommandNames[command], ACModeNames[mode], ACSpeedNames[speed], temp, enabled ? "true" : "false", energy_saver ? "true" : "false");

  command = AC_COMMAND_IDLE;
}

void ac_setup() {
  IrSender.begin(IR_SEND_PIN);
}

bool ac_run_command(ACCommand command) {
  return ac_push_command(command);
}

void ac_set_mode(ACMode set_mode) {
  mode = set_mode;
}

void ac_set_speed(ACSpeed set_speed) {
  if(!can_change_speed()) mode = AC_DEFAULT_SPEED_MODE;
  speed = set_speed;
}

void ac_set_temp(uint8_t set_temp) {
  if(!can_change_temp()) mode = AC_DEFAULT_TEMP_MODE; 
  temp = max(AC_MIN_TEMP, min(set_temp, AC_MAX_TEMP));
}

ACSpeed ac_get_speed() {
  return speed;
}

ACMode ac_get_mode() {
  return mode;
}

uint8_t ac_get_temp() {
  return temp;
}

bool ac_is_enabled() {
  return enabled;
}

bool ac_is_energy_saver() {
  return energy_saver;
}
