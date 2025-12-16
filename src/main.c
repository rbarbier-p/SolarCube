#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"
#include "UART.h"

uint64_t frame[6] = {0};

void init_timer0(void) 
{
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B |= (1 << CS02) | (1 << CS00); // 1024 prescaler
	TIMSK0 |= (1 << OCIE0A); // ouput compare match A interrupt enable

	OCR0A = 255; // 0.016384s per timer0 cycle. / 1(s) = ~61
}

ISR(TIMER0_COMPA_vect)
{
  update_leds(frame);
}


void select_led(uint8_t x, uint8_t y, uint8_t z) {
  frame[z] |= 1ULL << shiftRegisterPinMap[coordinates_to_led_num(x, y)];
}

void main () {
  pinInit();
  UART_init();
  sei();

  select_led(0, 0, 0);
  select_led(1, 1, 1);
  select_led(2, 2, 2);
  select_led(3, 3, 3);
  select_led(4, 4, 4);
  select_led(5, 5, 5);

  while (1) {
    for (uint8_t i = 0; i < 6; i++) {
      selectRows(0);
      selectLayer(i + 1);
      selectRows(frame[i]);
      _delay_ms(1);
    }
  }
}
