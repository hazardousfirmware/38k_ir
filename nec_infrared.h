#ifndef NEC_INFRARED_H_INCLUDED
#define NEC_INFRARED_H_INCLUDED

#include <stdint.h>

#define BUTTON_RELEASE_TIMEOUT_MS 110

void necdecoder_decode_falling_edge(uint32_t current_timestamp);
void necdecoder_handle_remote_button(uint8_t address, uint8_t command);

#endif // NEC_INFRARED_H_INCLUDED
