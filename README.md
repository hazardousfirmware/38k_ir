# necinfrared
Cross platform NEC Infrared protocol Decoding library

Generic library to compile into any embedded target to decode NEC Infrared protocol.

Measures the time difference between falling edge interrupts to decode a command/address value from an IR remote control button press

To get working:
1. Connect a 38KHz IR sensor to device GPIO,
1. enable a microsecond systick timer with timestamp count
1. falling edge interrupt on the GPIO line
1. implement function `void necdecoder_handle_remote_button(uint8_t address, uint8_t command)` to handle pressed button events
    - By default it is a *weak* function that just returns
1. call `necdecoder_decode_falling_edge` with timestamp from systick counter for every falling edge interrupt
1. call `necdecoder_register_button_actions` to register actions for a cmd/address from a remote button press

