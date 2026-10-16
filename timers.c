#include <xc.h>

void init_timer2(void)
{
    /* Selecting the scale as 1:16 */
    T2CKPS0 = 1;
    T2CKPS1 = 1;

    /* Loading the Pre Load register with 250 */
    PR2 = 250;

    TMR2IF = 0;
    TMR2IE = 1;

    /* Timer2 OFF initially */
    TMR2ON = 0;
}