# DCC-EX Tri Throttle

A serial throttle using three rotary encoders, an OLED, and a 4 x 3 keypad designed for use with DCC-EX EX-CommandStation.

## Hardware

- STM32F103C8 Bluepill
- SPI or I2C OLED
- 4 x 3 Keypad
- 3 x mouse wheel style rotary encoders with micro switches

## Pins

### Rotary Encoder connections

| Encoder | DT | CLK | Button |
|---------|----|-----|--------|
| 1 | PC14 | PC15 | PA0 |
| 2 | PB0 | PB1 | PA1 |
| 3 (Bluepill) | PB10 | PB11 | PA2 |
| 3 (Blackpill) | PB12 | PB13 | PA2 |

### SPI

- DC - PA3
- CS - PA4
- MOSI - PA7
- MISO - PA6
- SCK - PA5

### Serial

- RX -PA10
- TX - PA9

(Write to serial pins with Serial1)

### Keypad

- PIN1 - PB3
- PIN2 - PB4
- PIN3 - PB5
- PIN4 - PB6
- PIN5 - PB7
- PIN6 - PB8
- PIN7-  PB9
