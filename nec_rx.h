#ifndef NEC_INFRARED_H_INCLUDED
#define NEC_INFRARED_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void necdecoder_decode_falling_edge(uint32_t timestamp);

void nec_button_callback(uint8_t address, uint8_t command, bool repeat);
void nec16_button_callback(uint16_t address, uint8_t command, bool repeat);

#ifdef __cplusplus
     }
#endif

#endif // NEC_INFRARED_H_INCLUDED
