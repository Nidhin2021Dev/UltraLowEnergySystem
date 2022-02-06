#include "bsp.h"


void init_gpio(void)
{
SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF */
GPIOF->DIR |= (1U << 1)|(1U << 2)|(1U << 3); /* set as output */
GPIOF->DEN |= (1U << 1)|(1U << 2)|(1U << 3); /* digital enabled */
}

void init_UART0()
{
		SYSCTL->RCGCUART |= 0x01;  /* enable clock to UART0 */
	  //Delay(1);
    SYSCTL->RCGCGPIO |= 0x01;  /* enable clock to PORTA for PA0/Rx and RA1/Tx */
    //Delay(1);
    /* UART0 initialization */
	GPIOA->LOCK = 0x4C4F434B;
	GPIOA->CR = 0X03;
	/* UART5 TX5 and RX5 use PE4 and PE5. Configure them digital and enable alternate function */
    GPIOA->DEN = 0x03;      /* set PA0 and PA1 as digital */
    GPIOA->AFSEL = 0x03;    /* Use PA0,PA1 alternate function */
    GPIOA->AMSEL = 0;    /* Turn off analg function*/
    GPIOA->PCTL |= (GPIOA->PCTL & (~0x00000011))+0x00000011;     /* configure PA0 and PA1 for UART */

	//Delay(1);
    UART0->CTL = 0;         /* UART5 module disbable */
    UART0->IBRD = 104;      /* for 9600 baud rate, integer = 104 */
    UART0->FBRD = 11;       /* for 9600 baud rate, fractional = 11*/
    UART0->CC = 0x05;          /*select system clock*/
    UART0->LCRH = 0x60;     /* data lenght 8-bit, not parity bit, no FIFO */
    UART0->CTL |= 0x01 |(1U << 8)|(1U <<9);     /* Enable UART5 module, Rx and Tx */

    
	
	 // Delay(1); 
}

char readChar(void)
{
	char c;
	while(!(UART0->FR & (1 << 4)));
	c = UART0->DR;
	return c;	
	
}
void printChar(char c)
{
	
	while((UART0->FR & (0x01 << 5)));
	UART0->DR = c;
}
void printString(char *str)
{
	while(*str)
	{
		printChar(*(str++));
	}
}
void Delay(unsigned long counter)
{
	unsigned long i = 0;
	
	for(i=0; i< counter; i++)
	{
		for(int j = 0;j < 5000;j++);
	}
}
