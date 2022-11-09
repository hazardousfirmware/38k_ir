#include "panasonic_tx.h"

/*
The Protocol: modified manchester encoding, modulated onto 38KHz carrier

===start===
on for 3600us
off for 1200us

===data=== (16 bit identifier + 0x2002 + 16 bit code)
bit 0 = low for ~440us + high for ~440us
bit 1 = low for ~1200us + high for ~480us

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

#define MIDDLE_CONSTANT 0x2002

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
        pwm_on();
        internal_delay_us(BIT_LONG);
        pwm_off();
        internal_delay_us(BIT_SHORT);
    }
    else
    {
        // the bit is not set, encode a 0 symbol
        pwm_on();
        internal_delay_us(BIT_SHORT);
        pwm_off();
        internal_delay_us(BIT_SHORT);
    }
}

static inline void write_word(uint16_t val)
{
    for (int i = 0; i < 16; i++)
    {
        write_bit(val & 0x01);
        val >>= 1;
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

void send_panasonic_ircode(uint16_t id, uint16_t cmd)
{
    // AGC pulse
    pwm_on();
    internal_delay_us(PREAMBLE_LONG);
    pwm_off();
    internal_delay_us(PREAMBLE_SHORT);

    // Data
    write_word(MIDDLE_CONSTANT);
    write_word(id);
    write_word(cmd);

    pwm_off();
    internal_delay_ms(DELAY_BETWEEN_FRAMES);
}
