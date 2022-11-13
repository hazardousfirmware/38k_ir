#ifndef __SAMSUNG_RX_H__
#define __SAMSUNG_RX_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdint.h>

void samsung_decode_falling_edge(uint32_t timestamp);
void samsung_button_callback_1(uint16_t address, uint16_t command);
void samsung_button_callback_2(uint32_t address, uint16_t command, uint8_t extra);


#ifdef __cplusplus
     }
#endif

#endif // !__SAMSUNG_RX_H__
