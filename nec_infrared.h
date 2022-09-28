#ifndef NEC_INFRARED_H_INCLUDED
#define NEC_INFRARED_H_INCLUDED

#include <stdint.h>

#define BUTTON_RELEASE_TIMEOUT_MS 110

struct nec_infrared_command_t
{
    uint8_t address;
    uint8_t command;
    void (*action)(void);
};

void necdecoder_decode_falling_edge(uint32_t current_timestamp);
void necdecoder_handle_remote_button(uint8_t address, uint8_t command);
void necdecoder_register_button_actions(const struct nec_infrared_command_t *command_list, int command_count);

#endif // NEC_INFRARED_H_INCLUDED
