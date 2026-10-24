#ifndef AC_CONTROLLER_H
#define AC_CONTROLLER_H

#include <Arduino.h>

#include "config.hpp"

#define IR_SEND_PIN AC_IR_LED_PIN

#define AC_TEMP_BYTE_OFFSET 34
#define AC_IR_BUF_SIZE 199
#define AC_IR_FREQ 38


typedef enum {
  AC_COMMAND_UPDATE,
  AC_COMMAND_OFF,
  AC_COMMAND_TOGGLE_ENERGY_SAVER,
  AC_COMMAND_NONE
} ACCommand;

typedef enum {
  AC_SPEED_AUTO,
  AC_SPEED_LOW,
  AC_SPEED_MED,
  AC_SPEED_HIGH
} ACSpeed;

typedef enum {
  AC_MODE_FAN,
  AC_MODE_AUTO,
  AC_MODE_COOL,
  AC_MODE_DRY
} ACMode;

typedef struct ACStatus {
  uint8_t temp;
  ACSpeed speed;
  ACMode mode;
  bool enabled;
  bool energy_saver;
} ACStatus;

extern ACStatus ac_status;

#define AC_NAME_COUNT 4

static const char *const ACModeNames[AC_NAME_COUNT] = {
  "fan",
  "auto",
  "cool",
  "dry"
};

static const char *const ACSpeedNames[AC_NAME_COUNT] = {
  "auto",
  "low",
  "medium",
  "high"
};

static const char *const ACCommandNames[AC_COMMAND_NONE] = {
  "update",
  "off",
  "energy_saver"
};
      
bool ac_run_command(ACCommand command);
void ac_sync_state(bool restore_old_state);
void ac_send_ir(uint16_t *out, uint16_t len);

bool ac_can_change_speed();
bool ac_can_change_temp();

void ac_set_speed(ACSpeed set_speed);
void ac_set_temp(uint8_t set_temp);
void ac_set_mode(ACMode mode);

void ac_loop();
void ac_setup();

#endif