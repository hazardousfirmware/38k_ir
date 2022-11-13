#include <stdio.h>
#include <stdint.h>
#include "panasonic_rx.h"

uint16_t result_id;
uint16_t result_cmd;

void panasonic_button_callback(uint16_t identifier, uint16_t command)
{
    result_id = identifier;
    result_cmd = command;
    printf("callback, identifier=0x%04x, command=0x%04x\n", identifier, command);
}


int main()
{
    printf("timestamps1:\n");
    uint32_t timestamps[] = {
        1875514370,
        1875519520,
        1875520400,
        1875522050,
        1875522930,
        1875523770,
        1875524620,
        1875525460,
        1875526310,
        1875527150,
        1875528000,
        1875528840,
        1875529680,
        1875530520,
        1875531370,
        1875533020,
        1875533900,
        1875534740,
        1875535590,
        1875536430,
        1875537270,
        1875538110,
        1875538950,
        1875540630,
        1875541500,
        1875543150,
        1875544020,
        1875544860,
        1875545710,
        1875546550,
        1875547390,
        1875548230,
        1875549070,
        1875549920,
        1875551570,
        1875552440,
        1875553280,
        1875554130,
        1875554970,
        1875556620,
        1875557490,
        1875558340,
        1875560000,
        1875560870,
        1875561710,
        1875562550,
        1875563390,
        1875564230,
        1875565070,
        1875566720,
    };

    for (int i = 0; i < sizeof(timestamps) / sizeof(uint32_t); i++)
    {
        panasonic_decode_falling_edge(timestamps[i]);
    }
    if (result_id != 0x00a0 || result_cmd != 0x8121)
    {
        printf("Fail: expected id=%04x, cmd=%04x, actual id=%04x, cmd=%04x\n", 0x00a0, 0x8121, result_id, result_cmd);
        return 1;
    }


    printf("timestamps2:\n");
    uint32_t timestamps2[] = {
        2989310,
        2994450,
        2995330,
        2996980,
        2997860,
        2998700,
        2999540,
        3000380,
        3001220,
        3002060,
        3002900,
        3003740,
        3004580,
        3005420,
        3006260,
        3007940,
        3008790,
        3009630,
        3010470,
        3011320,
        3012160,
        3013020,
        3013860,
        3014680,
        3015530,
        3017210,
        3018080,
        3018930,
        3019770,
        3020610,
        3021450,
        3022290,
        3023130,
        3023970,
        3024820,
        3025660,
        3026510,
        3027350,
        3028190,
        3029850,
        3030720,
        3031560,
        3032400,
        3033250,
        3034090,
        3034930,
        3035770,
        3037420,
        3038290,
        3039940,
    };
    
    for (int i = 0; i < sizeof(timestamps2) / sizeof(uint32_t); i++)
    {
        panasonic_decode_falling_edge(timestamps2[i]);
    }
    if (result_id != 0x0080 || result_cmd != 0xa020)
    {
        printf("Fail: expected id=%04x, cmd=%04x, actual id=%04x, cmd=%04x\n", 0x0080, 0xa020, result_id, result_cmd);
        return 1;
    }

    printf("timestamps3:\n");
    uint32_t timestamps3[] = {
        31874700,
        31879820,
        31880660,
        31882340,
        31883210,
        31884050,
        31884890,
        31885730,
        31886570,
        31887410,
        31888250,
        31889090,
        31889930,
        31890770,
        31891610,
        31893260,
        31894130,
        31894970,
        31895810,
        31896650,
        31897500,
        31898340,
        31900020,
        31901700,
        31902550,
        31904230,
        31905100,
        31905940,
        31906780,
        31907620,
        31908470,
        31909310,
        31910160,
        31911000,
        31911840,
        31913500,
        31914370,
        31915210,
        31916050,
        31916890,
        31917730,
        31919390,
        31920260,
        31921920,
        31922790,
        31923640,
        31925290,
        31926970,
        31927840,
        31928680,
    };

    for (int i = 0; i < sizeof(timestamps3) / sizeof(uint32_t); i++)
    {
        panasonic_decode_falling_edge(timestamps3[i]);
    }
    if (result_id != 0x00b0 || result_cmd != 0x3282)
    {
        printf("Fail: expected id=%04x, cmd=%04x, actual id=%04x, cmd=%04x\n", 0x00b0, 0x3282, result_id, result_cmd);
        return 1;
    }

    return 0;
}
