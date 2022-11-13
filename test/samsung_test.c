#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "samsung_rx.h"

uint32_t result_addr;
uint16_t result_cmd;
uint8_t result_extra;

void samsung_button_callback_1(uint16_t address, uint16_t command)
{
    result_addr = address;
    result_cmd = command;
}

void samsung_button_callback_2(uint32_t address, uint16_t command, uint8_t extra)
{
    result_addr = address;
    result_cmd = command;
    result_extra = extra;
}


int main()
{
    // Type 2 timestamps
    uint32_t timestamps2[] = {
        13410,
        14540,
        16740,
        18970,
        20100,
        21220,
        23430,
        25660,
        26780,
        27920,
        29040,
        30180,
        31300,
        32430,
        34640,
        35780,
        36890,
        39110,
        41330,
        42460,
        43580,
        45800,
        48010,
        50230,
        52440,
        54660,
        56870,
        59100,
        60220,
        61350,
        62470,
        64700,
        65820,
        66950,
        69160,
        70290,
        72500,
        74720,
        76940,
        78060,
        80270,
        82500,
        83620,
        84140,
    };

    for (int i = 0; i < sizeof(timestamps2) / sizeof(uint32_t); i++)
    {
        samsung_decode_falling_edge(timestamps2[i]);
    }
    if (result_addr != 0x00f32066 || result_cmd != 0xba47 || result_extra != 0x1)
    {
        printf("Fail: expected addr=%04x, cmd=%04x, extra=%01x, actual id=%04x, cmd=%04x, extra=%01x\n", 0x00f32066, 0xba47, result_addr, result_cmd, result_extra);
        return 1;
    }


    // Some line time in the future
    samsung_decode_falling_edge(1000000);

    // Type 1 timestamps
    uint32_t timestamps1[] = {
        8910,
        11130,
        13360,
        15590,
        16690,
        17800,
        18910,
        20020,
        21120,
        23330,
        25550,
        27770,
        28870,
        29990,
        31090,
        32200,
        33310,
        34410,
        36620,
        38850,
        39950,
        41070,
        42170,
        43280,
        44380,
        46600,
        47710,
        48810,
        51030,
        53240,
        55450,
        57670,
        59880,
        60430,
    };
    for (int i = 0; i < sizeof(timestamps1) / sizeof(uint32_t); i++)
    {
        samsung_decode_falling_edge(timestamps1[i] + 1000000);
    }
    if (result_addr != 0x0707 || result_cmd != 0x06f9)
    {
        printf("Fail: expected id=%04x, cmd=%04x, actual id=%04x, cmd=%04x\n", 0x0707, 0x06f9, result_addr, result_cmd);
        return 1;
    }

    return 0;
}
