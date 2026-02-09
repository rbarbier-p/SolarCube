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

ISR(PCINT1_vect)
{
    static uint8_t last_pc3 = 1;          // previous state (pull-up = high)
    uint8_t now_pc3 = PINC & (1 << PC3);  // current state

    // Detect button press: HIGH -> LOW on PC3
    if (last_pc3 && !now_pc3)
    {
        _delay_ms(5);                    // small debounce
        if (!(PINC & (1 << PC3)))        // still low after debounce
        {
            current_animation++;
        }
    }

    last_pc3 = now_pc3;                 // store state for next interrupt
    PCIFR |= (1 << PCIF1);               // clear PCINT1 group flag
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

int main(void) {
  DDRC &= ~(1 << PC3);
  PORTC |= (1 << PC3);
  PCICR |= (1 << PCIE1);     // enable PCINT1 group
  PCMSK1 |= (1 << PCINT11);  // enable PCINT11 (PC3) specifically

  pin_init();
  UART_init();
  I2C_init(16, 15);
  DS1307_init();   // ensure oscillator ON
  UART_print_str("Starting DS1307...\r\n");

  DS1307_Time t = {
    .year = YEAR,
    .month = MONTH,
    .date = DAY,
    .hour = HOUR,
    .minute = MIN,
    .second = SEC,
    .day = 1
  };

  DS1307_setTime(&t);

  sei();
  while (1) {
    switch (current_animation) {
      case 0:
        solar_clock(t);
        break;
      case 1:
        vertical_sine_wave(0.5);
        break;
      case 2:
        vertical_sine_wave_rotated(0.5);
        break;
      case 3:
        continue;
      case 4:
        rain_animation(0.1);
        break;
      default:
        current_animation = 0;
        continue;
    }
  }
}

