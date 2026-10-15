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

#define AC_ENABLED_FRAME 0xA1
#define AC_ENERGY_FRAME 0xA2

#define AC_IGNORE_TEMP_BYTE 0x7E

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
 * payload[1] | [P SSSS MMM]              | Bit 7: Power (1=On, 0=Off)
 *            |                           | Bits 6-3: Fan speed code (ACSpeedBits)
 *            |                           |   auto=100, low=001, med=010, high=011
 *            |                           |   0000 in Auto and Dry (speed not selectable)
 *            |                           | Bits 2-0: Mode bits (ACModeBits)
 *            |                           |   fan=100, auto=010, cool=000, dry=001
 * -----------|---------------------------|---------------------------------
 * payload[2] | [TTTT TTTT]               | Temperature Value
 *            |                           | Encoded as: (Desired Temp + AC_TEMP_BYTE_OFFSET)
 *            |                           | Fan mode always sends 0x7E (temp ignored)
 * -----------|---------------------------|---------------------------------
 * payload[3] | [1111 1111] (0xFF)        | Unused / Padding Byte or probably for timer
 * -----------|---------------------------|---------------------------------
 * payload[4] | [1111 1111] (0xFF)        | Unused / Padding Byte or probably for timer
 * -----------|---------------------------|---------------------------------
 * payload[5] | [P CCC CCCC]              | Bits 6-0: Checksum of bytes 0-4 (midea_checksum)
 *            |                           | Bit 7: Power mirror (1=On, 0=Off)
 *            |                           | The AC accepts frames with this byte set to 0x00
 * -------------------------------------------------------------------------
 *
 * Energy saver toggle frame: A2 02 FF FF FF 7E
 * Off frame: the normal frame with bit 7 cleared in bytes 1 and 5.
 */

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

// goes in bits 6-3 of byte 1
static const uint8_t ACSpeedBits[4] = { 0b100, 0b001, 0b010, 0b011 };   // auto, low, med, high
// goes in bits 2-0 of byte 1
static const uint8_t ACModeBits[4]  = { 0b100, 0b010, 0b000, 0b001 };   // fan, auto, cool, dry

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

const ACStatus *ac_get_status();
void ac_set_speed(ACSpeed set_speed);
void ac_set_temp(uint8_t set_temp);
void ac_set_mode(ACMode mode);

void ac_loop();
void ac_setup();

#endif