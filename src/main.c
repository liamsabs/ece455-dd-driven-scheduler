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
#include "dd_task_gen.h"

/*-----------------------------------------------------------*/

#define mainQUEUE_LENGTH 1

/*-----------------------------------------------------------*/

#define DDS_PRIORITY (configMAX_PRIORITIES)
#define PERIODIC_TASK_GEN_PRIORITY (configMAX_PRIORITIES - 1)
#define ACTIVE_TASK_PRIORITY (configMAX_PRIORITIES - 2)
#define MONITOR_TASK_PRIORITY (configMAX_PRIORITIES - 3)

/*-----------------------------------------------------------*/

#define MONITOR_TASK_PERIOD (pdMS_TO_TICKS(200))

/*-----------------------------------------------------------*/

static void prvSetupHardware( void );
static void prvDDSTask( void *pvParameters );
static void prvMonitorTask( void *pvParameters );

/*-----------------------------------------------------------*/

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
	xActiveTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));
	xCompletedTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));
	xOverdueTaskListQueue = xQueueCreate(mainQUEUE_LENGTH, sizeof(uint32_t));

	/* Register Queues */
	QueueAddToRegistry(xTaskCreationQueue, "TaskCreationQueue");
	vQueueAddToRegistry(xTaskExecutionQueue, "TaskExecutionQueue");
	vQueueAddToRegistry(xTaskCompletionQueue, "TaskCompletionQueue");

	/* Create the Tasks */
	xTaskCreate(prvDDSTask, "DDS Task", configMINIMAL_STACK_SIZE, NULL, DDS_PRIORITY, NULL);
	xTaskCreate(periodicTaskGenerator1, "Periodic Task Gen 1", configMINIMAL_STACK_SIZE, NULL, PERIODIC_TASK_GEN_PRIORITY, NULL);
	xTaskCreate(periodicTaskGenerator2, "Periodic Task Gen 2", configMINIMAL_STACK_SIZE, NULL, PERIODIC_TASK_GEN_PRIORITY, NULL);
	xTaskCreate(periodicTaskGenerator3, "Periodic Task Gen 3", configMINIMAL_STACK_SIZE, NULL, PERIODIC_TASK_GEN_PRIORITY, NULL);
	xTaskCreate(prvMonitorTask, "Monitor Task", configMINIMAL_STACK_SIZE, NULL, MONITOR_TASK_PRIORITY, NULL);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return 0;
}

/*-----------------------------------------------------------*/

static void prvDDSTask( void *pvParameters ){

	dd_task taskToSchedule; // contents of xTaskCreationQueue will be copied to this
	xTaskHandle taskToRelease; // this handle will release the task with the soonest relative deadline
	uint32_t completedTaskID; // ID of completed task sent from periodic task 
	dd_task completedTask; // completed dd_task from executing task queue
	
	dd_task_list* active_list_head; // head of linked list for tasks to be scheduled
	dd_task_list* completed_list_head; // head of linked list for tasks that have been completed
	dd_task_list* overdue_list_head; // head of linked list for tasks that finished completing late

	uint16_t listrequest; // recieved to indicate the monitor is requesting list counts
	uint32_t active_list_count; // active list count sent to monitor
	uint32_t completed_list_count; // completed list count sent to monitor
	uint32_t overdue_list_count; // overdue list count sent to monitor

	while(1){
		
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

			if (ddTaskToDispatch.t_handle != NULL) { // check to make sure task handle is valid then dispatch
				vTaskPrioritySet(ddTaskToDispatch.t_handle, (configMAX_PRIORITIES - 1)); // set the task priority to high
				vTaskResume(ddTaskToDispatch.t_handle); // resume the task with the soonest deadline
			}

		}else if(xQueueReceive(xTaskCompletionQueue, &completedTaskID, 100)){ /* For once a scheduled task completes */
			
			xQueueReceive(xTaskExecutionQueue, &completedTask, 100); // recieve from the executing task queue
			completedTask.completion_time = xTaskGetTickCount(); // record completion time of the task

			/*Determine which list to add the completed task to*/
			if(completedTask.completion_time < completedTask.absolute_deadline){ // task met deadline
				insertAtEnd(completed_list_head, completedTask); //insert at end of completed list
			}else{ // task did not meet deadline
				insertAtEnd(overdue_list_head, completedTask); // insert at end of overdue list
			}

		}else if(xQueueReceive(xTaskListRequestQueue, &listrequest, 100)){ // request for task lists by monitor task
			active_list_count = countItems(active_list_head); // count items in active list
			completed_list_count = countItems(completed_list_head); // count items in completed list
			overdue_list_head = countItems(overdue_list_head); // count items in overdue list
			xQueueSend(xActiveTaskListQueue, &active_list_count, 0); // send active count
			xQueueSend(xCompletedTaskListQueue, &completed_list_count, 0); // send completed count
			xQueueSend(xOverdueTaskListQueue, &overdue_list_count, 0); // send overdue count
		}	
	}
}

/*-----------------------------------------------------------*/

static void prvMonitorTask( void *pvParameters ){

	uint16_t monitorDDSRequest = 0;
	uint32_t active_tasks;
	uint32_t completed_tasks;
	uint32_t overdue_tasks;

	while(1){
		xQueueSend(xTaskListRequestQueue, monitorDDSRequest, 0);
        active_tasks = get_active_dd_task_list();
		completed_tasks = get_complete_dd_task_list();
		overdue_tasks = get_overdue_dd_task_list();
		printf("Active: %d Completed: %d Overdue: %d", active_tasks, completed_tasks, overdue_tasks);
		vTaskDelay(MONITOR_TASK_PERIOD);
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