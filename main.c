#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LAY_CLOCK_PIN PD5 // pin 5
#define LAY_LATCH_PIN PD4 // pin 4
#define LAY_DATA_PIN  PD2 // pin 2
#define LAY_PORT      PORTD 
#define LAY_DDR       DDRD

#define ROW_CLOCK_PIN PB4 // pin 12
#define ROW_LATCH_PIN PB3 // pin 11
#define ROW_DATA_PIN  PB1 // pin 9
#define ROW_PORT      PORTB 
#define ROW_DDR       DDRB 

#define MULTIPLEX_DELAY 1
#define ANIM_DELAY 100

#define TAIL_LEN 5

// Lap path in order
static const uint8_t path[] = {
    // byte0: bits 0 → 7
    0,1,2,3,4,5,6,7,

    // byte2 (line3): bits 0 → 7
    16,17,18,19,20,21,22,23,

    // byte4 (line5): bits 0 → 7
    32,33,34,35,36,37,38,39,

    // byte5 (line6): bits 7 → 0
    47,46,45,44,43,42,41,40,

    // byte3 (line4): bits 7 → 0
    31,30,29,28,27,26,25,24,

    // byte1 (line2): bits 7 → 0
    15,14,13,12,11,10, 9, 8,
};

#define PATH_LEN (sizeof(path) / sizeof(path[0]))

uint64_t data = 0;

void init_timer0(void) 
{
	TCCR0A |= (1 << WGM01); // CTC mode
	TCCR0B |= (1 << CS02) | (1 << CS00); // 1024 prescaler
	TIMSK0 |= (1 << OCIE0A); // ouput compare match A interrupt enable

	OCR0A = 255; // 0.016384s per timer0 cycle. / 1(s) = ~61
}


void pinInit() {
  LAY_DDR |= (1 << LAY_CLOCK_PIN) | (1 << LAY_LATCH_PIN) | (1 << LAY_DATA_PIN);
  ROW_DDR |= (1 << ROW_CLOCK_PIN) | (1 << ROW_LATCH_PIN) | (1 << ROW_DATA_PIN);
}

void shiftOut(volatile uint8_t *port, uint8_t dataPin, uint8_t clockPin, uint8_t val) {
  for (uint8_t i = 0; i < 8; i++) {

    uint8_t bit = ((val >> i) & 1);

    if (bit)
      *port |=  (1 << dataPin);
    else
      *port &= ~(1 << dataPin);

    *port |=  (1 << clockPin);
    *port &= ~(1 << clockPin);
  }
}


void selectLayer(uint8_t layer) {
  if (layer > 6 || layer == 0)
    return ;
  LAY_PORT &= ~(1 << LAY_LATCH_PIN);
  shiftOut(&LAY_PORT, LAY_DATA_PIN, LAY_CLOCK_PIN, 1 << (7 - layer));
  LAY_PORT |= (1 << LAY_LATCH_PIN);
}

void selectRows(uint8_t rows) {
  ROW_PORT &= ~(1 << ROW_LATCH_PIN);
  shiftOut(&ROW_PORT, ROW_DATA_PIN, ROW_CLOCK_PIN, rows);
  ROW_PORT |= (1 << ROW_LATCH_PIN);
}

void update_leds() {
  selectLayer(1);
  selectRows((uint8_t) data);
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(2);
  selectRows((uint8_t) (data >> 8));
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);
  
  selectLayer(3);
  selectRows((uint8_t) (data >> 16));
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);
  
  selectLayer(4);
  selectRows((uint8_t) (data >> 24));
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(5);
  selectRows((uint8_t) (data >> 32));
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);

  selectLayer(6);
  selectRows((uint8_t) (data >> 40));
  _delay_ms(MULTIPLEX_DELAY);
  selectRows(0);
}

ISR(TIMER0_COMPA_vect)
{
  update_leds();
}

void laps_animation() {

  static uint16_t pos = 0;
  // turn ON the new head LED
  data |= ((uint64_t)1 << path[pos]);

  // turn OFF the LED TAIL steps behind
  uint16_t tailPos = (pos + PATH_LEN - TAIL_LEN) % PATH_LEN;
  data &= ~((uint64_t)1 << path[tailPos]);

  // move forward
  pos = (pos + 1) % PATH_LEN;

  _delay_ms(10);
}

void crossing_laps_animation() {
  
  static int posA = 0;
  static int posB = PATH_LEN / 2;
  // clear everything each frame
  data = 0;

  // draw comet A (forwards / clockwise)
  for (int i = 0; i < TAIL_LEN; i++) {
    int idx = (posA - i + PATH_LEN) % PATH_LEN;
    data |= (uint64_t)1 << path[idx];
  }

  // draw comet B (backwards / counter-clockwise)
  for (int i = 0; i < TAIL_LEN; i++) {
    int idx = (posB + i) % PATH_LEN;
    data |= (uint64_t)1 << path[idx];
  }

  // update movement
  posA = (posA + 1) % PATH_LEN;        // move forward
  posB = (posB - 1 + PATH_LEN) % PATH_LEN; // move backward

  _delay_ms(15);
}

void animation_even_odd()
{
  // 1) Turn on EVEN positions following track order
  for (uint8_t i = 0; i < 48; i++) {
    if ((i % 2) == 0) {
      data |= ((uint64_t)1 << path[i]);
      _delay_ms(ANIM_DELAY);
    }
  }

  // 2) Turn on ODD positions following track order
  for (uint8_t i = 0; i < 48; i++) {
    if ((i % 2) == 1) {
      data |= ((uint64_t)1 << path[i]);
      _delay_ms(ANIM_DELAY);
    }
  }

  // 3) Turn off EVEN positions following track order
  for (uint8_t i = 0; i < 48; i++) {
    if ((i % 2) == 0) {
      data &= ~((uint64_t)1 << path[i]);
      _delay_ms(ANIM_DELAY);
    }
  }

  // 4) Turn off ODD positions following track order
  for (uint8_t i = 0; i < 48; i++) {
    if ((i % 2) == 1) {
      data &= ~((uint64_t)1 << path[i]);
      _delay_ms(ANIM_DELAY);
    }
  }
}

void main () {
  sei();
  pinInit();
  init_timer0();

  while (1) {
    animation_even_odd();
  }
}
