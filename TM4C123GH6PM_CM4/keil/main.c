#include "bsp.h"

/* creating Binary semaphore */
 SemaphoreHandle_t xBinarySemaphore,Sem1, Sem2, Sem3;
 
/* Declare a variable that will be incremented by the hook function....*/
volatile uint32_t ulIdleCycleCount = 0UL;

void ErrorFxn(char *err);


static void service_1(void *threadp);
static void service_2(void *threadp);
static void service_3(void *threadp);
static void vPeriodicTask( void *pvParameters );
static void vHandlerTask( void *pvParameters );


int main( void ) 
	{     
		init_gpio();
		/* Before a semaphore is used it must be explicitly created.  In this example         
		a binary semaphore is created. */     
		xBinarySemaphore = xSemaphoreCreateBinary();  
		Sem1 = xSemaphoreCreateBinary(); Sem2 = xSemaphoreCreateBinary(); Sem3 = xSemaphoreCreateBinary(); 
    /* Check the semaphore was created successfully. */     
		if(( xBinarySemaphore != NULL )&& (Sem1 != NULL)  && (Sem2 != NULL)&& (Sem3 != NULL) )
			{         
				/* Create the 'handler' task, which is the task to which interrupt         
				processing is deferred.  This is the task that will be synchronized with          
				the interrupt.  The handler task is created with a high priority to ensure          
				it runs immediately after the interrupt exits.  In this case a priority of          
				3 is chosen. */         
				xTaskCreate( vHandlerTask, "Handler", 50, NULL, 4, NULL );  
        /* Create the task that will periodically generate a software interrupt.         
				This is created with a priority below the handler task to ensure it will         
				get preempted each time the handler task exits the Blocked state. */         
				xTaskCreate( vPeriodicTask, "Periodic", 100, NULL, 5, NULL );  
				
				xTaskCreate(service_1, "Service 1", 50, NULL, 3, NULL);
				xTaskCreate(service_2, "Service 2", 50, NULL, 2, NULL);
				xTaskCreate(service_3, "Service 3", 50, NULL, 1, NULL);
         
        /* Start the scheduler so the created tasks start executing. */         
				vTaskStartScheduler();     
			}  
    /* As normal, the following line should never be reached. */     
	for( ;; ); 
	} 


void ErrorFxn(char *err)
{
	int i =0;
	while(1)
	{
		if(i == 0)
		{
			i = 1;
			while(*err)
			{
			//printf("%c",*err);
				toggleBit(GPIOF->DATA,LEDBLUE);
			}
		}
	}
}

void service_1(void *threadp)
{
	for(;;)
	{
		xSemaphoreTake(Sem1,portMAX_DELAY) ; // portMAX_DELAY
			//puts("service 1\n");
			toggleBit(GPIOF->DATA,LEDRED);	
	}
}
void service_2(void *threadp)
{
	for(;;)
	{
		xSemaphoreTake(Sem2,portMAX_DELAY);
		toggleBit(GPIOF->DATA,LEDGREEN);		
	}
}
void service_3(void *threadp)
{
	for(;;)
	{
		xSemaphoreTake(Sem3,portMAX_DELAY);
		toggleBit(GPIOF->DATA,LEDBLUE);			
	}
}
static void vHandlerTask( void *pvParameters )
{     
		/* As per most tasks, this task is implemented within an infinite loop. */     
		for( ;; )     
		{         
			/* Use the semaphore to wait for the event.  The semaphore was created         
			before the scheduler was started, so before this task ran for the first         
			time.  The task blocks indefinitely, meaning this function call will only         
			return once the semaphore has been successfully obtained - so there is         
			no need to check the value returned by xSemaphoreTake(). */         
			xSemaphoreTake( xBinarySemaphore, portMAX_DELAY ); 
			vTaskDelay(5);			
      /* To get here the event must have occurred.  Process the event (in this         
			Case, just print out a message). */         
			//vPrintString( "Handler task - Processing event.\r\n" );     
		} 
} 
static void vPeriodicTask( void *pvParameters ) 
{ 
	const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );  
	unsigned long long seqCnt = 0;
 /* As per most tasks, this task is implemented within an infinite loop. */     
	for( ;; )     
	{  
	xSemaphoreGive( xBinarySemaphore);
	if((seqCnt % 2) == 0){ xSemaphoreGive(Sem1);}
  // Service_2	@ 10 Hz
  if((seqCnt % 10) == 0){ xSemaphoreGive(Sem2);}
	// Service_3	@ 0.66 Hz
	if((seqCnt % 15) == 0){ xSemaphoreGive(Sem3);}
	if(30 < seqCnt++)
	{
	seqCnt = 0;
	}		
	/* Block until it is time to generate the software interrupt again. */         
	vTaskDelay( xDelay500ms );        	
	}
} 
	

/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
and return void. */
void vApplicationIdleHook( void )
{
 /* This hook function does nothing but increment a counter. */
ulIdleCycleCount++;

}

