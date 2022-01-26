#include <stdint.h>
#include <FreeRTOS.h>
#include <task.h>
#include "bsp.h"
/* Declare a variable that will be incremented by the hook function....*/
volatile uint32_t ulIdleCycleCount = 0UL;
void vPeriodicTask(void *pvParameters);




int main()
{
	xTaskCreate(vPeriodicTask, "My Task", 256, NULL, 1, NULL);

	// Startup of the FreeRTOS scheduler.  The program should block here.  
	vTaskStartScheduler();
	
	// The following line should never be reached.  Failure to allocate enough
	//	memory from the heap would be one reason.
	for (;;);
	
}




void vPeriodicTask(void *pvParameters)
{
	
	// Establish the task's period....
	const TickType_t xDelay = pdMS_TO_TICKS(1000);
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	for (;;) {

		
		// Block until the next release time.
		vTaskDelayUntil(&xLastWakeTime, xDelay);
	}

}	

/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
and return void. */
void vApplicationIdleHook( void )
{
 /* This hook function does nothing but increment a counter. */
ulIdleCycleCount++;

}
