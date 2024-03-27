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
#define mainQUEUE_LENGTH 1


static void prvSetupHardware( void );
static void DDS_Task( void *pvParameters );
static void Monitor_Task( void *pvParameters );
static void vTaskTimerCallback ( xTimerHandle timerHandler );
static void vTaskGenTimerCallback ( xTimerHandle timerHandler );

/*Queue to pass created user defined tasks between the DDS and other Tasks*/
xQueueHandle xTaskCreationQueue = 0; //passed from task generator with task info
xQueueHandle xTaskExecutionQueue = 0; //passed from DDS onto queue to be taken by DDS once scheduled task executes
xQueueHandle xTaskCompletionQueue = 0;//passed to DDS to indicate 

// Queue to recieve list requests from the monitor task
xQueueHandle xTaskListRequestQueue = 0;
//Queues used to keep track of the 3 task lists
xQueueHandle xActiveTaskListQueue = 0;
xQueueHandle xCompletedTaskListQueue = 0;
xQueueHandle xOverdueTaskListQueue = 0;


/*-----------------------------------------------------------*/

int main(void)
{

	/* Initialize LEDs */
	STM_EVAL_LEDInit(LED4);
	STM_EVAL_LEDInit(LED5);
	STM_EVAL_LEDInit(LED6);

	/* Setup Hardware */
	prvSetupHardware();

	/*
	TODO: add the xTaskCreate declarations for our tasks, and edit the priorities of the above
	Important: DDS Task must be the highest priority, and Monitor Task should be the lowest, as it'll just run when nothing else is running
	*/
	
	
	/* Setup Queues */
	xTaskCreationQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task*));
	xTaskExecutionQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task));
	xTaskCompletionQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));
	xTaskListRequestQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof( uint16_t ));
	xActiveTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task_list*));
	xCompletedTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task_list*));
	xOverdueTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(dd_task_list*));

	/* Register Queues */
	QueueAddToRegistry(xTaskCreationQueue, "TaskCreationQueue");
	vQueueAddToRegistry(xTaskExecutionQueue, "TaskExecutionQueue");
	vQueueAddToRegistry(xTaskCompletionQueue, "TaskCompletionQueue");

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

static void DDS_Task( void *pvParameters ){

	dd_task taskToSchedule; // contents of xTaskCreationQueue will be copied to this
	xTaskHandle taskToRelease; // this handle will release the task with the soonest relative deadline
	uint32_t completedTaskID; // ID of completed task sent from periodic task 
	dd_task completedTask; // ID of completed task 
	
	dd_task_list* active_list_head;
	dd_task_list* completed_list_head;
	dd_task_list* overdue_list;

	// Not sure if everything should be in a while(1) loop?
	while(1){
		uint16_t list_type;

		/* for the creation of a task and adding to the active task list*/
		if(xQueueReceive(xTaskCreationQueue, &taskToSchedule, 100)){
			
			dd_task_list** taskToDispatch = &active_list_head; // used to store task with soonest deadline while iterating through list
			dd_task_list** currentNode = &active_list_head; // current Node of List to iterate through
			
			insertAtEnd(&active_list_head, taskToSchedule); //insert the dd_task recieved from queue onto the list

			while(currentNode != NULL){  // iterate through list
				if((*currentNode)->task.absolute_deadline < (*taskToDispatch)->task.absolute_deadline){ // if item in list has sooner deadline than current
					taskToDispatch = currentNode; //update taskToDispatch to current Node 
				}
				currentNode = (*currentNode)->next_task; // continue iterating though
			}

			dd_task ddTaskToDispatch = (*taskToDispatch)->task; // get dd_task to dispatch to add to execution queue and to delete from list

			deleteTask(active_list_head, taskToDispatch); // Delete the task from the active task list

			xQueueSend(xTaskExecutionQueue, &ddTaskToDispatch, 0); // add the task to the task executing queue

			vTaskPrioritySet(ddTaskToDispatch.t_handle, configMAX_PRIORITIES-1); // set the task priority to high

			vTaskResume(ddTaskToDispatch.t_handle); // resume the task with the soonest deadline

		}else if(xQueueReceive(xTaskCompletionQueue, &completedTaskID, 100)){ /* For once a scheduled task completes */
			
			xQueueReceive(xTaskExecutionQueue, &completedTask, 100); // recieve from the executing task queue
		
		
		}else if(xQueueReceive(xTaskListRequestQueue, &list_type, 100)){
			if(list_type == active){
				xQueueSend(xActiveTaskListQueue, &active_list, 100);
			}else if(list_type == completed){
				xQueueSend(xCompletedTaskListQueue, &completed_list, 100);
			}else if(list_type == overdue){
				xQueueSend(xOverdueTaskListQueue, &overdue_list, 100);
			}
		}
	}
	

}

/*-----------------------------------------------------------*/

static void User_Defined_Task( void *pvParameters ){

}

/*-----------------------------------------------------------*/

static void DD_Task_Generator_Task( void *pvParameters ){
	uint32_t new_task_ID = 0;
	//create_dd_task(TaskHandle_t t_handle, PERIODIC, new_task_ID, 1000);
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