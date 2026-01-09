#include "animations.h"

void drop_add(uint8_t x, uint8_t y, uint8_t z) {
  uint8_t drop_state = 5;
  while (drop_state != z) {
    select_led(x, y, drop_state);
    update_leds();
    update_leds();
    clear_led(x, y, drop_state);
    drop_state--;
 
  }
  select_led(x, y, z);
}

void drop_adder() {
  while (1) {
    for (uint8_t z = 0; z < 6; z++) {
      for (uint8_t y = 0; y < 6; y++) {
        for (uint8_t x = 0; x < 6; x++) {
          drop_add(x, y, z);
          if (!(PINC & (1 << PC3)))

          return (ft_delay(10), (void) 0);
        }
      }
    }
    clear_cube();
  }
}

void ft_delay(uint16_t ms) {
  if (ms == 0) return;

  cli();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = (uint32_t)ms * 15625UL / 1000UL;

  TCCR1B |= (1 << WGM12);               // CTC mode
  TCCR1B |= (1 << CS12) | (1 << CS10);  // prescaler 1024
  sei();

  while (!(TIFR1 & (1 << OCF1A)));      // wait for compare

  TIFR1 |= (1 << OCF1A);                // clear flag
  TCCR1B = 0;
}
//
// void vertical_sine_wave(float freq) {
//   float t = 0;
//
//   while (1) {
//     cli();
//     clear_cube();
//
//     for (int x = 0; x < SIZE; x++) {
//       float fx = (float)x / (SIZE - 1);
//       float value = sin(2 * PI * freq * fx + t);
//
//       // smoother mapping: multiply by (SIZE-1)/2 instead of 2.5
//       int z = (int)((value + 1.0) * ((SIZE - 1) / 2.0) + 0.5); 
//
//       for (int y = 0; y < SIZE; y++) {
//         select_led(x, y, z);
//       }
//     }
//
//     t += 0.1;        // smaller increment = more frames per cycle
//     ft_delay(ANIM_DELAY - 49);
//
//     if (!(PINC & (1 << PC3)))
//       return;
//   }
// }

#define SINE_RES 64  // number of points in sine table
const float sine_table[SINE_RES] = {
    0.0, 0.098, 0.195, 0.292, 0.383, 0.471, 0.556, 0.634,
    0.707, 0.773, 0.831, 0.881, 0.924, 0.956, 0.980, 0.995,
    1.0, 0.995, 0.980, 0.956, 0.924, 0.881, 0.831, 0.773,
    0.707, 0.634, 0.556, 0.471, 0.383, 0.292, 0.195, 0.098,
    0.0, -0.098, -0.195, -0.292, -0.383, -0.471, -0.556, -0.634,
    -0.707, -0.773, -0.831, -0.881, -0.924, -0.956, -0.980, -0.995,
    -1.0, -0.995, -0.980, -0.956, -0.924, -0.881, -0.831, -0.773,
    -0.707, -0.634, -0.556, -0.471, -0.383, -0.292, -0.195, -0.098
};

void vertical_sine_wave(float freq) {
    float t = 0;

    while (1) {
        clear_cube();

        for (int x = 0; x < SIZE; x++) {
            // map x to sine table index
            int index = ((int)((freq * x / (SIZE - 1) * SINE_RES + t)) % SINE_RES + SINE_RES) % SINE_RES;
            float value = sine_table[index];

            int z = (int)((value + 1.0) * ((SIZE - 1) / 2.0) + 0.5);

            for (int y = 0; y < SIZE; y++) {
                select_led(x, y, z);
            }
        }

        t += 1;   // speed in table indices
        update_leds();
        update_leds();

        if (!(PINC & (1 << PC3)))
            return;
    }
}

void wave_animation() {
  float t = 0;

  while (1) {
    clear_cube();

    for (int x = 0; x < SIZE; x++) {
      for (int y = 0; y < SIZE; y++) {

        // Normalize x and y to 0..2π
        float fx = (float)x / (SIZE - 1) * 2 * PI;
        float fy = (float)y / (SIZE - 1) * 2 * PI;

        // Sine wave
        float value = sin(fx + fy + t);

        // Map from -1..1 to 0..5
        int z = (int)((value + 1.0) * 2.5);

        select_led(x, y, z);
      }
    }

    t += 0.7;   // wave speed
    update_leds();
    update_leds();
    if (!(PINC & (1 << PC3)))
      return ;
  }
}

void diagonal_wave() {
  int step = 0;

  while (1) {
    cli();
    clear_cube();

    for (int x = 0; x < 6; x++) {
      for (int y = 0; y < 6; y++) {
        int z = (x + y + step) % 6;
        select_led(x, y, z);
      }
    }

    step++;
    sei();
    ft_delay(ANIM_DELAY);
    if (!(PINC & (1 << PC3)))
      return ;
  }
}
