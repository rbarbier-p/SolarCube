#include "animations.h"
#include <stdlib.h>
#include <time.h>

void anim_delay(uint16_t ms) {
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = (uint32_t)ms * 15625UL / 1000UL;

  TCCR1B |= (1 << WGM12);               // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // prescaler 1024

  while (!(TIFR1 & (1 << OCF1A))) {// wait for compare
    update_leds();
  }      

  TIFR1 |= (1 << OCF1A);                // clear flag
  TCCR1B = 0;
}

void drop_add(uint8_t x, uint8_t y, uint8_t z) {
  uint8_t drop_state = 5;
  while (drop_state != z) {
    select_led(x, y, drop_state);
    anim_delay(10);
    clear_led(x, y, drop_state);
    drop_state--;
 
  }
  select_led(x, y, z);
}

void drop_adder() {
  uint8_t anim_index = current_animation;
  while (1) {
    for (uint8_t z = 0; z < 6; z++) {
      for (uint8_t y = 0; y < 6; y++) {
        for (uint8_t x = 0; x < 6; x++) {
          drop_add(x, y, z);
          anim_delay(80);
          if (anim_index != current_animation) return;
        }
      }
    }
    clear_cube();
  }
}

void vertical_sine_wave(float freq) {
  uint8_t anim_index = current_animation;
  float t = 0;

  while (anim_index == current_animation) {
    clear_cube();

    for (int x = 0; x < SIZE; x++) {
      float fx = (float)x / (SIZE - 1);
      float value = sin(2 * PI * freq * fx + t);

      // smoother mapping: multiply by (SIZE-1)/2 instead of 2.5
      int z = (int)((value + 1.0) * ((SIZE - 1) / 2.0) + 0.5); 

      for (int y = 0; y < SIZE; y++) {
        select_led(x, y, z);
      }
    }

    t += 0.2;        // smaller increment = more frames per cycle
    update_leds();
    update_leds();
    update_leds();
  }
}

void vertical_sine_wave_rotated(float freq) {
  uint8_t anim_index = current_animation;
  float t = 0;
  float theta = 0;  // rotation angle

  while (anim_index == current_animation) {
    clear_cube();

    float cos_theta = cos(theta);
    float sin_theta = sin(theta);

    for (int x = 0; x < SIZE; x++) {
      for (int y = 0; y < SIZE; y++) {
        // rotate coordinates around the center
        float cx = x - (SIZE - 1) / 2.0f;
        float cy = y - (SIZE - 1) / 2.0f;

        float rx = cx * cos_theta - cy * sin_theta;
        // float ry = cx * sin_theta + cy * cos_theta; // not needed for vertical wave

        // map rotated x to sine wave
        float fx = (rx + (SIZE - 1) / 2.0f) / (SIZE - 1);
        float value = sin(2 * PI * freq * fx + t);

        int z = (int)((value + 1.0f) * ((SIZE - 1) / 2.0f) + 0.5);

        select_led(x, y, z);
      }
    }

    t += 0.1;           // wave motion
    theta += 0.01;      // rotation speed, adjust for faster/slower
    if (theta > 2 * PI) theta -= 2 * PI;

    update_leds();
  }
}

void rain_animation(float density) {
  uint8_t anim_index = current_animation;
  // Call once at the start
  srand(time(NULL));  // seed random numbers
  // density: probability of a new drop per column per frame (0.0 to 1.0)
  int cube[SIZE][SIZE][SIZE] = {0};

  while (anim_index == current_animation) {
    clear_cube();

    // Move drops down
    for (int z = 0; z < SIZE - 1; z++) {
      for (int x = 0; x < SIZE; x++) {
        for (int y = 0; y < SIZE; y++) {
          cube[z][x][y] = cube[z + 1][x][y];
        }
      }
    }

    // Clear top layer
    for (int x = 0; x < SIZE; x++)
      for (int y = 0; y < SIZE; y++)
        cube[SIZE - 1][x][y] = 0;

    // Spawn new drops at top layer randomly
    for (int x = 0; x < SIZE; x++) {
      for (int y = 0; y < SIZE; y++) {
        if ((float)rand() / RAND_MAX < density)
          cube[SIZE - 1][x][y] = 1; // drop present
      }
    }

    // Draw the cube
    for (int z = 0; z < SIZE; z++)
      for (int x = 0; x < SIZE; x++)
        for (int y = 0; y < SIZE; y++)
          if (cube[z][x][y])
            select_led(x, y, z);

    anim_delay(60);
    // update_leds();
    // update_leds();
    // update_leds();
    // update_leds();
    // update_leds();
    // update_leds();
    // update_leds();
    // update_leds();
    // update_leds();
  }
}
