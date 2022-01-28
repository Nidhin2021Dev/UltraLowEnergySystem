#include "bsp.h"


void init_gpio(void)
{
SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF */
GPIOF->DIR |= (1U << 1)|(1U << 2)|(1U << 3); /* set as output */
GPIOF->DEN |= (1U << 1)|(1U << 2)|(1U << 3); /* digital enabled */
}