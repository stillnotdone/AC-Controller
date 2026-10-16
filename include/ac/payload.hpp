#ifndef AC_PAYLOAD_H
#define AC_PAYLOAD_H

#include <Arduino.h>

#include "ac/controller.hpp"

#define AC_ENABLED_FRAME 0xA1
#define AC_ENERGY_FRAME 0xA2

#define AC_IGNORE_TEMP_BYTE 0x7E

#define AC_POWER_ON_MASK 0b10000000
#define AC_PAYLOAD_1_MODE_MASK 0b1111
#define AC_PAYLOAD_5_MODE_MASK 0b111

void ac_build_payload(uint8_t payload[6]);
void ac_apply_command(uint8_t payload[6], ACCommand command);
void ac_send_payload(uint8_t payload[6]);

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


#endif