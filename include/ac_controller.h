#ifndef AC_CONTROLLER_H
#define AC_CONTROLLER_H

#include <Arduino.h>

#define IR_SEND_PIN 4

#define AC_STACK_SIZE 10
#define AC_COMMAND_FREQ 250 // ms

#define AC_TEMP_BYTE_OFFSET 34
#define AC_IR_BUF_SIZE 199
#define AC_IR_FREQ 38

#define AC_MAX_TEMP (uint8_t)86u
#define AC_MIN_TEMP (uint8_t)62u
#define AC_DEFAULT_TEMP (AC_MIN_TEMP + ((AC_MAX_TEMP - AC_MIN_TEMP) / 2))

#define AC_POWER_ON_MASK 0b10000000
#define AC_PAYLOAD_1_MODE_MASK 0b1111
#define AC_PAYLOAD_5_MODE_MASK 0b111

#define AC_DEFAULT_SPEED_MODE AC_MODE_FAN
#define AC_DEFAULT_TEMP_MODE AC_MODE_COOL

/*
 * MIDEA IR PROTOCOL PAYLOAD STRUCTURE (6 Bytes)
 * -------------------------------------------------------------------------
 * Byte Index | Bit Field / Mask          | Description & Usage
 * -----------|---------------------------|---------------------------------
 * payload[0] | [1010 0001] (0xA1 / 0xA2) | Command Type 
 *            |                           | (0xA1 = Standard, 0xA2 = Energy Saver)
 * -----------|---------------------------|---------------------------------
 * payload[1] | [P---] [----]             | Bit 7 (MSB): Power Toggle (1=On, 0=Off)
 *            |                           | Lower bits: Mode/Speed configuration bits
 * -----------|---------------------------|---------------------------------
 * payload[2] | [TTTT TTTT]               | Temperature Value 
 *            |                           | Encoded as: (Desired Temp + AC_TEMP_BYTE_OFFSET)
 * -----------|---------------------------|---------------------------------
 * payload[3] | [1111 1111] (0xFF)        | Unused / Padding Byte or probably for timer
 * -----------|---------------------------|---------------------------------
 * payload[4] | [1111 1111] (0xFF)        | Unused / Padding Byte or probably for timer
 * -----------|---------------------------|---------------------------------
 * payload[5] | [P---] [-MMM]             | Bit 7 (MSB): Power Toggle mirror (1=On, 0=Off)
 *            |                           | Bits 2-0 (LSBs): Active AC Mode / Speed bits
 * -------------------------------------------------------------------------
 */

typedef enum {
  AC_COMMAND_IDLE,
  AC_COMMAND_UPDATE,
  AC_COMMAND_OFF,
  AC_COMMAND_TOGGLE_ENERGY_SAVER
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

// the first go into the first 3 lsb of the 6th byte and the second goes into the first 4 lsb of the 2nd byte of the payload
// todo reverse index 0 to be index 1 to be consistent with other offset tables
static const uint8_t ACModeOffsets[4][2] = {
  { 0b011, 0b1100 },
  { 0b100, 0b0010 },
  { 0b110, 0b1000 },
  { 0b111, 0b0001 }
};

static const uint8_t ACSpeedFanOffsets[4][2] = {
  { 0b10100100, 0b11011011 },
  { 0b10001100, 0b11110011 },
  { 0b10010100, 0b11101011 },
  { 0b10011100, 0b11100011 }
};
// first goes into the second byte, and the 2nd goes into the sixth byte
static const uint8_t ACSpeedCoolOffsets[4][2] = {
  { 0b10100000, 0b11000110 },
  { 0b10001000, 0b11101010 },
  { 0b10010000, 0b11111010 },
  { 0b10011000, 0b11110010 }
};

#define AC_NAME_COUNT 4

static const char *ACModeNames[AC_NAME_COUNT] = {
  "fan",
  "auto",
  "cool",
  "dry"
};

static const char *ACSpeedNames[AC_NAME_COUNT] = {
  "auto",
  "low",
  "medium",
  "high"
};

static const char *ACCommandNames[4] = {
  "idle",
  "update",
  "off",
  "energy_saver"
};
      
bool ac_run_command(ACCommand command);
void ac_set_speed(ACSpeed set_speed);
void ac_set_temp(uint8_t set_temp);
void ac_set_mode(ACMode mode);
ACMode ac_get_mode();
ACSpeed ac_get_speed();
uint8_t ac_get_temp();
bool ac_is_enabled();
bool ac_is_energy_saver();
void ac_sync_state(bool keep_old_state);
void ac_update_state();
void ac_loop();
void ac_setup();

#endif