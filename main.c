#include <xc.h>
#include "microwave.h"

#define _XTAL_FREQ 20000000

#pragma config WDTE = OFF

unsigned char screen_flag = ON_SCREEN;
unsigned char reset_flag = RESET_NULL;

void init_config(void)
{
    init_clcd();
    init_matrix_keypad();
    init_timer2();

    PEIE = 1;
    GIE = 1;
    
    FAN_DDR = 0x00;
    BUZZER_DDR = 0x00;
}

void main(void)
{
    unsigned char key;

    init_config();

    while (1)
    {
        key = read_matrix_keypad(STATE);

        switch (screen_flag)
        {
            case ON_SCREEN:

                if (power_on() == SUCCESS)
                {
                    screen_flag = MENU_SCREEN;
                    reset_flag =  RESET_MENU;
                }

                break;

            case MENU_SCREEN:
                
                display_menu(key);
                
                break;
                
            case MICRO_MODE:
                
                if (micro_mode(key) == SUCCESS)
                {
                    screen_flag = START_MODE;
                    reset_flag = RESET_MODE;
                    clear_screen();
                }

                break;
                
            case GRILL_MODE:
                
                if (grill_mode(key) == SUCCESS)
                {
                    screen_flag = START_MODE;
                    reset_flag = RESET_MODE;
                    clear_screen();
                }

                break;
                
            case CONVECTION_MODE:
                
                if (convection_mode(key) == SUCCESS)
                {
                    screen_flag = START_MODE;
                    reset_flag = RESET_MODE;
                    clear_screen();
                }

                break;

           case START_MODE:
               
                TMR2ON = 1;

                if (start_mode(key) == SUCCESS)
                {
                    screen_flag = MENU_SCREEN;
                    reset_flag = RESET_MENU;
                    clear_screen();
                    
                    TMR2ON = 0;
                }

                break;
        }
    }
}