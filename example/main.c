#include "nec_rx.h"
#include "nec_tx.h"
#include "hw.h"
#include "panasonic_tx.h"
#include "panasonic_rx.h"
#include "ring.h"
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>

#define CONSOLE_BAUDRATE 57600

volatile uint32_t timestamp = 0;
volatile uint32_t time_delay = 0;

void delay_ms(uint32_t ms)
{
    time_delay = ms*100; // 100 * 10us = 1ms
    while (time_delay > 0);
}

int main(void)
{
    init_serial_console(CONSOLE_BAUDRATE);
    init_systick();
    init_input_gpio();
    sei();

    uint32_t item = 0;

    while(1)
    {
        if (!ring_empty())
        {
            // data is available
            item = ring_get() * 10; // 10us has elapsed per tick
            necdecoder_decode_falling_edge(item);
            panasonic_decode_falling_edge(item);
            //printf("%lu\n", item);
        }
    }
}

void nec16_button_callback(uint16_t address, uint8_t command, bool repeat)
{
    printf("NEC-16, addr=%04x, cmd=%02x\n", address, command);
}

void nec_button_callback(uint8_t address, uint8_t command, bool repeat)
{
    printf("NEC, addr=%02x, cmd=%02x\n", address, command);
}

void panasonic_button_callback(uint16_t identifier, uint16_t command)
{
    printf("Panasonic id=0x%04x, code=0x%04x\n", identifier, command);
}


// Port change interrupt (rise and fall)
ISR(PCINT0_vect)
{
    if (!read_input_gpio())
    {
        ring_put(timestamp);
    }
}

// Timer interrupt (systick)
ISR(TIMER0_COMPA_vect)
{
    static uint32_t timestamp10 = 0;

    // systick every 10us
    timestamp10++;

    if (time_delay != 0)
    {
        time_delay--;
    }

    timestamp = timestamp10;
}
