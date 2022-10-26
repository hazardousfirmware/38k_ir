#include "panasonic_rx.h"

void __attribute__((weak)) panasonic_button_callback(uint16_t command)
{
}

/*
===start===
on for 3600us
off for 1200 us

===data=== (32bit constant 0x01004004 + 16 bit code + 1 bit)
state 0 = (720 - 880)us, low for 480us, high for 240us (440)
state 1 = (1680-1760)us, low for 1200us, high for (480 to 560)us

=== end / between frames ===
off for 73ms
*/

typedef enum 
{
    STATE_IDLE,
    STATE_MAGIC,
    STATE_DATA,
    STATE_END,
    STATE_LOCK
} decoder_state_machine_t;

#define PROTOCOL_MAGIC_NUMBER 0x01004004

void panasonic_decode_falling_edge(uint32_t current_timestamp)
{
    static decoder_state_machine_t state = STATE_IDLE;

    static uint32_t last_timestamp = 0;
    const uint32_t sinceLast = current_timestamp - last_timestamp;

    static uint32_t bits = 0;
    static int count = 0;

    if (state == STATE_IDLE)
    {
        bits = 0;
        count = 0;

        if (sinceLast >= 4400 && sinceLast <= 5200) // ~4800us
        {
            state = STATE_MAGIC;
        }
        
        last_timestamp = current_timestamp;
    }
    else if (state == STATE_MAGIC)
    {
        // Sample 32 bits and check for magic number identifying the protocol

        if (sinceLast > 700 && sinceLast < 900)
        {
            // bit 0 detected
            bits <<= 1;
            bits &= ~(1 << 0);
            count++;
        }
        else if (sinceLast > 1600 && sinceLast < 1820)
        {
            // bit 1 detected
            bits <<= 1;
            bits |= (1 << 0);
            count++;
        }
        else
        {
            state = STATE_IDLE;
            count = 0;
        }

        if (count == 32)
        {
            state = (bits == PROTOCOL_MAGIC_NUMBER) ? STATE_DATA : STATE_IDLE;
            bits = 0;
            count = 0;
        }

        last_timestamp = current_timestamp;
    }
    else if (state == STATE_DATA)
    {
        // Sample 16bits to decode command/data
        if (sinceLast > 700 && sinceLast < 900)
        {
            // bit 0 detected
            bits <<= 1;
            bits &= ~(1 << 0);
            count++;
        }
        else if (sinceLast > 1600 && sinceLast < 1820)
        {
            // bit 1 detected
            bits <<= 1;
            bits |= (1 << 0);
            count++;
        }
        else
        {
            state = STATE_IDLE;
            count = 0;
        }

        if (count == 16)
        {
            state = STATE_END;
            count = 0;
        }

        last_timestamp = current_timestamp;
    }
    else if (state == STATE_END)
    {
        if (sinceLast > 1600 && sinceLast < 1820)
        {
            // end bit detected
            panasonic_button_callback(bits);
            state = STATE_LOCK;
        }
        else
        {
            state = STATE_IDLE;
        }
        bits = 0;
        count = 0;
        last_timestamp = current_timestamp;
    }
    else if (state == STATE_LOCK)
    {
        // block from handling falling edge for at least 73us
        if (sinceLast > 72500)
        {
            state = STATE_IDLE;
            last_timestamp = current_timestamp;
        }
    }
    else
    {
        state = STATE_IDLE;
        last_timestamp = current_timestamp;
    }
}
