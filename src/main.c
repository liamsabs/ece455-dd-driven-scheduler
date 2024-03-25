/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/*
FreeRTOS is a market leading RTOS from Real Time Engineers Ltd. that supports
31 architectures and receives 77500 downloads a year. It is professionally
developed, strictly quality controlled, robust, supported, and free to use in
commercial products without any requirement to expose your proprietary source
code.

This simple FreeRTOS demo does not make use of any IO ports, so will execute on
any Cortex-M3 of Cortex-M4 hardware.  Look for TODO markers in the code for
locations that may require tailoring to, for example, include a manufacturer
specific header file.

This is a starter project, so only a subset of the RTOS features are
demonstrated.  Ample source comments are provided, along with web links to
relevant pages on the http://www.FreeRTOS.org site.

Here is a description of the project's functionality:

The main() Function:
main() creates the tasks and software timers described in this section, before
starting the scheduler.

The Queue Send Task:
The queue send task is implemented by the prvQueueSendTask() function.
The task uses the FreeRTOS vTaskDelayUntil() and xQueueSend() API functions to
periodically send the number 100 on a queue.  The period is set to 200ms.  See
the comments in the function for more details.
http://www.freertos.org/vtaskdelayuntil.html
http://www.freertos.org/a00117.html

The Queue Receive Task:
The queue receive task is implemented by the prvQueueReceiveTask() function.
The task uses the FreeRTOS xQueueReceive() API function to receive values from
a queue.  The values received are those sent by the queue send task.  The queue
receive task increments the ulCountOfItemsReceivedOnQueue variable each time it
receives the value 100.  Therefore, as values are sent to the queue every 200ms,
the value of ulCountOfItemsReceivedOnQueue will increase by 5 every second.
http://www.freertos.org/a00118.html

An example software timer:
A software timer is created with an auto reloading period of 1000ms.  The
timer's callback function increments the ulCountOfTimerCallbackExecutions
variable each time it is called.  Therefore the value of
ulCountOfTimerCallbackExecutions will count seconds.
http://www.freertos.org/RTOS-software-timer.html

The FreeRTOS RTOS tick hook (or callback) function:
The tick hook function executes in the context of the FreeRTOS tick interrupt.
The function 'gives' a semaphore every 500th time it executes.  The semaphore
is used to synchronise with the event semaphore task, which is described next.

The event semaphore task:
The event semaphore task uses the FreeRTOS xSemaphoreTake() API function to
wait for the semaphore that is given by the RTOS tick hook function.  The task
increments the ulCountOfReceivedSemaphores variable each time the semaphore is
received.  As the semaphore is given every 500ms (assuming a tick frequency of
1KHz), the value of ulCountOfReceivedSemaphores will increase by 2 each second.

The idle hook (or callback) function:
The idle hook function queries the amount of free FreeRTOS heap space available.
See vApplicationIdleHook().

The malloc failed and stack overflow hook (or callback) functions:
These two hook functions are provided as examples, but do not contain any
functionality.
*/

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "dd_tasks.h"


/*-----------------------------------------------------------*/
#define mainQUEUE_LENGTH 100

#define amber  	0
#define green  	1
#define red  	2
#define blue  	3

#define amber_led	LED3
#define green_led	LED4
#define red_led		LED5
#define blue_led	LED6


/*
 * TODO: Implement this function for any hardware specific clock configuration
 * that was not already performed before main() was called.
 */
static void prvSetupHardware( void );

/*
 * The queue send and receive tasks as described in the comments at the top of
 * this file.
 */
static void Manager_Task( void *pvParameters );
static void Blue_LED_Controller_Task( void *pvParameters );
static void Green_LED_Controller_Task( void *pvParameters );
static void Red_LED_Controller_Task( void *pvParameters );
static void Amber_LED_Controller_Task( void *pvParameters );

static void DDS_Task( void *pvParameters );
static void User_Defined_Task( void *pvParameters );
static void DD_Task_Generator_Task( void *pvParameters );
static void Monitor_Task( void *pvParameters );

xQueueHandle xQueue_handle = 0;


static void vDDSTimerCallback ( xTimerHandle timerHandler );
static void vTaskTimerCallback ( xTimerHandle timerHandler );
static void vTaskGenTimerCallback ( xTimerHandle timerHandler );

//Queue to pass created user defined tasks between the DDS and other Tasks
xQueueHandle xTaskCreationQueue = 0;
xQueueHandle xTaskCompletionQueue = 0;

// Queue to recieve list requests from the monitor task
xQueueHandle xTaskListRequestQueue = 0;
//Queues used to keep track of the 3 task lists
xQueueHandle xActiveTaskListQueue = 0;
xQueueHandle xCompletedTaskListQueue = 0;
xQueueHandle xOverdueTaskListQueue = 0;

// Timer Handlers
xTimerHandle xTaskGenTimerHandler = 0;
xTimerHandle xTaskTimerHandler = 0;


/*-----------------------------------------------------------*/

int main(void)
{

	/* Initialize LEDs */
	STM_EVAL_LEDInit(amber_led);
	STM_EVAL_LEDInit(green_led);
	STM_EVAL_LEDInit(red_led);
	STM_EVAL_LEDInit(blue_led);

	prvSetupHardware();


	/* Create the queue used by the queue send and queue receive tasks.
	http://www.freertos.org/a00116.html */
	xQueue_handle = xQueueCreate( 	mainQUEUE_LENGTH,		/* The number of items the queue can hold. */
							sizeof( uint16_t ) );	/* The size of each item the queue holds. */

	/* Add to the registry, for the benefit of kernel aware debugging. */
	vQueueAddToRegistry( xQueue_handle, "MainQueue" );

	/*
	The below tasks are for the LEDs, which we may be using for testing.
	*/
	xTaskCreate( Manager_Task, "Manager", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate( Blue_LED_Controller_Task, "Blue_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Red_LED_Controller_Task, "Red_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Green_LED_Controller_Task, "Green_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate( Amber_LED_Controller_Task, "Amber_LED", configMINIMAL_STACK_SIZE, NULL, 1, NULL);



	/*
	TODO: add the xTaskCreate declarations for our tasks, and edit the priorities of the above
	Important: DDS Task must be the highest priority, and Monitor Task should be the lowest, as it'll just run when nothing else is running
	*/
	
	
	/* Setup Queues */
	xTaskCreationQueue = xQueueCreate(1, sizeof(dd_task*));
	xTaskCompletionQueue = xQueueCreate(1, sizeof(uint32_t));
	xTaskListRequestQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof( uint16_t ));
	xActiveTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task_list*));
	xCompletedTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task_list*));
	xOverdueTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task_list*));

	/*
	Beginning of our tasks and timers
	*/
	TimerHandle_t Task_Timer = 
				xTimerCreate("TaskTimer", 
				pdMS_TO_TICKS(1000), 
				pdFALSE, 
				(void *) 0, 
				vTaskTimerCallback);
	
	// Need to somehow add the timer to the queue, or allow Task Generator to see it somehow?

	TimerHandle_t Task_Generator_Timer =
				xTimerCreate("TaskGenTimer",
				pdMS_TO_TICKS(1000),
				pdFALSE,
				(void *) 1,
				vTaskGenTimerCallback);


	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

static void DDS_Task( void *pvParameters ){
//Niaomi to-do: queue recieving and requests from task list
//Will add this asap

	TimerHandle_t DDS_Timer = 
				xTimerCreate("DDSTimer", 
				pdMS_TO_TICKS(1000), 
				pdFALSE, 
				(void *) 0, 
				vDDSTimerCallback);

	dd_task taskToSchedule; // contents of xTaskCreationQueue will be copied to this
	dd_task taskToComplete; // contents of xTaskCompletionQueue will be copied to this
	xTaskHandle taskToRelease; // this handle will release the task with the soonest relative deadline
	
	dd_task_list* active_list_head;
	dd_task_list* completed_list_head;
	dd_task_list* overdue_list_head;

	uint16_t list_type;

	// Not sure if everything should be in a while(1) loop?
	while(1){
		

		
		if(xQueueReceive(xTaskCreationQueue, &taskToSchedule, 100)){
			dd_task_list** taskToDispatch = NULL;
			dd_task_list** currentNode = active_list_head;
			insertAtEnd(&active_list_head, taskToSchedule);
			for(dd)


		}else if(xQueueReceive(xTaskCompletionQueue, &taskToComplete, 100)){
			dd_task_list** taskToComplete = NULL;
			dd_task_list** currentNode = completed_list_head;
			insertAtEnd(&completed_list_head, taskToComplete);
			for(dd)

		
		}else if(xQueueReceive(xTaskListRequestQueue, &list_type, 100)){
		/*
		This checks if the get_xxx_task_list has sent a request, and what type of list they want
		Then, it updates the appropriate queue and sends the requested list along.
		*/
			if(list_type == active){
				xQueueSend(xActiveTaskListQueue, &active_list_head, 100);
			}else if(list_type == completed){
				xQueueSend(xCompletedTaskListQueue, &completed_list_head, 100);
			}else if(list_type == overdue){
				xQueueSend(xOverdueTaskListQueue, &overdue_list_head, 100);
			}
		}
	}
	

}

/*-----------------------------------------------------------*/

static void DD_Task_Generator_Task( void *pvParameters ){
	
}

/*-----------------------------------------------------------*/

static void Monitor_Task( void *pvParameters ){

	/*
	Lowest priority task, will run whenever there is a break in execution of other tasks and reports the number of active, completed and overdue tasks
	*/

	dd_task_list* active_list_head;
	dd_task_list* completed_list_head;
	dd_task_list* overdue_list_head;

	unsigned int active_tasks;
	unsigned int completed_tasks;
	unsigned int overdue_tasks;

	while(1){

		active_list_head = get_active_dd_task_list();
		active_tasks = countItems(active_list_head);

		completed_list_head = get_complete_dd_task_list();
		completed_tasks = countItems(completed_list_head);

		overdue_list_head = get_overdue_dd_task_list();
		overdue_tasks = countItems(overdue_list_head);

	}
}

/*-----------------------------------------------------------*/

static void vDDSTimerCallback ( xTimerHandle timerHandler ){
	/*
	When timer done, system should call Task Generator to create a new periodic task
	*/
}

static void vTaskTimerCallback ( xTimerHandle timerHandler ){
	/*
	When timer done, system should call Task Generator to create a new periodic task
	*/
}

static void vTaskGenTimerCallback ( xTimerHandle timerHandler ){
	/*
	When timer done, system should call Task Generator to create a new periodic task
	*/
}

/*-----------------------------------------------------------*/

static void Manager_Task( void *pvParameters )
{
	uint16_t tx_data = amber;


	while(1)
	{

		if(tx_data == amber)
			STM_EVAL_LEDOn(amber_led);
		if(tx_data == green)
			STM_EVAL_LEDOn(green_led);
		if(tx_data == red)
			STM_EVAL_LEDOn(red_led);
		if(tx_data == blue)
			STM_EVAL_LEDOn(blue_led);

		if( xQueueSend(xQueue_handle,&tx_data,1000))
		{
			printf("Manager: %u ON!\n", tx_data);
			if(++tx_data == 4)
				tx_data = 0;
			vTaskDelay(1000);
		}
		else
		{
			printf("Manager Failed!\n");
		}
	}
}

/*-----------------------------------------------------------*/

static void Blue_LED_Controller_Task( void *pvParameters )
{
	uint16_t rx_data;
	while(1)
	{
		if(xQueueReceive(xQueue_handle, &rx_data, 500))
		{
			if(rx_data == blue)
			{
				vTaskDelay(250);
				STM_EVAL_LEDOff(blue_led);
				printf("Blue Off.\n");
			}
			else
			{
				if( xQueueSend(xQueue_handle,&rx_data,1000))
					{
						printf("BlueTask GRP (%u).\n", rx_data); // Got wwrong Package
						vTaskDelay(500);
					}
			}
		}
	}
}


/*-----------------------------------------------------------*/

static void Green_LED_Controller_Task( void *pvParameters )
{
	uint16_t rx_data;
	while(1)
	{
		if(xQueueReceive(xQueue_handle, &rx_data, 500))
		{
			if(rx_data == green)
			{
				vTaskDelay(250);
				STM_EVAL_LEDOff(green_led);
				printf("Green Off.\n");
			}
			else
			{
				if( xQueueSend(xQueue_handle,&rx_data,1000))
					{
						printf("GreenTask GRP (%u).\n", rx_data); // Got wrong Package
						vTaskDelay(500);
					}
			}
		}
	}
}

/*-----------------------------------------------------------*/

static void Red_LED_Controller_Task( void *pvParameters )
{
	uint16_t rx_data;
	while(1)
	{
		if(xQueueReceive(xQueue_handle, &rx_data, 500))
		{
			if(rx_data == red)
			{
				vTaskDelay(250);
				STM_EVAL_LEDOff(red_led);
				printf("Red off.\n");
			}
			else
			{
				if( xQueueSend(xQueue_handle,&rx_data,1000))
					{
						printf("RedTask GRP (%u).\n", rx_data); // Got wrong Package
						vTaskDelay(500);
					}
			}
		}
	}
}


/*-----------------------------------------------------------*/

static void Amber_LED_Controller_Task( void *pvParameters )
{
	uint16_t rx_data;
	while(1)
	{
		if(xQueueReceive(xQueue_handle, &rx_data, 500))
		{
			if(rx_data == amber)
			{
				vTaskDelay(250);
				STM_EVAL_LEDOff(amber_led);
				printf("Amber Off.\n");
			}
			else
			{
				if( xQueueSend(xQueue_handle,&rx_data,1000))
					{
						printf("AmberTask GRP (%u).\n", rx_data); // Got wrong Package
						vTaskDelay(500);
					}
			}
		}
	}
}


/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

