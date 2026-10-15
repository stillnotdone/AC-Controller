#include <Arduino.h>

#include "ac_controller.h"
#include "analog_handler.h"

bool IRAM_ATTR check_debounce() {
  static unsigned long lastPress = 0;
  unsigned long now = millis();
  if (now - lastPress < INPUT_DEBOUCE) return false;

  lastPress = now;
  return true;
}

void IRAM_ATTR handle_on() {
  ac_run_command(AC_COMMAND_UPDATE);
}

void IRAM_ATTR handle_off() {
  ac_run_command(AC_COMMAND_OFF);
}

void IRAM_ATTR handle_energy() {
  ac_run_command(AC_COMMAND_TOGGLE_ENERGY_SAVER);
}

void IRAM_ATTR handle_mode() {
  if(!check_debounce()) return;
  ACMode mode = (ACMode)((ac_get_mode() + 1) % 4);
  ac_set_mode(mode);
}

void IRAM_ATTR handle_speed() {
  if(!check_debounce()) return;
  ACSpeed speed = (ACSpeed)((ac_get_speed() + 1) % 4);
  ac_set_speed(speed);
}

void IRAM_ATTR handle_temp() {
  if(!check_debounce()) return;
  static int mod = -1;
  uint8_t temp = ac_get_temp();
  temp += mod;
  if(temp >= AC_MAX_TEMP) mod = -1;
  if(temp <= AC_MIN_TEMP) mod = 1;
  ac_set_temp(temp);
}

const uint8_t input_pins[] = {
  INPUT_ON_PIN,
  INPUT_OFF_PIN,
  INPUT_TEMP_PIN,
  INPUT_MODE_PIN,
  INPUT_SPEED_PIN,
  INPUT_ENERGY_PIN
};

void (*input_handlers[])() = {
  handle_on,
  handle_off,
  handle_temp,
  handle_mode,
  handle_speed,
  handle_energy
};

void analog_handler_setup() {
  size_t pin_count = sizeof(input_pins) / sizeof(input_pins[0]);
  for(size_t i = 0; i < pin_count; i++) {
    pinMode(input_pins[i], INPUT_PIN_MODE);
    attachInterrupt(input_pins[i], input_handlers[i], INPUT_INTERRUPT_MODE);
  }
}