#include <xc.h>
#include "microwave.h"

void __interrupt() isr(void)
{
    static unsigned int count = 0;

    if (TMR2IF == 1)
    {
        if (++count == 1250)
        {
            count = 0;

            if (running && remaining > 0)
            {
                remaining--;
            }
        }

        TMR2IF = 0;
    }
}