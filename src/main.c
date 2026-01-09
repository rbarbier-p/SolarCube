#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"
#include "UART.h"
#include "animations.h"


void init_timer0(void) 
{
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B |= (1 << CS02) | (1 << CS00); // 1024 prescaler
	TIMSK0 |= (1 << OCIE0A); // ouput compare match A interrupt enable

	OCR0A = 20; // 0.016384s per timer0 cycle. / 1(s) = ~61
}

ISR(TIMER0_COMPA_vect) {
  update_leds();
}

void main () {
  DDRC &= ~(1 << PC3);
  PORTC |= (1 << PC3);
  pinInit();
  UART_init();
  sei();

  init_timer0();
 
  while (1) {
    diagonal_wave();
    vertical_sine_wave(0.4);
    wave_animation();
  }
}
