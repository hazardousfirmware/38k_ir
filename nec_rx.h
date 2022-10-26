#ifndef NEC_INFRARED_H_INCLUDED
#define NEC_INFRARED_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

void necdecoder_decode_falling_edge(uint32_t current_timestamp);
void necdecoder_handle_remote_button(uint8_t address, uint8_t command, bool repeat);

uint8_t swap_bit_order(uint8_t value);

#endif // NEC_INFRARED_H_INCLUDED
