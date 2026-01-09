#include "animations.h"

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

void vertical_sine_wave(float freq) {
  float t = 0;

  while (1) {
    cli();
    clear_cube();

    for (int x = 0; x < SIZE; x++) {
      float fx = (float)x / (SIZE - 1);

      // freq = number of waves across the cube
      float value = sin(2 * PI * freq * fx + t);

      int z = (int)((value + 1.0) * 2.5);

      for (int y = 0; y < SIZE; y++) {
        select_led(x, y, z);
      }
    }

    t += 1;   // temporal speed
    sei();
    ft_delay(ANIM_DELAY);
    if (!(PINC & (1 << PC3)))
      return ;
  }
}

void wave_animation() {
  float t = 0;

  while (1) {
    cli();
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

    t += 0.8;   // wave speed
    sei();
    ft_delay(ANIM_DELAY);
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
