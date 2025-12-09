#ifndef MYSOFTI2C_H
#define MYSOFTI2C_H

#include <avr/io.h>
#include <util/delay.h>

#define WRITE 0
#define READ  1

void I2C_init(volatile uint8_t *sda_port,
              volatile uint8_t *sda_ddr,
              volatile uint8_t *sda_pinreg,
              uint8_t sda_bit,
              volatile uint8_t *scl_port,
              volatile uint8_t *scl_ddr,
              volatile uint8_t *scl_pinreg,
              uint8_t scl_bit);

void I2C_start(void);
void I2C_stop(void);
uint8_t I2C_write(uint8_t data);
uint8_t I2C_read(uint8_t ack);

#endif

