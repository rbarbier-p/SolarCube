#include "mySoftI2C.h"

// Global pin references
static volatile uint8_t *SDA_PORT;
static volatile uint8_t *SDA_DDR;
static volatile uint8_t *SDA_PIN;
static uint8_t SDA_BIT;

static volatile uint8_t *SCL_PORT;
static volatile uint8_t *SCL_DDR;
static volatile uint8_t *SCL_PIN;
static uint8_t SCL_BIT;

static inline void i2c_delay(void) {
    _delay_us(5); // 100 kHz approx
}

static inline void SDA_high(void) {
    *SDA_DDR &= ~(1 << SDA_BIT);   // input (line goes high via pull-up)
}

static inline void SDA_low(void) {
    *SDA_PORT &= ~(1 << SDA_BIT);
    *SDA_DDR |= (1 << SDA_BIT);    // output low
}

static inline void SCL_high(void) {
    *SCL_DDR &= ~(1 << SCL_BIT);
}

static inline void SCL_low(void) {
    *SCL_PORT &= ~(1 << SCL_BIT);
    *SCL_DDR |= (1 << SCL_BIT);
}

void I2C_init(volatile uint8_t *sda_port,
              volatile uint8_t *sda_ddr,
              volatile uint8_t *sda_pinreg,
              uint8_t sda_bit,
              volatile uint8_t *scl_port,
              volatile uint8_t *scl_ddr,
              volatile uint8_t *scl_pinreg,
              uint8_t scl_bit)
{
    SDA_PORT = sda_port;
    SDA_DDR = sda_ddr;
    SDA_PIN = sda_pinreg;
    SDA_BIT = sda_bit;

    SCL_PORT = scl_port;
    SCL_DDR = scl_ddr;
    SCL_PIN = scl_pinreg;
    SCL_BIT = scl_bit;

    SDA_high();
    SCL_high();
    _delay_us(10);
}

void I2C_start(void) {
    SDA_high();
    SCL_high();
    i2c_delay();
    SDA_low();
    i2c_delay();
    SCL_low();
}

void I2C_stop(void) {
    SDA_low();
    SCL_high();
    i2c_delay();
    SDA_high();
}

uint8_t I2C_write(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        if (data & 0x80) SDA_high();
        else SDA_low();

        i2c_delay();
        SCL_high();
        i2c_delay();
        SCL_low();

        data <<= 1;
    }

    // ACK
    SDA_high(); // release
    i2c_delay();
    SCL_high();
    uint8_t ack = !(*SDA_PIN & (1 << SDA_BIT)); // ACK = pulled low
    SCL_low();
    return ack;
}

uint8_t I2C_read(uint8_t ack) {
    uint8_t data = 0;

    SDA_high(); // release line

    for (uint8_t i = 0; i < 8; i++) {
        data <<= 1;

        SCL_high();
        i2c_delay();
        if (*SDA_PIN & (1 << SDA_BIT)) data |= 1;
        SCL_low();
        i2c_delay();
    }

    // ACK/NACK
    if (ack)
        SDA_low();
    else
        SDA_high();

    i2c_delay();
    SCL_high();
    i2c_delay();
    SCL_low();
    SDA_high();

    return data;
}

