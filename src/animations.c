#include "animations.h"
#include <stdlib.h>
#include "times.h"
#include "myDS1307.h"
#include "UART.h"
#include <time.h>

void update(uint8_t n) {
  for (uint8_t i = 0; i < n; i++) {
    update_leds();
  }
}

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

void drop_add(uint8_t x, uint8_t y, uint8_t z, uint8_t speed) {
  int8_t drop_state = 5;

  while (drop_state > z) {
    select_led(x, y, drop_state);
    anim_delay(speed);
    clear_led(x, y, drop_state);
    drop_state--;
  }

  select_led(x, y, z);
}

void drop_remove(uint8_t x, uint8_t y, uint8_t z, uint8_t speed) {
    uint8_t drop_state = z;

    while (drop_state < 6) {   // 6 is above top layer
        select_led(x, y, drop_state);  // light current layer
        anim_delay(speed);                // small delay for animation
        clear_led(x, y, drop_state);   // clear it before moving up
        drop_state++;
    }
}


void drop_adder() {
  uint8_t anim_index = current_animation;
  while (1) {
    for (uint8_t z = 0; z < 6; z++) {
      for (uint8_t y = 0; y < 6; y++) {
        for (uint8_t x = 0; x < 6; x++) {
          drop_add(x, y, z, 10);
          anim_delay(80);
          if (anim_index != current_animation) return;
        }
      }
    }
    clear_cube();
  }
}

uint16_t get_sunrise_minutes(DS1307_Time t) {
  uint16_t sunrise = pgm_read_word(&(sunrises[(t.month - 1) * 30 + t.date - 1]));
  return (sunrise / 100) * 60 + (sunrise % 100);
}

uint16_t get_sunset_minutes(DS1307_Time t) {
  uint16_t sunset = pgm_read_word(&(sunsets[(t.month - 1) * 30 + t.date - 1]));
  return (sunset / 100) * 60 + (sunset % 100);
}

void initial_drop_wave(uint8_t column_height[6][6]) {
    const uint8_t top_offset = 9; // start above cube
    static int8_t current_z[6][6];       // current position of each column
    static int16_t start_delay[6][6];    // delay before each column starts falling
    const int16_t delay_step = 30; // delay increment between columns (ms)
    
    // initialize
    for(uint8_t x=0; x<6; x++){
        for(uint8_t y=0; y<6; y++){
            current_z[x][y] = top_offset;
            start_delay[x][y] = (x + y) * delay_step; // staggered wave
        }
    }

    uint8_t finished = 0;

    // animate until all columns reach their target
    while(finished < 36){
        finished = 0;
        clear_cube();

        for(uint8_t x=0; x<6; x++){
            for(uint8_t y=0; y<6; y++){
                uint8_t col_h = column_height[x][y];
                if(col_h == 0){
                    finished++;
                    continue;
                }

                if(start_delay[x][y] > 0){
                    // wait for the staggered start
                    start_delay[x][y] -= 20; // matches anim_delay below
                } else if(current_z[x][y] > col_h){
                    // move the whole column down by 1
                    for(uint8_t dz=0; dz<col_h; dz++){
                        select_led(x, y, current_z[x][y] - 1 + dz - col_h);
                    }
                    current_z[x][y]--;
                } else {
                    // reached target height
                    for(uint8_t dz=0; dz<col_h; dz++){
                        select_led(x, y, dz);
                    }
                    finished++;
                }
            }
        }

        update(5); //speed
    }
}

void solar_clock(DS1307_Time t) {
  uint8_t anim_index = current_animation;

  // Get the current time and sunrise/sunset times once
  DS1307_getTime(&t);
  uint16_t sunrise_minutes = get_sunrise_minutes(t);
  uint16_t sunset_minutes = get_sunset_minutes(t);
  uint16_t current_minutes = t.hour * 60 + t.minute;

  // Calculate the total minutes of daylight and the number of lit LEDs
  uint8_t led_count = 216;
  uint8_t lit_leds = 0;

  if (current_minutes >= sunrise_minutes && current_minutes <= sunset_minutes) {
    uint16_t midday_minutes = (sunrise_minutes + sunset_minutes) / 2;
    if (current_minutes <= midday_minutes) {
      lit_leds = (uint8_t)(((current_minutes - sunrise_minutes) / (float)(midday_minutes - sunrise_minutes)) * led_count);
    } else {
      lit_leds = (uint8_t)(((sunset_minutes - current_minutes) / (float)(sunset_minutes - midday_minutes)) * led_count);
    }
  }

  // Send UART message with the current time and sunrise/sunset hours
  UART_print_str("Percentage: ");
  UART_print_num((lit_leds * 100) / led_count); // Percentage of lit LEDs
  UART_print_str("\r\nCurrent Time: ");
  UART_print_num(t.hour); // Send current hour
  UART_print_str(":");
  UART_print_num(t.minute); // Send current minute
  UART_print_str("\r\nSunrise: ");
  UART_print_num(sunrise_minutes / 60); // Send sunrise hour
  UART_print_str(":");
  UART_print_num(sunrise_minutes % 60); // Send sunrise minute
  UART_print_str("\r\nSunset: ");
  UART_print_num(sunset_minutes / 60); // Send sunset hour
  UART_print_str(":");
  UART_print_num(sunset_minutes % 60); // Send sunset minute


  uint8_t prev_lit_leds = lit_leds;
  clear_cube();

  uint8_t column_height[6][6] = {0};
  for(uint8_t i=0; i<lit_leds; i++){
    uint8_t x = i % 6;
    uint8_t y = (i / 6) % 6;
    column_height[x][y] = (i / 36) + 1;
  }

  initial_drop_wave(column_height);
  while (anim_index == current_animation) {

    // Recalculate the number of lit LEDs in the loop
    DS1307_getTime(&t);
    current_minutes = t.hour * 60 + t.minute;

    if (current_minutes >= sunrise_minutes && current_minutes <= sunset_minutes) {
      uint16_t midday_minutes = (sunrise_minutes + sunset_minutes) / 2;
      if (current_minutes <= midday_minutes) {
        lit_leds = (uint8_t)(((current_minutes - sunrise_minutes) / (float)(midday_minutes - sunrise_minutes)) * led_count);
      } else {
        lit_leds = (uint8_t)(((sunset_minutes - current_minutes) / (float)(sunset_minutes - midday_minutes)) * led_count);
      }
    } else {
      lit_leds = 0; // No LEDs lit outside of sunrise/sunset
    }

  if (lit_leds > prev_lit_leds) {
    // ADD LEDs (sun rising)
    for (uint8_t i = prev_lit_leds; i < lit_leds; i++) {
      uint8_t x = i % 6;
      uint8_t y = (i / 6) % 6;
      uint8_t z = i / 36;
      drop_add(x, y, z, 100);
    }
  }
  else if (lit_leds < prev_lit_leds) {
    // REMOVE LEDs (sun setting / decay)
    for (int16_t i = prev_lit_leds - 1; i >= lit_leds; i--) {
      uint8_t x = i % 6;
      uint8_t y = (i / 6) % 6;
      uint8_t z = i / 36;
      drop_remove(x, y, z, 100);
    }
  }

  prev_lit_leds = lit_leds;
  update(100);
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
    update(3);
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

    // anim_delay(60);
    update(10);
  }
}
