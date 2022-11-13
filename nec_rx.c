#include "nec_rx.h"
#include <stddef.h>

void __attribute__((weak)) nec_button_callback(uint8_t address, uint8_t command, bool repeat) 
{
}

void __attribute__((weak)) nec16_button_callback(uint16_t address, uint8_t command, bool repeat)
{
}

// NEC Infra-red remote protocol
// https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol
// Line code: pulse distance encoding, modulated onto 38KHz carrier
// preamble: ~13600us (9.1ms pulse + 4.5ms space)
// bit 0: ~1125us (562us pulse, 562us space)
// bit 1: ~2250us (562us pulse, 1687us space)

// Protocol 4x8bits lsb first
// Address | Address# | Command | Command#
// Repeat code: ~11300us (9.1ms pulse + 2.2ms space), every ~108ms since start of last AGC pulse
// First repeat is ~40ms since end of data
// Subsequent repeat is ~97ms since end of last repeat

typedef enum
{
    STATE_IDLE,
    STATE_DATA,
    STATE_REPEAT,
    STATE_TYPE1_LOCK,
    STATE_LOCK2,
} decoder_state_machine_t;

// Decoding state machine to process the infra-red signal and generate bits
void necdecoder_decode_falling_edge(uint32_t timestamp)
{
    static decoder_state_machine_t state = STATE_IDLE;

    static uint32_t last_timestamp = 0;
    const uint32_t sinceLast = timestamp - last_timestamp;

    static uint32_t bits = 0;
    static int count = 0;

    static uint16_t last_address = 0;
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

        if (sinceLast >= 13200 && sinceLast <= 13700)
        {
            // preamble detected, next interrupt should be data bits
            state = STATE_DATA;

        }
        last_timestamp = timestamp;
    }
    else if (state == STATE_DATA)
    {
        last_timestamp = timestamp;

        if (sinceLast > 1000 && sinceLast < 1500)
        {
            // bit 0 detected
            //bits &= ~(1lu << count);
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

        if (count == 32)
        {
            // All bits received
            const uint8_t comm_i = (bits >> 24);
            const uint8_t comm = (bits >> 16);
            const uint8_t addr_i = (bits >> 8);
            const uint8_t addr = (uint8_t)(bits);

            if (comm != (uint8_t)~comm_i)
            {
                // error, reset
                state = STATE_IDLE;
                return;
            }
            
            last_command = comm;

            if (addr == (uint8_t)~addr_i)
            {
                // successfully verified and decoded the remote button
                last_address = addr;
                nec_button_callback(addr, comm, false);
            }
            else
            {
                const uint16_t addr_ext = ((uint16_t)addr_i << 8) | addr;
                nec16_button_callback(addr_ext, comm, false);

                last_address = addr_ext;
            }

            // Lock the decoder until the repeat comes in
            state = STATE_TYPE1_LOCK;
        }
    }
    else if (state == STATE_TYPE1_LOCK)
    {
        if (sinceLast < 39500)
        {
            // It is considered spurious and ignored
        }
        else if (sinceLast > 41000)
        {
            // too long has elapsed, reset
            state = STATE_IDLE;
            last_timestamp = timestamp;
        }
        else
        {
            // Next interrupt should hopefully be in ~11.3ms, if so a repeat has occurred
            state = STATE_REPEAT;
            last_timestamp = timestamp;
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
            last_timestamp = timestamp;
        }
        else
        {
            // Repeat has occurred, Next interrupt should hopefully be in ~108ms, if so a repeat has occurred
            state = STATE_LOCK2;
            last_timestamp = timestamp;

            // definitely a repeat command
            if (last_address > 255)
            {
                nec16_button_callback(last_address, last_command, true);
            }
            else
            {
                nec_button_callback((uint8_t)last_address, last_command, true);
            }
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
            last_timestamp = timestamp;
        }
        else
        {
            // Next interrupt should hopefully be in ~11.3ms, if so a repeat has occurred
            state = STATE_REPEAT;
            last_timestamp = timestamp;
            // TODO: extended mode repeats are not working
        }
    }
    else
    {
        state = STATE_IDLE;
        last_timestamp = timestamp;
    }
}

