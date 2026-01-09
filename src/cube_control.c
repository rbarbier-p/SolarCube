#include "cube_control.h"

uint64_t frame[6] = {0};

void select_led(uint8_t x, uint8_t y, uint8_t z) {
  frame[z] |= 1ULL << shiftRegisterPinMap[coordinates_to_led_num(x, y)];
}

void clear_cube() {
  for (int i = 0; i < 6; i++) {
    frame[i] = 0;
  }
}

uint8_t coordinates_to_led_num(uint8_t x, uint8_t y) {
  return (x + (y * 6));
}

void update_leds() {
  selectLayer(1);
  selectRows(frame[0]);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(2);
  selectRows(frame[1]);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(3);
  selectRows(frame[2]);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(4);
  selectRows(frame[3]);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(5);
  selectRows(frame[4]);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(6);
  selectRows(frame[5]);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);
}

void pinInit() {
  LAY_DDR |= (1 << LAY_CLOCK_PIN) | (1 << LAY_LATCH_PIN) | (1 << LAY_DATA_PIN);
  ROW_DDR |= (1 << ROW_CLOCK_PIN) | (1 << ROW_LATCH_PIN) | (1 << ROW_DATA_PIN);

  LAY_PORT &= ~(1 << LAY_LATCH_PIN);
  shiftOut(&LAY_PORT, MSBFIRST, LAY_DATA_PIN, LAY_CLOCK_PIN, 0);
  LAY_PORT |= (1 << LAY_LATCH_PIN);
  ROW_PORT &= ~(1 << ROW_LATCH_PIN);
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, 0);
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, 0);
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, 0);
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, 0);
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, 0);
  ROW_PORT |= (1 << ROW_LATCH_PIN);
}

void shiftOut(volatile uint8_t *port, uint8_t bitOrder, uint8_t dataPin, uint8_t clockPin, uint8_t val) {
  for (uint8_t i = 0; i < 8; i++) {

    uint8_t bit;

    if (bitOrder == MSBFIRST)
      bit = ((val << i) & 0x80);
    else 
      bit = ((val >> i) & 0x01);
      
    if (bit)
      *port |=  (1 << dataPin);
    else
      *port &= ~(1 << dataPin);

    *port |=  (1 << clockPin);
    *port &= ~(1 << clockPin);
  }
}


void selectLayer(uint8_t layer) {
  uint8_t bit = 1 << layer + 1; // skips unconnected layer, lets 0 be a non connected layer.
  LAY_PORT &= ~(1 << LAY_LATCH_PIN);
  shiftOut(&LAY_PORT, MSBFIRST, LAY_DATA_PIN, LAY_CLOCK_PIN, bit);
  LAY_PORT |= (1 << LAY_LATCH_PIN);
}

void selectRows(uint64_t rows) {
  ROW_PORT &= ~(1 << ROW_LATCH_PIN);
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, (uint8_t) (rows >> 32));
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, (uint8_t) (rows >> 24));
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, (uint8_t) (rows >> 16));
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, (uint8_t) (rows >> 8));
  shiftOut(&ROW_PORT, MSBFIRST, ROW_DATA_PIN, ROW_CLOCK_PIN, (uint8_t) rows);
  ROW_PORT |= (1 << ROW_LATCH_PIN);
}
