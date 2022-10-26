#ifndef __PANASONIC_RX_H__
#define __PANASONIC_RX_H__

#include <stdint.h>

void panasonic_decode_falling_edge(uint32_t current_timestamp);
void panasonic_button_callback(uint16_t command);

#endif // !__PANASONIC_RX_H__
