#include "samsung_rx.h"
#include <string.h>

void __attribute__((weak)) samsung_button_callback_1(uint16_t address, uint16_t command){}
void __attribute__((weak)) samsung_button_callback_2(uint32_t address, uint16_t command, uint8_t extra){}

/*
Samsung IR Protocol
    Similar to standard NEC protocol but with the following modifications:

Type 1: (DVD)
- AGC: 4.5ms on / 4.5ms off
- 16 bits address + 16 bits data
- No repeat pulse, instead entire frame is repeated
- ~46.73ms between repeats (46.14 excl stop bit)

Type 2: (TV)
- 42 bits data (24 bits address + 16 bits command + 2 extra bits (possibly CRC) )
- No repeat pulse, instead entire frame is repeated
- ~22.97ms between repeats (22.44 excl stop bit)
*/


typedef enum
{
    STATE_IDLE,
    STATE_TYPE1_DATA,
    STATE_TYPE2_DATA,
    STATE_TYPE1_LOCK,
    STATE_TYPE2_LOCK,
} decoder_state_machine_t;

// Decoding state machine to process the infra-red signal and generate bits
void samsung_decode_falling_edge(uint32_t timestamp)
{
    static decoder_state_machine_t state = STATE_IDLE;

    static uint32_t last_timestamp = 0;
    const uint32_t sinceLast = timestamp - last_timestamp;

    static uint8_t bytes[6] = {0}; // Type 2 has 42 bits of data

    static uint8_t bits = 0;
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
        memset(bytes, 0, sizeof(bytes));

        if (sinceLast >= 13300 && sinceLast <= 13700)
        {
            // variant 2 protocol preamble detected, next interrupt should be data bits
            state = STATE_TYPE2_DATA;
        }
        else if (sinceLast >= 8850 && sinceLast <= 9150)
        {
            // variant 1 protocol preamble detected, next interrupt should be data bits
            state = STATE_TYPE1_DATA;
        }
        last_timestamp = timestamp;
    }
    else if (state == STATE_TYPE1_DATA || state == STATE_TYPE2_DATA)
    {
        last_timestamp = timestamp;
        if (sinceLast > 1000 && sinceLast < 1500)
        {
            // bit 0 detected
            // bits &= ~(1 << (count & 0x07));
            count++;
        }
        else if (sinceLast > 2000 && sinceLast < 2500)
        {
            // bit 1 detected
            bits |= (1 << (count & 0x07));
            count++;
        }
        else
        {
            state = STATE_IDLE;
            return;
        }
    }
    else if (state == STATE_TYPE2_LOCK)
    {
        // block from handling falling edge for at least 23us
        if (sinceLast > 22000)
        {
            state = STATE_IDLE;
            last_timestamp = timestamp;
        }
    }
    else if (state == STATE_TYPE1_LOCK)
    {
        // block from handling falling edge for at least 46us
        if (sinceLast > 46600)
        {
            state = STATE_IDLE;
            last_timestamp = timestamp;
        }
    }
    else
    {
        state = STATE_IDLE;
        last_timestamp = timestamp;
        return;
    }

    if ((count & 0x07) == 0x00 && count > 0)
    {
        // Multiple of 8 bits
        int index = (count >> 3) - 1; // index = (count / 8) - 1
        bytes[index] = bits;
        bits = 0;
    }

    if (state == STATE_TYPE1_DATA && count == 32)
    {
        // All bits received
        uint16_t addr = bytes[0] << 8 | bytes[1];
        uint16_t cmd = bytes[2] << 8 | bytes[3];
        
        samsung_button_callback_1(addr, cmd);

        // Lock the decoder until the repeat comes in
        state = STATE_TYPE1_LOCK;
    }

    if (state == STATE_TYPE2_DATA && count == 42)
    {
        bytes[5] = bits << 6;

        uint32_t address = bytes[0] | (bytes[1] << 8) | ((uint32_t)bytes[2] << 16);
        uint16_t cmd = bytes[3] | (bytes[4] << 8);
        uint8_t extra = bytes[5] >> 6;

        samsung_button_callback_2(address, cmd, extra);

        // Lock the decoder until repeat comes in
        state = STATE_TYPE2_LOCK;

        //TODO: fix repeated commands
    }
}
