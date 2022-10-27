#include "panasonic_rx.h"

void __attribute__((weak)) panasonic_button_callback(uint16_t identifier, uint16_t command)
{
}

/*
===start===
on for 3600us
off for 1200 us

===data=== (16bit identifier + 0x4004 + 16 bit code)
state 0 = (720 - 880)us, low for 480us, high for 240us (440)
state 1 = (1680-1760)us, low for 1200us, high for (480 to 560)us

=== end / between frames ===
off for 73ms
*/

typedef enum 
{
    STATE_IDLE,
    STATE_ADDR,
    STATE_MAGIC,
    STATE_DATA,
    STATE_LOCK
} decoder_state_machine_t;

#define MIDDLE_CONSTANT (uint16_t)0x4004

void panasonic_decode_falling_edge(uint32_t current_timestamp)
{
    static decoder_state_machine_t state = STATE_IDLE;

    static uint32_t last_timestamp = 0;
    const uint32_t sinceLast = current_timestamp - last_timestamp;

    static uint16_t cmd = 0;
    static uint16_t id = 0;
    static int count = 0;

    if (sinceLast < 100)
    {
        // Spurious, ignore
        return;
    }

    if (state == STATE_IDLE)
    {
        cmd = 0;
        id = 0;
        count = 0;

        if (sinceLast >= 4800 && sinceLast <= 5300) // ~4800us
        {
            state = STATE_MAGIC;
        }
        
        last_timestamp = current_timestamp;
    }
    else if (state == STATE_MAGIC)
    {
        // Sample 16bits to decode magic number 0x4004
        if (sinceLast > 700 && sinceLast < 900)
        {
            // bit 0 detected
            count++;
            cmd &= ~(1 << count);
        }
        else if (sinceLast > 1600 && sinceLast < 1820)
        {
            // bit 1 detected
            count++;
            cmd |= (1 << count);
        }
        else
        {
            state = STATE_IDLE;
            count = 0;
        }

        if (count == 16)
        {
            // last bit detected
            state = (cmd == MIDDLE_CONSTANT) ? STATE_ADDR : STATE_IDLE;
            count = 0;
            cmd = 0;
        }

        last_timestamp = current_timestamp;
    }
    else if (state == STATE_ADDR)
    {
        // Sample 16 bits and check for number identifying the device
        if (sinceLast > 700 && sinceLast < 900)
        {
            // bit 0 detected
            count++;
            id &= ~(1 << count);
        }
        else if (sinceLast > 1600 && sinceLast < 1820)
        {
            // bit 1 detected
            count++;
            id |= (1 << count);
        }
        else
        {
            state = STATE_IDLE;
            count = 0;
        }

        if (count == 16)
        {
            state = STATE_DATA;
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
            count++;
            cmd &= ~(1 << count);
        }
        else if (sinceLast > 1600 && sinceLast < 1820)
        {
            // bit 1 detected
            count++;
            cmd |= (1 << count);
        }
        else
        {
            state = STATE_IDLE;
            count = 0;
        }

        if (count == 16)
        {
            // last bit detected
            panasonic_button_callback(id, cmd);
            state = STATE_LOCK;
            count = 0;
            cmd = 0;
            id = 0;
        }

        last_timestamp = current_timestamp;
    }
    else if (state == STATE_LOCK)
    {
        // block from handling falling edge for at least 73us
        if (sinceLast > 73000)
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
