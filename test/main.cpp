#include <iostream>
#include <vector>
#include <cstdint>

#include "panasonic_rx.h"
#include "panasonic_tx.h"

#include "nec_rx.h"
#include "nec_tx.h"

std::vector<uint32_t> timestamps;
bool pwm = false;
uint16_t result_id = 0;
uint16_t result_cmd = 0;
uint32_t microseconds = 0;

void panasonic_button_callback(uint16_t identifier, uint16_t command)
{
    result_id = identifier;
    result_cmd = command;
}

void nec_button_callback(uint8_t address, uint8_t command, bool repeat)
{
    result_id = address;
    result_cmd = command;
}

void nec16_button_callback(uint16_t address, uint8_t command, bool repeat)
{
    result_id = address;
    result_cmd = command;
}

void pwm_on1()
{
    pwm = true;
}

void pwm_off1()
{
    pwm = false;
}

void fake_delay(unsigned int us)
{
    microseconds += us;
    if (!pwm)
    {
        timestamps.push_back(microseconds);
    }
}

void fake_delay2(uint32_t ms)
{
}

void reset()
{
    pwm = false;
    timestamps.clear();
    result_id = 0;
    result_cmd = 0;
}

int main()
{
    // Panasonic test

    printf("Panasonic test\n");
    reset();
    panasonic_register_functions(pwm_on1, pwm_off1, fake_delay, fake_delay2);

    uint16_t expect_id = 0x0080; // panasonic TV power button
    uint16_t expect_cmd = 0xbd3d;

    send_panasonic_ircode(expect_id, expect_cmd);

    for (auto time: timestamps)
    {
        panasonic_decode_falling_edge(time);
    }

    printf("expected id=0x%02x cmd=0x%02x ; actual id=0x%02x cmd=0x%02x\n", expect_id, expect_cmd, result_id, result_cmd);

    if (result_id != expect_id || result_cmd != expect_cmd)
    {
        printf("FAIL\n");
        abort();
    }

    printf("\n\n");

    // NEC test

    reset();
    necdecoder_decode_falling_edge(microseconds);

    printf("NEC test\n");
    nec_register_functions(pwm_on1, pwm_off1, fake_delay);

    expect_id = 0xaa;
    expect_cmd = 0x33;
    send_nec_ircode(expect_id, expect_cmd);

    for (auto time: timestamps)
    {
        necdecoder_decode_falling_edge(time);
    }

    printf("expected id=0x%02x cmd=0x%02x ; actual id=0x%02x cmd=0x%02x\n", expect_id, expect_cmd, result_id, result_cmd);

    if (result_id != expect_id || result_cmd != expect_cmd)
    {
        printf("FAIL\n");
        abort();
    }
    printf("\n\n");
    microseconds += 20000; // sufficiently long time after first "button press" so it resets and doesn't expect a repeat
    necdecoder_decode_falling_edge(microseconds);
    
    // extended NEC test

    reset();
    printf("NEC (extended) test\n");

    expect_id = 0x0586;
    expect_cmd = 0xCC;
    send_nec16_ircode(expect_id, expect_cmd);

    for (auto time: timestamps)
    {
        necdecoder_decode_falling_edge(time);
    }

    printf("expected id=0x%04x cmd=0x%02x ; actual id=0x%04x cmd=0x%02x\n", expect_id, expect_cmd, result_id, result_cmd);

    if (result_id != expect_id || result_cmd != expect_cmd)
    {
        printf("FAIL\n");
        abort();
    }

    return 0;
}
