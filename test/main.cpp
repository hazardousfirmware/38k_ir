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

void necdecoder_handle_remote_button(uint8_t address, uint8_t command, bool repeat)
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
    timestamps.push_back(ms * 1000 + microseconds);
}

void reset()
{
    pwm = false;
    microseconds = 0;
    timestamps.clear();
    result_id = 0;
    result_cmd = 0;
}

int main()
{
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

    printf("expected id=%02x cmd=%02x ; actual id=%02x cmd=%02x\n", expect_id, expect_cmd, result_id, result_cmd);

    if (result_id != expect_id || result_cmd != expect_cmd)
    {
        printf("FAIL\n");
        abort();
    }

    printf("\n\n");

    reset();

    printf("NEC test\n");
    nec_register_functions(pwm_on1, pwm_off1, fake_delay);

    expect_id = 0xaa;
    expect_cmd = 0x33;
    send_nec_ircode(expect_id, expect_cmd);

    for (auto time: timestamps)
    {
        necdecoder_decode_falling_edge(time);
    }

    printf("expected id=%02x cmd=%02x ; actual id=%02x cmd=%02x\n", expect_id, expect_cmd, result_id, result_cmd);

    if (result_id != expect_id || result_cmd != expect_cmd)
    {
        printf("FAIL\n");
        abort();
    }

    return 0;
}
