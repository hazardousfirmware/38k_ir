# 38k_ir
Generic Cross platform library for decoding/encoding consumer Infra Red Protocols

Compile into any embedded target to decode Infrared protocols.

Measures the time difference between falling edge interrupts to decode a command/address value from an IR remote control button press

## Supported protocols
- NEC (Decode only, encode coming soon)
- Panasonic (Encode and Decode)

## Hardware prerequisites
- GPIO output pin with 38KHz PWM (to infrared LED, driver transistor is recommended)
- GPIO input pin with falling edge interrupt (Data pin of 38KHz IR deocder IC)
- Microsecond accurate timer or system tick to count timestamp (10us or greater may still work, remember to multiply time count on a falling edge)


## To get working:
1. connect a 38KHz IR sensor to device GPIO,
1. enable a microsecond systick timer with timestamp count
1. falling edge interrupt on the GPIO line
1. configure PWM at 38KHz (30% to 50% duty)

### Decoding NEC protocol
1. implement function `void necdecoder_handle_remote_button(uint8_t address, uint8_t command)` to handle pressed button events
    - By default it is a *weak* function that does nothing and returns
1. call `necdecoder_decode_falling_edge(uint32_t current_timestamp)` with timestamp from systick counter for every falling edge interrupt on the IR sensor pin

### Decoding Panasonic protocol
1. implement function `void panasonic_button_callback(uint16_t command)` to handle pressed button events
    - By default it is a *weak* function that does nothing and returns
1. call `panasonic_decode_falling_edge(uint32_t current_timestamp)` with timestamp from systick counter for every falling edge interrupt on the IR sensor pin

### Encoding Panasonic protocol
1. call `panasonic_register_functions` and provide pointers to hardware specific functions for enabling PWM, disabling PWM, microsecond delay
1. call `send_panasonic_ircode` with 16 bit code to transmit

## Gotchas
- The decoding code might be slow in an IRQ, use a queue or ring buffer to hold interrupt events
