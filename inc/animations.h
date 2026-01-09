#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "cube_control.h"

#define SIZE 6
#define PI 3.14159265
#define ANIM_DELAY 50 

void vertical_sine_wave(float freq);
void wave_animation();
void diagonal_wave();
void ft_delay(uint16_t ms);

#endif

