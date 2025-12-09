#include "myDS1307.h"

// ---------------------- BCD UTILS ----------------------
static uint8_t dec_to_bcd(uint8_t d) {
    return ((d / 10) << 4) | (d % 10);
}

static uint8_t bcd_to_dec(uint8_t b) {
    return (b >> 4) * 10 + (b & 0x0F);
}

// ---------------------- INIT ----------------------
void DS1307_init(void) {
    // Ensure oscillator is running (CH bit = 0)
    I2C_start();
    I2C_write(0x68 << 1);   // write
    I2C_write(0x00);        // seconds register
    I2C_write(0x00);        // CH=0 and seconds=0
    I2C_stop();
}

// ---------------------- SET TIME ----------------------
void DS1307_setTime(DS1307_Time *t) {
    I2C_start();
    I2C_write(0x68 << 1); // write
    I2C_write(0x00);      // starting at seconds

    I2C_write(dec_to_bcd(t->second) & 0x7F);  // CH=0
    I2C_write(dec_to_bcd(t->minute));
    I2C_write(dec_to_bcd(t->hour) & 0x3F);    // force 24h mode
    I2C_write(dec_to_bcd(t->day));
    I2C_write(dec_to_bcd(t->date));
    I2C_write(dec_to_bcd(t->month));
    I2C_write(dec_to_bcd(t->year));

    I2C_stop();
}

// ---------------------- READ TIME ----------------------
void DS1307_getTime(DS1307_Time *t) {
    I2C_start();
    I2C_write(0x68 << 1); // write
    I2C_write(0x00);      // start at seconds
    I2C_stop();

    I2C_start();
    I2C_write((0x68 << 1) | 1); // read

    uint8_t sec  = I2C_read(1);
    uint8_t min  = I2C_read(1);
    uint8_t hour = I2C_read(1);
    uint8_t day  = I2C_read(1);
    uint8_t date = I2C_read(1);
    uint8_t mon  = I2C_read(1);
    uint8_t year = I2C_read(0); // last is NACK

    I2C_stop();

    t->second = bcd_to_dec(sec & 0x7F);
    t->minute = bcd_to_dec(min);
    t->hour   = bcd_to_dec(hour & 0x3F); // 24h mode only
    t->day    = bcd_to_dec(day);
    t->date   = bcd_to_dec(date);
    t->month  = bcd_to_dec(mon);
    t->year   = bcd_to_dec(year);
}

// ---------------------- RAM WRITE ----------------------
void DS1307_writeRAM(uint8_t addr, uint8_t data) {
    if (addr > 55) return; // DS1307 has 56 RAM bytes

    I2C_start();
    I2C_write(0x68 << 1);
    I2C_write(0x08 + addr);  // RAM begins at 0x08
    I2C_write(data);
    I2C_stop();
}

// ---------------------- RAM READ ----------------------
uint8_t DS1307_readRAM(uint8_t addr) {
    if (addr > 55) return 0xFF;

    uint8_t data;

    I2C_start();
    I2C_write(0x68 << 1);
    I2C_write(0x08 + addr);
    I2C_stop();

    I2C_start();
    I2C_write((0x68 << 1) | 1);
    data = I2C_read(0);
    I2C_stop();

    return data;
}

