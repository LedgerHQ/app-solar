#pragma once

#include <stdint.h>  // uint*_t

#define CLA      ((uint8_t)0xE0)  // Instruction class
#define P1_START ((uint8_t)0x00)  // Indicates the first APDU chunk received
#define P2_MORE  ((uint8_t)0x80)  // Indicates that more chunks are to be received
#define P2_LAST  ((uint8_t)0x00)  // Indicates the last APDU chunk received

#define P1_APPROVAL ((uint8_t)0x01)  // Indicates that user review is required
#define P_UNUSED    ((uint8_t)0x00)  // Indicates that a parameter will not be used
