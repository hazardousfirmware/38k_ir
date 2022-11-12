#include "samsung_rx.h"

void __attribute__((weak)) samsung_button_callback_1(uint16_t address, uint16_t command){}
void __attribute__((weak)) samsung_button_callback_2(uint32_t address, uint16_t command){}

/*
Samsung IR Protocol
    Similar to standard NEC protocol but with the following modifications:

Type 1: (DVD)
- AGC: 4.5ms on / 4.5ms off
- 16 bits address + 16 bits data
- No repeat pulse, instead entire frame is repeated
- ~46.73ms between repeats (46.14 excl stop bit)

Type 2: (TV)
- 42 bits data (26 bits address + 16 bits command)
- No repeat pulse, instead entire frame is repeated
- ~22.97ms between repeats (22.44 excl stop bit)
*/


typedef enum
{
    STATE_IDLE,
    STATE_TYPE1_DATA,
    STATE_TYPE2_DATA1,
    STATE_TYPE2_DATA2,
    STATE_TYPE1_LOCK,
    STATE_TYPE2_LOCK,
} decoder_state_machine_t;

// Decoding state machine to process the infra-red signal and generate bits
void samsung_decode_falling_edge(uint32_t timestamp)
{
    static decoder_state_machine_t state = STATE_IDLE;

    static uint32_t last_timestamp = 0;
    const uint32_t sinceLast = timestamp - last_timestamp;

    static uint32_t bits = 0;
    static uint32_t data1 = 0;
    static int count = 0;

    if (sinceLast < 800)
    {
        // Its something stupid, don't even bother
        return;
    }

    if (state == STATE_IDLE)
    {
        bits = 0x0;
        count = 0;
        data1 = 0;

        if (sinceLast >= 13300 && sinceLast <= 13700)
        {
            // variant 2 protocol preamble detected, next interrupt should be data bits
            state = STATE_TYPE2_DATA1;
        }
        else if (sinceLast >= 8900 && sinceLast <= 9100)
        {
            // variant 1 protocol preamble detected, next interrupt should be data bits
            state = STATE_TYPE1_DATA;
        }
        last_timestamp = timestamp;
    }
    else if (state == STATE_TYPE1_DATA || state == STATE_TYPE2_DATA1 || state == STATE_TYPE2_DATA2)
    {
        if (sinceLast > 1000 && sinceLast < 1500)
        {
            // bit 0 detected
            bits &= ~(1lu << count);
            count++;
        }
        else if (sinceLast > 2000 && sinceLast < 2500)
        {
            // bit 1 detected
            bits |= (1lu << count);
            count++;
        }
        else
        {
            state = STATE_IDLE;
            count = 0;
        }
        last_timestamp = timestamp;
    }
    else if (state == STATE_TYPE1_LOCK)
    {
        // block from handling falling edge for at least 23us
        if (sinceLast > 22400)
        {
            state = STATE_IDLE;
            last_timestamp = timestamp;
        }
    }
    else if (state == STATE_TYPE2_LOCK)
    {
        // block from handling falling edge for at least 46us
        if (sinceLast > 46100)
        {
            state = STATE_IDLE;
            last_timestamp = timestamp;
        }
    }
    else
    {
        state = STATE_IDLE;
        last_timestamp = timestamp;
    }

    if (state == STATE_TYPE1_DATA && count == 32)
    {
        // All bits received
        uint16_t addr = bits & 0xffff;
        uint16_t cmd = bits >> 16;
        
        samsung_button_callback_1(addr, cmd);

        // Lock the decoder until the repeat comes in
        state = STATE_TYPE1_LOCK;
    }

    if (state == STATE_TYPE2_DATA1 && count == 32)
    {
        data1 = bits;
        state = STATE_TYPE2_DATA2;
        count = 0;
        bits = 0;
    }

    if (state == STATE_TYPE2_DATA2 && count == 10)
    {
        uint16_t data2 = bits << 6;
        bits |= (data1 >> 26);

        data1 &= 0x3FFFFFF;
        samsung_button_callback_2(data1, data2);
        count = 0;
        bits = 0;
        state = STATE_TYPE2_LOCK;
    }

    //TODO: fix type 1 and repeated commands

}
