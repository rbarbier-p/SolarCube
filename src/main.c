#include "mySoftI2C.h"
#include "myDS1307.h"

#define RTCADDR 0x50

// -----------------------------------------------------
// UART FUNCTIONS
// -----------------------------------------------------
void UART_init(uint16_t baud) {
    uint16_t ubrr = (F_CPU / (16UL * baud)) - 1;
    UBRR0H = (ubrr >> 8);
    UBRR0L = ubrr;
    UCSR0B = (1 << TXEN0);           // Enable TX
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // 8N1
}

void UART_sendChar(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void UART_print(const char *s) {
    while (*s) UART_sendChar(*s++);
}

void UART_printNum(uint8_t n) {
    UART_sendChar('0' + (n / 10));
    UART_sendChar('0' + (n % 10));
}

// -----------------------------------------------------
// MAIN
// -----------------------------------------------------
int main(void) {
    // Example: SDA = PB0, SCL = PB1
    I2C_init(&PORTB, &DDRB, &PINB, PB0,
             &PORTB, &DDRB, &PINB, PB1);

    UART_init(9600);
    UART_print("Starting DS1307...\r\n");

    DS1307_init();   // ensure oscillator ON
    
    DDS1307_Time t = {
        .second = 0,
        .minute = 15,
        .hour   = 18,
        .day    = 2,
        .date   = 25,
        .month  = 12,
        .year   = 24
    };
    DS1307_setTime(&t);

    while (1) {
        DS1307_getTime(&t);

        // Print format: HH:MM:SS DD/MM/YY
        UART_printNum(t.hour);
        UART_sendChar(':');
        UART_printNum(t.minute);
        UART_sendChar(':');
        UART_printNum(t.second);

        UART_print("   ");

        UART_printNum(t.date);
        UART_sendChar('/');
        UART_printNum(t.month);
        UART_sendChar('/');
        UART_printNum(t.year);

        UART_print("\r\n");

        _delay_ms(1000);
    }
}

