#ifndef MW_H
#define MW_H

#include <xc.h>
#include "clcd.h"
#include "matrix_keypad.h"
#include "timers.h"

#define ON_SCREEN          0x01
#define MENU_SCREEN        0x02
#define MICRO_MODE         0x11
#define GRILL_MODE         0x12
#define CONVECTION_MODE    0x13
#define START_MODE         0x14

#define RESET_NULL         0x00
#define RESET_MENU         0xAA
#define RESET_MODE         0xFF
#define PREHEAT_MODE       0xCC
#define SET_TIME           0xDD

#define FAN_DDR            TRISC2
#define FAN                RC2

#define BUZZER_DDR         TRISC1
#define BUZZER             RC1

#define SUCCESS            0x22

extern volatile unsigned int remaining;
extern volatile unsigned char running;

extern volatile unsigned int temperature;
extern volatile unsigned char convection;

unsigned char power_on(void);
unsigned char display_menu(unsigned char key);
unsigned char set_time(unsigned char key);
unsigned char micro_mode(unsigned char key);
unsigned char grill_mode(unsigned char key);
unsigned char convection_mode(unsigned char key);
unsigned char preheat_mode(unsigned char key);
unsigned char start_mode(unsigned char key);

#endif