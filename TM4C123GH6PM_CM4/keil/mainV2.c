


#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>

#define SETBIT(reg,bit) reg |= (1U << bit)
#define CLEARBIT(reg,bit) reg &= (~(1U << bit))
 
#define GPIO_SETBits(addr,x)  *((volatile unsigned long *)(addr + (x << 2))) = x
#define GPIO_ClrBits(addr,x)  *((volatile unsigned long *)(addr + (x << 2))) = 0
#define LEDRED (1U << 1)
#define LEDGREEN (1U << 3)
#define LEDBLUE (1U << 2)

/* Declare a variable that will be incremented by the hook function. */
volatile uint32_t ulIdleCycleCount = 0UL;

void vPeriodicTask(void *pvParameters);
void Blinky_1();
void Blinky_2();
void Blinky_3();
void init(void);
int main()
{
	init();
//	xTaskCreate(vPeriodicTask, "My Task 0", 56, NULL, 20, NULL);
	xTaskCreate(Blinky_1, "My Task 1", 128, NULL,4, NULL);
	xTaskCreate(Blinky_2, "My Task 2", 128, NULL, 3, NULL);
	//xTaskCreate(Blinky_3, "My Task 3", 128, NULL, 2, NULL);
	// Startup of the FreeRTOS scheduler.  The program should block here.  
	vTaskStartScheduler();
	
	// The following line should never be reached.  Failure to allocate enough
	//	memory from the heap would be one reason.
	for (;;);
	
}

void vPeriodicTask(void *pvParameters)
{
	
	// Establish the task's period.
	const TickType_t xDelay = pdMS_TO_TICKS(1000);
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	for (;;) {

		//SETBIT(GPIOF->DATA,1);
		// Block until the next release time.
		vTaskDelayUntil(&xLastWakeTime, xDelay);
		//i = ~ i;
		//CLEARBIT(GPIOF->DATA,1);
		//vTaskDelayUntil(&xLastWakeTime, xDelay);
	}

}	
void init(void)
{
SYSCTL->RCGCGPIO  |= (1U << 5); /* enable Run mode for GPIOF */
GPIOF->DIR |= (1U << 1)|(1U << 2)|(1U << 3); /* set as output */
GPIOF->DEN |= (1U << 1)|(1U << 2)|(1U << 3); /* digital enabled */
}

void Blinky_1()
{
	for (;;) {
	GPIO_ClrBits(GPIOF_BASE,LEDBLUE);
	GPIO_SETBits(GPIOF_BASE,LEDRED);
	vTaskDelay(500);	
	GPIO_ClrBits(GPIOF_BASE,LEDBLUE);
	GPIO_ClrBits(GPIOF_BASE,LEDRED);
	vTaskDelay(500);	
	}
}
void Blinky_2()
{
	for (;;) 
	{
		GPIO_ClrBits(GPIOF_BASE,LEDBLUE);
		GPIO_SETBits(GPIOF_BASE,LEDGREEN);
		vTaskDelay(250);
		GPIO_ClrBits(GPIOF_BASE,LEDBLUE);
		GPIO_ClrBits(GPIOF_BASE,LEDGREEN);
		vTaskDelay(250);
		
	}
}
void Blinky_3()
{
	for (;;) {
		GPIO_SETBits(GPIOF_BASE,LEDBLUE);
		vTaskDelay(150);
		GPIO_ClrBits(GPIOF_BASE,LEDBLUE);
		vTaskDelay(150);
		
		
	}
}
/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
and return void. */
void vApplicationIdleHook( void )
{
 /* This hook function does nothing but increment a counter. */
ulIdleCycleCount++;
GPIO_SETBits(GPIOF_BASE,LEDBLUE);
}