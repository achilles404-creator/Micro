#include <xc.h>
#include "microwave.h"

extern unsigned char screen_flag;
extern unsigned char reset_flag;

static unsigned char count = 0;
static unsigned char blink = 0;

static unsigned char min;
static unsigned char sec;

volatile unsigned int temperature = 0;
volatile unsigned char convection = 0;

volatile unsigned int remaining = 0;
volatile unsigned char running = 0;

unsigned char power_on(void)
{
    static unsigned char i = 0, j = 0;

    if (i == 0)
    {
        clear_screen();

        clcd_print("MICROWAVE OVEN", LINE2(1));
        clcd_print("Powering ON", LINE3(2));
    }

    clcd_putch(0xFF, LINE1(i));
    clcd_putch(0xFF, LINE4(i));

    i++;

    if (i >= 16)
    {
        i = 0;
        return SUCCESS;
    }
  
    __delay_ms(100);
    
    return RESET_NULL;
}

unsigned char display_menu(unsigned char key)
{
    if(reset_flag == RESET_MENU)
    {
        clear_screen();
        
        clcd_print("1.Micro", LINE1(0));
        clcd_print("2.Grill", LINE2(0));
        clcd_print("3.Convection", LINE3(0));
        clcd_print("4.Start", LINE4(0));
        
        reset_flag = RESET_NULL;
    }

    if (key == 1)
    {
        screen_flag = MICRO_MODE;
        reset_flag = RESET_MODE;
    }
    else if (key == 2)
    {
        screen_flag = GRILL_MODE;
        reset_flag = RESET_MODE;
    }
    else if (key == 3)
    {
        screen_flag = CONVECTION_MODE;
        reset_flag = RESET_MODE;
    }
    else if (key == 4)
    {
        remaining = 30;
        screen_flag = START_MODE;
        reset_flag = RESET_MODE;
    }
    
    return RESET_NULL;       
}

unsigned char set_time(unsigned char key)
{
    static unsigned char field;

    if(reset_flag == SET_TIME)
    {
        min = 0;
        sec = 0;
        field = 0;

        count = 0;
        blink = 0;

        clear_screen();

        clcd_print("SET TIME <MM:SS>", LINE1(0));
        clcd_print("TIME:      00:00", LINE2(0));
        clcd_print("*:CLEAR  #:ENTER", LINE4(0));

        reset_flag = RESET_NULL;
    }

    count++;

    if(count >= 25)
    {
        count = 0;
        blink = !blink;
    }

    if(key <= 9)
    {
        if(field == 0)
        {
            sec = (sec * 10) + key;

            if(sec > 59)
            {
                sec = key;
            }
            else if(sec >= 10)
            {
                field = 1;
            }
        }
        else
        {
            min = (min * 10) + key;

            if(min > 99)
                min = key;
        }
    }
    else if(key == '*')
    {
        if(field == 1 && min == 0)
        {
            sec = 0;
            field = 0;
        }
        else if(field == 1)
        {
            min = 0;
        }
        else
        {
            sec = 0;
        }

        count = 0;
        blink = 0;
    }
    else if(key == '#')
    {
        if(field == 0)
        {
            field = 1;
            count = 0;
            blink = 0;
        }
        else
        {
            remaining = (min * 60) + sec;
            return SUCCESS;
        }
    }

    if(field == 0)
    {
        /* Blink seconds */
        if(blink)
        {
            clcd_putch(' ', LINE2(14));
            clcd_putch(' ', LINE2(15));
        }
        else
        {
            clcd_putch((sec / 10) + '0', LINE2(14));
            clcd_putch((sec % 10) + '0', LINE2(15));
        }
        
        clcd_putch((min / 10) + '0', LINE2(11));
        clcd_putch((min % 10) + '0', LINE2(12));
    }
    else
    {
        /* Blink minutes */
        if(blink)
        {
            clcd_putch(' ', LINE2(11));
            clcd_putch(' ', LINE2(12));
        }
        else
        {
            clcd_putch((min / 10) + '0', LINE2(11));
            clcd_putch((min % 10) + '0', LINE2(12));
        }

        clcd_putch((sec / 10) + '0', LINE2(14));
        clcd_putch((sec % 10) + '0', LINE2(15));
    }

    return RESET_NULL;
}
unsigned char micro_mode(unsigned char key)
{
    if(reset_flag == RESET_MODE)
    {
        clear_screen();
        
        clcd_print("  POWER = 900W  ", LINE2(0));
        __delay_ms(1000);
        
        reset_flag = SET_TIME;
    }

    if(set_time(key) == SUCCESS)
    {
        return SUCCESS;
    }
    
    return RESET_NULL;
}

unsigned char grill_mode(unsigned char key)
{
    if(reset_flag == RESET_MODE)
    {
        reset_flag = SET_TIME;
    }
    
    if(set_time(key) ==  SUCCESS)
    {
        return SUCCESS;
    }
    
    return RESET_NULL;
}

unsigned char convection_mode(unsigned char key)
{
    if (reset_flag == RESET_MODE)
    {
        convection = 0;
        temperature = 0;

        clear_screen();

        clcd_print("SET TEMP    < C>", LINE1(0));
        clcd_putch(0xDF, LINE1(13));
        clcd_print("TEMP = 000", LINE2(0));
        clcd_print("*:CLEAR  #:ENTER", LINE4(0));

        reset_flag = RESET_NULL;
    }

    /* PHASE 0: SET TEMPERATURE */
    if (convection == 0)
    {
        if (key <= 9)
        {
            temperature = (temperature * 10) + key;

            /* Maximum temperature = 180°C */
            if (temperature > 180)
            {
                temperature = 180;
            }

            clcd_putch((temperature / 100) + '0', LINE2(7));
            clcd_putch(((temperature / 10) % 10) + '0', LINE2(8));
            clcd_putch((temperature % 10) + '0', LINE2(9));
        }

        /* CLEAR */
        else if (key == '*')
        {
            temperature = 0;

            clcd_putch('0', LINE2(7));
            clcd_putch('0', LINE2(8));
            clcd_putch('0', LINE2(9));
        }

        /* ENTER TEMPERATURE */
        else if (key == '#')
        {
            if (temperature > 180)
            {
                temperature = 180;
            }

            remaining = 60;
            running = 1;
            FAN = 1;

            convection = 1;
            reset_flag = PREHEAT_MODE;
        }
    }

    /* PHASE 1: PREHEATING */
    else if (convection == 1)
    {
        if (preheat_mode(key) == SUCCESS)
        {
            convection = 2;
            reset_flag = SET_TIME;
        }
    }

    /* PHASE 2: SET TIME */
    else if (convection == 2)
    {
        if (set_time(key) == SUCCESS)
        {
            convection = 0;
            return SUCCESS;
        }
    }

    return RESET_NULL;
}

unsigned char preheat_mode(unsigned char key)
{
    if (reset_flag == PREHEAT_MODE)
    {
        clear_screen();
        
        TMR2ON = 1;

        clcd_print("Preheating", LINE1(3));
        clcd_print("Time left: ", LINE2(1));

        reset_flag = RESET_NULL;
    }
    
    clcd_putch((remaining / 10) + '0', LINE2(12));
    clcd_putch((remaining % 10) + '0', LINE2(13));
    clcd_putch('s', LINE2(14));

    if (remaining == 0)
    {
        TMR2ON = 0;
        running = 0;
        FAN = 0;
        BUZZER = 0;

        return SUCCESS;
    }

    return RESET_NULL;
}

unsigned char start_mode(unsigned char key)
{
    static unsigned char pause = 0;

    if (reset_flag == RESET_MODE)
    {
        clear_screen();

        clcd_print("TIME = ", LINE1(0));

        clcd_putch((remaining / 60) / 10 + '0', LINE1(7));
        clcd_putch((remaining / 60) % 10 + '0', LINE1(8));
        clcd_putch(':', LINE1(9));
        clcd_putch((remaining % 60) / 10 + '0', LINE1(10));
        clcd_putch((remaining % 60) % 10 + '0', LINE1(11));

        clcd_print("4.START/RESUME", LINE2(0));
        clcd_print("5.PAUSE", LINE3(0));
        clcd_print("6.STOP", LINE4(0));

        running = 0;
        pause = 0;
        key = 4;

        reset_flag = RESET_NULL;
    }

    if (key == 6)
    {
        running = 0;
        remaining = 0;
        pause = 0;

        FAN = 0;

        return SUCCESS;
    }

    if (key == 4)
    {
        if (pause == 1)
        {
            pause = 0;
            running = 1;
            FAN = 1;
        }
        else if (running == 1)
        {
            remaining = remaining + 30;
        }
        else if (remaining > 0)
        {
            running = 1;
            FAN = 1;
        }
    }
    else if (key == 5)
    {
        if (running == 1)
        {
            running = 0;
            pause = 1;
            FAN = 0;
        }
    }
    
    /* Display updated time */
    clcd_putch((remaining / 60) / 10 + '0', LINE1(7));
    clcd_putch((remaining / 60) % 10 + '0', LINE1(8));
    clcd_putch(':', LINE1(9));
    clcd_putch((remaining % 60) / 10 + '0', LINE1(10));
    clcd_putch((remaining % 60) % 10 + '0', LINE1(11));
    
    if(remaining == 0 && running == 1)
    {
        FAN = 0;
        BUZZER = 1;
        
        running = 0;
        
        clear_screen();
        
        clcd_print("   TIME IS UP   ", LINE1(0));
        clcd_print("   YOUR  FOOD   ", LINE2(0));
        clcd_print("   IS   READY   ", LINE3(0));
        clcd_print("   ENJOY        ", LINE4(0));
        
        __delay_ms(1000);
        BUZZER = 0;

        return SUCCESS;
    }

    return RESET_NULL;
}