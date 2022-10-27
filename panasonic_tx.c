#include "panasonic_tx.h"

/*
===start===
on for 3600us
off for 1200 us

===data=== (32bit identifier + 16 bit code)
state 0 = (720 - 880)us, low for 480us, high for 240us (440)
state 1 = (1680-1760)us, low for 1200us, high for (480 to 560)us

=== end / between frames ===
off for 73ms
*/

// timing of pulses in microseconds
#define PREAMBLE_LONG 3600
#define PREAMBLE_SHORT 1200

#define BIT_LONG 1200
#define BIT_SHORT 440

// minimum time between frames in milliseconds
#define DELAY_BETWEEN_FRAMES 73

// Default function for unregistered pointers
static void do_nothing(uint32_t val)
{
}

static void (*pwm_on)(void) = (void(*)(void))do_nothing;
static void (*pwm_off)(void) = (void(*)(void))do_nothing;
static void (*internal_delay_us)(unsigned int) = (void(*)(unsigned int))do_nothing;
static void (*internal_delay_ms)(uint32_t) = do_nothing;

static inline void write_bit(uint8_t bit)
{
    if (bit)
    {
        // the bit is set, encode a 1 symbol
        pwm_off();
        internal_delay_us(BIT_LONG);
        pwm_on();
        internal_delay_us(BIT_SHORT);
    }
    else
    {
        // the bit is not set, encode a 0 symbol
        pwm_off();
        internal_delay_us(BIT_SHORT);
        pwm_on();
        internal_delay_us(BIT_SHORT);
    }
}


void panasonic_register_functions(void (*pwm_on_function)(void), void (*pwm_off_function)(void), 
                                  void (*delay_us_function)(unsigned int),
                                  void (*delay_ms_function)(uint32_t))
{
    pwm_on = pwm_on_function;
    pwm_off = pwm_off_function;
    internal_delay_us = delay_us_function;
    internal_delay_ms = delay_ms_function;
}

void send_panasonic_ircode(uint32_t device, uint16_t code)
{
    uint8_t bit = 0;

    // Preamble
    pwm_on();
    internal_delay_us(PREAMBLE_LONG);
    pwm_off();
    internal_delay_us(PREAMBLE_SHORT);

    // Device identifier
    for (int i = 0; i < 32; i++)
    {
        bit = (uint8_t)(device & 0x01);
        write_bit(bit);
        device >>= 1;
    }

    // Command
    for (int i = 0; i < 16; i++)
    {
        bit = (uint8_t)(code & 0x01);
        write_bit(bit);
        code >>= 1;
    }

    internal_delay_ms(DELAY_BETWEEN_FRAMES);
}
