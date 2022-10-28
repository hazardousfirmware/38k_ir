#include "nec_tx.h"

// Default function for unregistered pointers
static void do_nothing(uint32_t val)
{
}

// Function pointers for delays and PWM control
static void (*pwm_on)(void) = (void(*)(void))do_nothing;
static void (*pwm_off)(void) = (void(*)(void))do_nothing;
static void (*internal_delay_us)(unsigned int) = (void(*)(unsigned int))do_nothing;


void nec_register_functions(void (*pwm_on_function)(void), void (*pwm_off_function)(void),
                                  void (*delay_us_function)(unsigned int))
{
    pwm_on = pwm_on_function;
    pwm_off = pwm_off_function;
    internal_delay_us = delay_us_function;
}

static inline void write_bit(uint8_t bit)
{
    if (!bit)
    {
        // the bit is not set, encode a 0 symbol
        pwm_on();
        internal_delay_us(562);
        pwm_off();
        internal_delay_us(562);
    }
    else
    {
        // the bit is set, encode a 1 symbol
        pwm_on();
        internal_delay_us(562);
        pwm_off();
        internal_delay_us(1687);
    }
}

static inline void write_octet(uint8_t value)
{
    uint8_t bit = 0;
    for (int i = 0; i < 8; i++)
    {
        bit = (uint8_t)(value & 0x80);
        write_bit(bit);
        value <<= 1;
    }
}

void send_nec_ircode(uint8_t address, uint8_t cmd)
{
    // Send preamble
    pwm_on();
    internal_delay_us(9100);
    pwm_off();
    internal_delay_us(4500);

    // Send data
    write_octet(address);
    write_octet(~address);

    write_octet(cmd);
    write_octet(~cmd);

    pwm_off();
    internal_delay_us(40000);
}
