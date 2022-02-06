#include "bsp.h"
#define NUM_THREADS 5
typedef struct
{
    int threadIdx;
    unsigned long long sequencePeriods;
} threadParams_t;


/* creating Binary semaphore */
 SemaphoreHandle_t xBinarySemaphore,Sem1, Sem2, Sem3;
int abortTest = 0;
int abortS0 = 0,abortS1 = 0, abortS2 = 0, abortS3 = 0;
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
		init_UART0();
		init_gpio();
		threadParams_t threadParams[NUM_THREADS];
		for(int i = 0; i < NUM_THREADS; i++)
  {
		threadParams[i].threadIdx=i;
	}
		/* Before a semaphore is used it must be explicitly created.  In this example         
		a binary semaphore is created. */     
		xBinarySemaphore = xSemaphoreCreateBinary();  
		Sem1 = xSemaphoreCreateBinary(); 
		Sem2 = xSemaphoreCreateBinary(); 
		Sem3 = xSemaphoreCreateBinary(); 
		
		
		
		
    /* Check the semaphore was created successfully. */     
		if(( xBinarySemaphore != NULL )&& (Sem1 != NULL)  && (Sem2 != NULL)&& (Sem3 != NULL) )
			{  	
				printString("Semaphores created\r\n");
					//printString("Semaphores created successfully \n");
				/* Create the 'handler' task, which is the task to which interrupt         
				processing is deferred.  This is the task that will be synchronized with          
				the interrupt.  The handler task is created with a high priority to ensure          
				it runs immediately after the interrupt exits.  In this case a priority of          
				3 is chosen. */         
				xTaskCreate( vHandlerTask, "Handler", 50, NULL, 4, NULL );  							
				xTaskCreate(service_1, "Service 1", 50, NULL, 3, NULL);
				printString("Service 1 created at \r\n");	
				xTaskCreate(service_2, "Service 2", 50, NULL, 2, NULL);
				printString("Service 2 created at \r\n");	
				xTaskCreate(service_3, "Service 3", 50, NULL, 1, NULL);
				printString("Service 3 created at \r\n");	
				
				/* Create the task that will periodically generate a software interrupt.         
				This is created with a priority below the handler task to ensure it will         
				get preempted each time the handler task exits the Blocked state. */  
threadParams[0].sequencePeriods = 10;				
				 
				/* Create one of the two tasks. */     
				xTaskCreate( vPeriodicTask, /* Pointer to the function that implements the task. */                     
										"Periodic",			/* Text name for the task.  This is to facilitate debugging only. */                     
											100,					/* Stack depth - small microcontrollers will use much less stack than this. */                     
										(void*)&(threadParams[0]),  /* Pass the text to be printed into the task using the task parameter. */                     
											5, 						/* This task will run at priority 5. */                     
										NULL ); 				/* The task handle is not used in this example. */ 
				printString("Sequencer created at \r\n");	
         
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
	printString("Service 1 started at \r\n");	
	while(abortS1 == 0) 
	{
		xSemaphoreTake(Sem1,portMAX_DELAY) ; // portMAX_DELAY
			printString("Service 1 start at \r\n");
			toggleBit(GPIOF->DATA,LEDRED);	
	}
	printString("Deleting Service 1\r\n");
	vTaskDelete(NULL);
}
void service_2(void *threadp)
{
	printString("Service 2 started at \r\n");
	while(abortS2 == 0) 
	{
		xSemaphoreTake(Sem2,portMAX_DELAY);
		printString("Service 2 start at \r\n");
		toggleBit(GPIOF->DATA,LEDGREEN);		
	}
	printString("Deleting Service 2 \r\n");
	vTaskDelete(NULL);
}
void service_3(void *threadp)
{
	printString("Service 3 started at \r\n");
	while(abortS3 == 0) 
	{
		xSemaphoreTake(Sem3,portMAX_DELAY);
		printString("Service 3 start at \r\n");
		toggleBit(GPIOF->DATA,LEDBLUE);			
	}
	printString("Deleting Service 3 \r\n");
	vTaskDelete(NULL);
}
static void vHandlerTask( void *pvParameters )
{     
		/* As per most tasks, this task is implemented within an infinite loop. */     
		while(abortS0 == 0)   
		{         
			/* Use the semaphore to wait for the event.  The semaphore was created         
			before the scheduler was started, so before this task ran for the first         
			time.  The task blocks indefinitely, meaning this function call will only         
			return once the semaphore has been successfully obtained - so there is         
			no need to check the value returned by xSemaphoreTake(). */         
			xSemaphoreTake( xBinarySemaphore, portMAX_DELAY ); 
			printString("Service 0 started at \r\n");			
      /* To get here the event must have occurred.  Process the event (in this         
			Case, just print out a message). */         
			//vPrintString( "Handler task - Processing event.\r\n" );     
		} 
		printString("Deleting Service 0 \r\n");
		vTaskDelete(NULL);
} 
static void vPeriodicTask( void *pvParameters ) 
{ 
	const TickType_t xDelay500ms = pdMS_TO_TICKS( 500UL );  
	unsigned long long seqCnt = 0;
	threadParams_t *threadParams = (threadParams_t *)pvParameters;
	printString("Sequencer started at \r\n");	
 /* As per most tasks, this task is implemented within an infinite loop. */     
	do     
	{  
	//xSemaphoreGive( );
	if((seqCnt % 2) == 0){ xSemaphoreGive(xBinarySemaphore);}
	if((seqCnt % 7) == 0){ xSemaphoreGive(Sem1);}
  // Service_2	@ 10 Hz
  if((seqCnt % 10) == 0){ xSemaphoreGive(Sem2);}
	// Service_3	@ 0.66 Hz
	if((seqCnt % 15) == 0){ xSemaphoreGive(Sem3);}
	if(30 < seqCnt++)
	{
		abortTest = 1;
	}
	/* Block until it is time to generate the software interrupt again. */         
	vTaskDelay( xDelay500ms );        	
	}while(!abortTest && (seqCnt < threadParams->sequencePeriods));
	xSemaphoreGive(xBinarySemaphore);xSemaphoreGive(Sem1);xSemaphoreGive(Sem2);xSemaphoreGive(Sem3);
	abortS0 = 1;abortS1 = 1; abortS2 = 1; abortS3 = 1;
	printString("Deleting Sequencer \r\n");	
	vTaskDelete(NULL);
} 
	

/* Idle hook functions MUST be called vApplicationIdleHook(), take no parameters,
and return void. */
void vApplicationIdleHook( void )
{
 /* This hook function does nothing but increment a counter. */
ulIdleCycleCount++;

}

