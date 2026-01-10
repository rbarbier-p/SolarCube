#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"
#include "UART.h"
#include "animations.h"

volatile uint8_t current_animation = 0;

ISR(PCINT1_vect) {
  static int pressed = 0;
  pressed = (pressed + 1) % 2; // skip release

  if (pressed == 1) {
    current_animation++;
  }

  _delay_ms(2);                // simple debounce
  PCIFR |= (1 << PCIF1);        // clear pending interrupt for PCINT1 group
}

void main () {
  DDRC &= ~(1 << PC3);
  PORTC |= (1 << PC3);
  PCICR |= (1 << PCIE1);     // enable PCINT1 group
  PCMSK1 |= (1 << PCINT11);  // enable PCINT11 (PC3) specifically

  pin_init();
  UART_init();
  sei();

  while (1) {
    switch (current_animation) {
      case 0:
        vertical_sine_wave(0.5);
        break;
      case 1:
        vertical_sine_wave_rotated(0.5);
        break;
      case 2:
        drop_adder();
        break;
      case 3:
        rain_animation(0.1);
        break;
      default:
        current_animation = 0;
        break;
    }
  }
}

