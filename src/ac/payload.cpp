#include "ac/payload.hpp"
#include "ac/controller.hpp"
#include "ac/midea.hpp"

#include <IRremote.hpp>

// goes in bits 6-3 of byte 1
static const uint8_t ACSpeedBits[4] = { 0b100, 0b001, 0b010, 0b011 };   // auto, low, med, high
// goes in bits 2-0 of byte 1
static const uint8_t ACModeBits[4]  = { 0b100, 0b010, 0b000, 0b001 };   // fan, auto, cool, dry

// here because IRremote can only be included once
void ac_setup() {
  IrSender.begin(IR_SEND_PIN);
}

void ac_build_payload(uint8_t payload[6]) {
  uint8_t speed_bits = ac_can_change_speed() ? ACSpeedBits[ac_status.speed] : 0;
  
  memset(payload, 0xFF, 6);

  payload[0] = AC_ENABLED_FRAME; // Toggle ON payload (default payload)
  payload[1] = AC_POWER_ON_MASK | (speed_bits << 3) | ACModeBits[ac_status.mode];
  
  if(ac_can_change_temp())
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
      payload[1] &= ~AC_POWER_ON_MASK;
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