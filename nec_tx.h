#ifndef __NEC_TX_H__
#define __NEC_TX_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void send_nec_ircode(uint8_t address, uint8_t cmd);
void nec_register_functions(void (*pwm_on_function)(void),
                                  void (*pwm_off_function)(void),
                                  void (*delay_us_function)(unsigned int));

#ifdef __cplusplus
     }
#endif

#endif // !__NEC_TX_H__
