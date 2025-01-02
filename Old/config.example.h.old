/*
 * This is an example config.h file for the DCC-EX serial throttle.
*/

/////////////////////////////////////////////////////////////////////////////////////
//  Ensure only one instance of config.h is loaded.
// 
#ifndef CONFIG_H
#define CONFIG_H

/////////////////////////////////////////////////////////////////////////////////////
//  Number of times to retry connecting at startup before reporting connection error.
//  A connection retry occurs every second, and can take some time if using WiFi on
//  a CommandStation as it takes a few seconds at least to connect to WiFi.
// 
#define CONNECT_RETRIES 20

/////////////////////////////////////////////////////////////////////////////////////
//  Number of throttles to setup.
// 
#define NUM_THROTTLES 3

/////////////////////////////////////////////////////////////////////////////////////
//  Rotary encoder setup.
// 
#define ENC1_DT PC14
#define ENC1_CLK PC15
#define ENC2_DT PB0
#define ENC2_CLK PB1
#if defined(ARDUINO_BLUEPILL_F103C8)
#define ENC3_DT PB10
#define ENC3_CLK PB11
#elif defined(ARDUINO_BLACKPILL_F411CE)
#define ENC3_DT PB12
#define ENC3_CLK PB13
#endif

/////////////////////////////////////////////////////////////////////////////////////
//  Button setup.
// 
#define BUTTON1 PA0
#define BUTTON2 PA1
#define BUTTON3 PA2

/////////////////////////////////////////////////////////////////////////////////////
//  OLED configuration.
//  Use this section for SPI, and the section further down for I2C.
//  Comment out the section not in use.
// 
// Uncomment from the line below for SPI
#define USE_OLED_SPI
#define CS_PIN  PA4
#define DC_PIN  PA3
// Pinout reference lines below only, do not uncomment
// SCK - PA5
// MISO - PA6
// MOSI - PA7

// Comment out above and uncomment from the line below for I2C
// #define USE_OLED_I2C
// #define OLED_ADDRESS 0x3C
// I2C definition ends here, don't uncomment this line

/////////////////////////////////////////////////////////////////////////////////////
//  Keypad configuration.
// 
#define KEYPAD_DEBOUNCE 10
#define KEYPAD_HOLD 500

#define KEYPAD_PIN1 PB3
#define KEYPAD_PIN2 PB4
#define KEYPAD_PIN3 PB5
#define KEYPAD_PIN4 PB6
#define KEYPAD_PIN5 PB7
#define KEYPAD_PIN6 PB8
#define KEYPAD_PIN7 PB9

#endif
