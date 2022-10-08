#include "nec_infrared.h"
#include <stddef.h>

uint8_t swap_bit_order(uint8_t value);

void necdecoder_handle_remote_button(uint8_t address, uint8_t command) __attribute__((weak))
{
}

// NEC Infra-red remote protocol
// https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol
// Line code:
// preamble: ~13600us (9.1ms pulse + 4.5ms space) [13400 - 13700]
// bit 0: ~1125us (562us pulse, 562us space) [1110 - 1150]
// bit 1: ~2250us (562us pulse, 1687us space) [2240 - 2290]

// Protocol 4x8bits lsb first
// Address | Address# | Command | Command#
// Repeat code: ~11300us (9.1ms pulse + 2.2ms space) [11000 - 11400], every ~108ms since start of last AGC pulse
// First repeat is ~40ms [39000 - 41000] since end of data
// Subsequent repeat is ~97ms [96000 - 98000] since end of last repeat

// Process the infra-red signal and generate bits
// Decoding state machine

enum decoder_state_machine_t
{
    STATE_IDLE,
    STATE_DATA,
    STATE_REPEAT,
    STATE_LOCK1,
    STATE_LOCK2,
};

void necdecoder_decode_falling_edge(uint32_t current_timestamp)
{
    static enum decoder_state_machine_t state = STATE_IDLE;

    static uint32_t last_timestamp = 0;
    const uint32_t sinceLast = current_timestamp - last_timestamp;

    static uint32_t bits = 0;
    static int count = 0;


    static uint8_t last_address = 0;
    static uint8_t last_command = 0;

    if (sinceLast < 800)
    {
        // Its something stupid, don't even bother
        return;
    }


    if (state == STATE_IDLE)
    {
        bits = 0x0;
        count = 0;

        if (sinceLast >= 13400 && sinceLast <= 13700)
        {
            // preamble detected, next interrupt should be data bits
            state = STATE_DATA;

        }
        last_timestamp = current_timestamp;
    }
    else if (state == STATE_DATA)
    {
        last_timestamp = current_timestamp;

        if (sinceLast > 1000 && sinceLast < 1500)
        {
            // bit 0 detected
            bits <<= 1;
            bits &= ~(1 << 0);
            count++;
        }
        else if (sinceLast > 2000 && sinceLast < 2500)
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
            // All bits received
            const uint8_t addr = (bits >> 24);
            const uint8_t addr_i = (bits >> 16);
            const uint8_t comm = (bits >> 8);
            const uint8_t comm_i = (uint8_t)(bits);

            if (addr == (uint8_t)~addr_i && comm == (uint8_t)~comm_i)
            {
                // successfully verified and decoded the remote button
                last_address = addr;
                last_command = comm;
                necdecoder_handle_remote_button(addr, comm);

                // Lock the decoder until the repeat comes in
                state = STATE_LOCK1;
            }
            else
            {
                // reset the state machine
                state = STATE_IDLE;
            }
        }
    }
    else if (state == STATE_LOCK1)
    {
        if (sinceLast < 39500)
        {
            // It is considered spurious and ignored
        }
        else if (sinceLast > 41000)
        {
            // too long has elapsed, reset
            state = STATE_IDLE;
            last_timestamp = current_timestamp;
        }
        else
        {
            // Next interrupt should hopefully be in ~11.3ms, if so a repeat has occurred
            state = STATE_REPEAT;
            last_timestamp = current_timestamp;
        }
    }
    else if (state == STATE_REPEAT)
    {
        if (sinceLast < 11000)
        {
            // It is considered spurious and ignored
        }
        else if (sinceLast > 11500)
        {
            // too long has elapsed, reset
            state = STATE_IDLE;
            last_timestamp = current_timestamp;
        }
        else
        {
            // Repeat has occurred, Next interrupt should hopefully be in ~108ms, if so a repeat has occurred
            state = STATE_LOCK2;
            last_timestamp = current_timestamp;

            // definitely a repeat command
            necdecoder_handle_remote_button(last_address, last_command);
        }
    }
    else if (state == STATE_LOCK2)
    {
        if (sinceLast < 96000)
        {
            // It is considered spurious and ignored
        }
        else if (sinceLast > 98000)
        {
            // too long has elapsed, reset
            state = STATE_IDLE;
            last_timestamp = current_timestamp;
        }
        else
        {
            // Next interrupt should hopefully be in ~11.3ms, if so a repeat has occurred
            state = STATE_REPEAT;
            last_timestamp = current_timestamp;
        }
    }
    else
    {
        state = STATE_IDLE;
        last_timestamp = current_timestamp;
    }
}

inline void necdecoder_register_button_handler(void (*func)(uint8_t, uint8_t))
{
    necdecoder_handle_remote_button = func;
}

inline uint8_t swap_bit_order(uint8_t value)
{
    value = (value & 0xF0) >> 4 | (value & 0x0F) << 4;
    value = (value & 0xCC) >> 2 | (value & 0x33) << 2;
    value = (value & 0xAA) >> 1 | (value & 0x55) << 1;

    return value;
}
