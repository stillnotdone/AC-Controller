#ifndef AC_CONTROLLER_H
#define AC_CONTROLLER_H

#include <Arduino.h>

#define IR_SEND_PIN 4

#define AC_COMMAND_STACK_SIZE 10
#define AC_COMMAND_FREQ 250 // ms

#define AC_TEMP_BYTE_OFFSET 34
#define AC_IR_BUF_SIZE 199
#define AC_IR_FREQ 38

#define AC_MIN_TEMP (uint8_t)62u
#define AC_MAX_TEMP (uint8_t)86u
#define AC_DEFAULT_TEMP (AC_MIN_TEMP + ((AC_MAX_TEMP - AC_MIN_TEMP) / 2))

#define AC_DEFAULT_SPEED_MODE AC_MODE_FAN
#define AC_DEFAULT_TEMP_MODE AC_MODE_COOL

typedef enum {
  AC_COMMAND_IDLE,
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

// first 3 lsb are the mode bits on the 6th byte of the payload
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
  "idle",
  "update",
  "off",
  "energy_saver"
};
      
bool ac_run_command(ACCommand command);
void ac_sync_state(bool restore_old_state);

bool ac_can_change_speed();
bool ac_can_change_temp();

void ac_set_speed(ACSpeed set_speed);
void ac_set_temp(uint8_t set_temp);
void ac_set_mode(ACMode mode);

void ac_loop();
void ac_setup();

#endif