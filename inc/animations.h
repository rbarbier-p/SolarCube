#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <avr/io.h>
#include <util/delay.h>

#define ANIM_DELAY 100
#define TAIL_LEN 5

void laps_animation();
void crossing_laps_animation();
void animation_even_odd();

#endif

