#ifndef __PANASONIC_RX_H__
#define __PANASONIC_RX_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void panasonic_decode_falling_edge(uint32_t timestamp);
void panasonic_button_callback(uint16_t identifier, uint16_t command);

#ifdef __cplusplus
     }
#endif

#endif // !__PANASONIC_RX_H__
