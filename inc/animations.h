#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"

#define SIZE 6
#define PI 3.14159265
#define ANIM_DELAY 50 

extern volatile uint8_t current_animation;

void vertical_sine_wave(float freq);
void wave_animation();
void diagonal_wave();
void anim_delay(uint16_t ms);
void update(uint8_t n);
void drop_add(uint8_t x, uint8_t y, uint8_t z);
void drop_adder();
void rotating_sine_wave(float freq);
void vertical_sine_wave_rotated(float freq);
void rain_animation(float density);

#endif

