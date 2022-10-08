#ifndef NEC_INFRARED_H_INCLUDED
#define NEC_INFRARED_H_INCLUDED

#include <stdint.h>

#define BUTTON_RELEASE_TIMEOUT_MS 110

void necdecoder_decode_falling_edge(uint32_t current_timestamp);
void necdecoder_register_button_handler(void (*func)(uint8_t, uint8_t));

#endif // NEC_INFRARED_H_INCLUDED
