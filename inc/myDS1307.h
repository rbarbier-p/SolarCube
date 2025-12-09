#ifndef MYDS1307_H
#define MYDS1307_H

#include <avr/io.h>
#include "mySoftI2C.h"

typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;       // 24h
    uint8_t day;        // 1–7
    uint8_t date;       // 1–31
    uint8_t month;      // 1–12
    uint8_t year;       // 0–99
} DS1307_Time;

// --- Functions ---
void DS1307_init(void);
void DS1307_setTime(DS1307_Time *t);
void DS1307_getTime(DS1307_Time *t);

// Optional
void DS1307_writeRAM(uint8_t addr, uint8_t data);
uint8_t DS1307_readRAM(uint8_t addr);

#endif

