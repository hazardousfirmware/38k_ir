#ifndef ATMEGA_H_INCLUDED
#define ATMEGA_H_INCLUDED

#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL // or whatever may be your frequency
#endif // F_CPU

void delay_us(unsigned int time);

void init_systick(void);

void init_input_gpio(void);
uint8_t read_input_gpio(void);

void init_serial_console(uint32_t baud);
void usart_transmit(uint8_t data);

#endif //ATMEGA_H_INCLUDED
