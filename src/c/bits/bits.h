#ifndef BITS_H
#define BITS_H

#include <stdint.h>

// --- 16-Bit Extractions (From a 32-bit source) ---

// Gets the lower 16 bits of a 32-bit integer
uint16_t get_16_low(uint32_t value);
// Gets the higher 16 bits of a 32-bit integer
uint16_t get_16_high(uint32_t value);

// --- 8-Bit Extractions (From a 16-bit source) ---
// Gets the lower 8 bits of a 16-bit integer
uint8_t get_8_low(uint16_t value);
// Gets the higher 8 bits of a 16-bit integer
uint8_t get_8_high(uint16_t value);

// --- 4-Bit Extractions (From an 8-bit source) ---
// Gets the lower 4 bits of an 8-bit integer
uint8_t get_4_low(uint8_t value);
// Gets the higher 4 bits of an 8-bit integer
uint8_t get_4_high(uint8_t value);


#endif