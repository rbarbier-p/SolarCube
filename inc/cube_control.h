#ifndef CUBE_CONTROL_H
#define CUBE_CONTROL_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LAY_CLOCK_PIN PD5 // pin D5 
#define LAY_LATCH_PIN PD3 // pin D3 
#define LAY_DATA_PIN  PD2 // pin D2 
#define LAY_PORT      PORTD 
#define LAY_DDR       DDRD

#define ROW_CLOCK_PIN PD7 // pin D7 
#define ROW_LATCH_PIN PD6 // pin D6 
#define ROW_DATA_PIN  PD4 // pin D4 
#define ROW_PORT      PORTD 
#define ROW_DDR       DDRD 

#define MULTIPLEX_DELAY 1
#define LSBFIRST 0
#define MSBFIRST 1

static const uint8_t shiftRegisterPinMap[] = {
  // -> first shift register skips pin 0 
  1,2,3,4,5,6,7,

  // -> second skips non
  8,9,10,11,12,13,14,15,

  // -> the rest skip pin 0
  17,18,19,20,21,22,23,

  25,26,27,28,29,30,31,

  33,34,35,36,37,38,39, 
};

void update_leds();
void pinInit();
void shiftOut(volatile uint8_t *port, uint8_t bitOrder, uint8_t dataPin, uint8_t clockPin, uint8_t val);
void selectLayer(uint8_t layer);
void selectRows(uint64_t rows);
uint8_t coordinates_to_led_num(uint8_t x, uint8_t y);
void clear_cube();
void select_led(uint8_t x, uint8_t y, uint8_t z);
void clear_led(uint8_t x, uint8_t y, uint8_t z);

#endif
