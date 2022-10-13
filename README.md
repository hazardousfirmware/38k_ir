# 38k_ir
Generic Cross platform library for decoding/encoding consurmer Infra Red Protocols

Compile into any embedded target to decode Infrared protocols.

Measures the time difference between falling edge interrupts to decode a command/address value from an IR remote control button press

## Supported protocols
- NEC (Decode only, encode coming soon)
- Panasonic (Encode Only)


## To get working:
1. Connect a 38KHz IR sensor to device GPIO,
1. enable a microsecond systick timer with timestamp count
1. falling edge interrupt on the GPIO line

### Decoding NEC protocol
1. implement function `void necdecoder_handle_remote_button(uint8_t address, uint8_t command)` to handle pressed button events
    - By default it is a *weak* function that just returns
1. call `necdecoder_decode_falling_edge` with timestamp from systick counter for every falling edge interrupt

### Encoding Panasonic protocol
1. call `panasonic_register_functions` and provide pointers to hardware specific functions for enabling PWM, disabling PWM, microsecond delay
1. call `send_panasonic_ircode` with 16 bit code to transmit

