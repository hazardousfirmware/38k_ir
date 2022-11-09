#ifndef __PANASONIC_TX_H__
#define __PANASONIC_TX_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void send_panasonic_ircode(uint16_t id, uint16_t cmd);
void panasonic_register_functions(void (*pwm_on_function)(void),
                                  void (*pwm_off_function)(void),
                                  void (*delay_us_function)(unsigned int),
                                  void (*internal_delay_ms)(uint32_t));


#ifdef __cplusplus
     }
#endif

#endif //!__PANASONIC_TX_H__
