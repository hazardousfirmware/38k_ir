#include "hw.h"

#include <avr/io.h>
#include <stdio.h>

int usart_putchar_printf(char var, FILE *stream)
{
    if (var == '\n')
    {
        usart_transmit('\r');
    }
    usart_transmit(var);
    return 0;
}

FILE serial_stdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

// microsecond delay
void delay_us(unsigned int time)
{
    while (time--)
    {
        //_delay_us(1);

        /* avr's delay function is faulty
         so I'll make my own.
         at 16MHz clock, 1 clock happens every 62.5ns.
         we need 16 clocks to take 1 us.
         a few of the clocks are taken by the comparison of values (x2), int decrements (x2) and the while loop (a branch or jump)
         */

         __asm__ ("nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  "nop\n"
                  );
    }
}

void init_systick(void)
{
    // Create a system tick by interrupting when a timer (counter) matches compared value

    TCCR0B = (1<<CS00); // prescale (divide) by 1, every clock
    TCCR0A = (1<<WGM01); // CTC mode

    OCR0A = (F_CPU / (100000UL)) -1; // Timer resets every 10 microseconds

    // Enable interrupt on compare/clear
    TIMSK0 = (1<<OCIE0A);
}

#define IR_INPUT_PIN (1<<4) // IR sensor (interrupt) is on PB4
void init_input_gpio(void)
{
    DDRB &= ~IR_INPUT_PIN;
    PORTB |= IR_INPUT_PIN; // Pull up

    // Enable the ISR
    PCICR = (1<<PCIE0);
    PCMSK0 = (1<<PCINT4);
}

uint8_t read_input_gpio(void)
{
    __asm__("nop"); // to sync pin
    return (PINB & IR_INPUT_PIN) == IR_INPUT_PIN;
}

void init_serial_console(uint32_t baud)
{
    // Set up the usart: 8 data, 1 stop bit, no parity
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);

    // Set the baud rate
    baud = F_CPU / (16*baud) -1;
    UBRR0L = (uint16_t)baud;
    UBRR0H = baud >> 8;

    // Enable Tx
    UCSR0B = (1<<TXEN0);

    // redirect stdout to usart
    stdout = &serial_stdout;
}

void usart_transmit(uint8_t data)
{
    /* Wait for empty transmit buffer */
    while ( !( UCSR0A & (1<<UDRE0)) );
    /* Put data into buffer, sends the data */
    UDR0 = data;
}
