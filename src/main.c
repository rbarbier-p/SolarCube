#include "mySoftI2C.h"
#include "myDS1307.h"
#include "UART.h"

#define RTCADDR 0x50

int main(void) {
  // Example: SDA = PB0, SCL = PB1
  I2C_init(8, 9);

  UART_init();
  UART_print_str("Starting DS1307...\r\n");

  DS1307_init();   // ensure oscillator ON

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

  while (1) {
    DS1307_getTime(&t);

    // Print format: HH:MM:SS DD/MM/YY
    UART_print_num(t.hour);
    UART_tx(':');
    UART_print_num(t.minute);
    UART_tx(':');
    UART_print_num(t.second);

    UART_print_str("   ");

    UART_print_num(t.date);
    UART_tx('/');
    UART_print_num(t.month);
    UART_tx('/');
    UART_print_num(t.year);

    UART_print_str("\r\n");

    _delay_ms(1000);
  }
}

