#ifndef ANALOG_HANDLER_H
#define ANALOG_HANDLER_H

#ifdef ESP32
#define INPUT_PIN_MODE INPUT_PULLDOWN
#define INPUT_INTERRUPT_MODE RISING
#define INPUT_ON_PIN 5
#define INPUT_OFF_PIN 18
#define INPUT_TEMP_PIN 14
#define INPUT_MODE_PIN 27
#define INPUT_ENERGY_PIN 19
#define INPUT_SPEED_PIN 23
#elif defined(ESP8266)
#define INPUT_PIN_MODE INPUT_PULLUP
#define INPUT_INTERRUPT_MODE FALLING
#define INPUT_ON_PIN 5       // D1
#define INPUT_OFF_PIN 12     // D6
#define INPUT_TEMP_PIN 13    // D7
#define INPUT_MODE_PIN 14    // D5
#define INPUT_ENERGY_PIN 0   // D3
#define INPUT_SPEED_PIN 2    // D4 
#endif

#define INPUT_DEBOUCE 500

void analog_handler_setup();

#endif