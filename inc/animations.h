#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"
#include "myDS1307.h"

#define SIZE 6
#define PI 3.14159265
#define ANIM_DELAY 50 

extern volatile uint8_t current_animation;

void vertical_sine_wave(float freq);
void anim_delay(uint16_t ms);
void update(uint16_t n);
void drop_adder();
void vertical_sine_wave_rotated(float freq);
void rain_animation(float density);
void solar_clock(DS1307_Time t);
void expanding_cube();
void moving_cube_random();
void moving_42();


#endif

