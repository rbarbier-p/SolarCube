#include "animations.h"

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

  _delay_ms(500);
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
