#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"
#include "UART.h"
#include "animations.h"
#include "times.h"
#include "mySoftI2C.h"
#include "myDS1307.h"

#define RTCADDR 0x50

volatile uint8_t current_animation = 0;
volatile uint16_t anim_timer_ms = 0;

void timer0_init(void) {
  TCCR0A = (1 << WGM01);
  TCCR0B = 0;

  OCR0A = 249;

  TIMSK0 &= ~(1 << OCIE0A);
  TCNT0 = 0;
}

void timer0_ON(void) {
  TCNT0 = 0;
  anim_timer_ms = 0;

  TIMSK0 |= (1 << OCIE0A);
  TCCR0B = (1 << CS01) | (1 << CS00);
}

void timer0_OFF(void) {
  TCCR0B = 0;
  TIMSK0 &= ~(1 << OCIE0A);
}

ISR(TIMER0_COMPA_vect) {
  anim_timer_ms++;

  if (anim_timer_ms >= 10000) {
    anim_timer_ms = 0;
    current_animation++;
  }
}

ISR(PCINT1_vect)
{
  static uint8_t last_pc3 = 1;
  uint8_t now_pc3 = PINC & (1 << PC3);

  if (last_pc3 && !now_pc3)
  {
    _delay_ms(5);
    if (!(PINC & (1 << PC3)))
      current_animation++;
  }

  last_pc3 = now_pc3;
  PCIFR |= (1 << PCIF1);
}

void print_time(DS1307_Time *t) {
  // Print format: HH:MM:SS DD/MM/YY
  UART_print_num(t->hour);
  UART_tx(':');
  UART_print_num(t->minute);
  UART_tx(':');
  UART_print_num(t->second);
  UART_print_str("   ");
  UART_print_num(t->date);
  UART_tx('/');
  UART_print_num(t->month);
  UART_tx('/');
  UART_print_num(t->year);
  UART_print_str("\r\n");
}

void cycle() {
  const uint8_t anim_index = current_animation;

  timer0_ON();
  current_animation = 0;

  while (PINC & (1 << PC3)) {
    switch (current_animation) {
      case 0: vertical_sine_wave(0.5); break;
      case 1: vertical_sine_wave_rotated(0.5); break;
      case 2: rain_animation(0.1); break;
      case 3: expanding_cube(); break;
      case 4: moving_cube_random(); break;
      case 5: moving_42(); break;
      default: current_animation = 0; continue;
    }
  }

  timer0_OFF();
  current_animation = 2;
  anim_timer_ms = 0;
} 

int main(void) {
  DDRC &= ~(1 << PC3);
  PORTC |= (1 << PC3);
  PCICR |= (1 << PCIE1);
  PCMSK1 |= (1 << PCINT11);

  pin_init();
  UART_init();
  I2C_init(16, 15);
  DS1307_init();
  timer0_init();

  /*DS1307_Time t = {
    .year = YEAR,
    .month = MONTH,
    .date = DAY,
    .hour = HOUR,
    .minute = MIN,
    .second = SEC,
    .day = 1
  };
  DS1307_setTime(&t);*/

  DS1307_Time t;
  DS1307_getTime(&t);

  sei();
  while (1) {
    switch (current_animation) {
      case 0: solar_clock(t); break;
      case 1: cycle(); break;
      case 2: vertical_sine_wave(0.5); break;
      case 3: vertical_sine_wave_rotated(0.5); break;
      case 4: rain_animation(0.1); break;
      case 5: expanding_cube(); break;
      case 6: moving_cube_random(); break;
      case 7: moving_42(); break;
      default: current_animation = 0; continue;
    }
  }
}

