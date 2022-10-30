#ifndef NEC_INFRARED_H_INCLUDED
#define NEC_INFRARED_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void necdecoder_decode_falling_edge(uint32_t current_timestamp);
void necdecoder_handle_remote_button(uint8_t address, uint8_t command, bool repeat);

#ifdef __cplusplus
     }
#endif

#endif // NEC_INFRARED_H_INCLUDED
