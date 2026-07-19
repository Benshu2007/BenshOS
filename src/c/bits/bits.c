#include "bits.h"


uint16_t get_16_low(uint32_t value) {
    return (uint16_t)(value & 0xFFFF);
}

uint16_t get_16_high(uint32_t value) {
    return (uint16_t)((value >> 16) & 0xFFFF);
}

uint8_t get_8_low(uint16_t value) {
    return (uint8_t)(value & 0xFF);
}

uint8_t get_8_high(uint16_t value) {
    return (uint8_t)((value >> 8) & 0xFF);
}

uint8_t get_4_low(uint8_t value) {
    return (uint8_t)(value & 0x0F);
}

uint8_t get_4_high(uint8_t value) {
    return (uint8_t)((value >> 4) & 0x0F);
}
