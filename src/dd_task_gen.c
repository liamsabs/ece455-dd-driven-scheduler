#include "dd_task_gen.h"


void periodicTaskGenerator1 (void *pvParameters)
{
	TaskHandle_t xTaskHandle1; 
	while(1)
	{
		xTaskCreate(vPeriodicTask1, "PeriodicTask1", configMINIMAL_STACK_SIZE, NULL, (configMAX_PRIORITIES-4), &xTaskHandle1); // create task
		vTaskSuspend(xTaskHandle1); // Suspend Task for DDS to handle
		release_dd_task(xTaskHandle1, PERIODIC, 1, xTaskGetTickCount() + pdMS_TO_TICKS(TASK_1_PERIOD)); // release task to DDS
		vTaskDelay(pdMS_TO_TICKS(TASK_1_PERIOD)); // block periodic task for task 1 period
	}
}

void periodicTaskGenerator2 (void *pvParameters)
{
	TaskHandle_t xTaskHandle2;
	while(1)
	{
		xTaskCreate(vPeriodicTask2, "PeriodicTask2", configMINIMAL_STACK_SIZE, NULL, (configMAX_PRIORITIES-4), &xTaskHandle2); // create task
		vTaskSuspend(xTaskHandle2); // Suspend Task for DDS to handle
		release_dd_task(xTaskHandle2, PERIODIC, 2, xTaskGetTickCount() + pdMS_TO_TICKS(TASK_2_PERIOD)); // release task to DDS
		vTaskDelay(pdMS_TO_TICKS(TASK_2_PERIOD)); // block periodic task for task 2 period
	}
}


void periodicTaskGenerator3 (void *pvParamters)
{
	TaskHandle_t xTaskHandle3;
	while(1)
	{
		xTaskCreate(vPeriodicTask1, "PeriodicTask3", configMINIMAL_STACK_SIZE, NULL, (configMAX_PRIORITIES-4), &xTaskHandle3); // create task
		vTaskSuspend(xTaskHandle3); // Suspend Task for DDS to handle
		release_dd_task(xTaskHandle3, PERIODIC, 3, xTaskGetTickCount() + pdMS_TO_TICKS(TASK_3_PERIOD)); // release task to DDS
		vTaskDelay(pdMS_TO_TICKS(TASK_3_PERIOD)); // block periodic task for task 3 period
	}
}

void vPeriodicTask1 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount(); // get current tick value
	TickType_t finalTick = currentTick + pdMS_TO_TICKS(TASK_1_EXECUTION_TIME); // get final tick value after execution time
	STM_EVAL_LEDOn(LED3); // turn on amber LED
	while (currentTick < finalTick) // loop while current tick value < final tick value
	    currentTick = xTaskGetTickCount(); // update current tick value
	STM_EVAL_LEDOff(LED3); // turn off amber LED 
	complete_dd_task(1); // complete the task with ID 1
}

void vPeriodicTask2 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount(); // get current tick value
	TickType_t finalTick = currentTick + pdMS_TO_TICKS(TASK_2_EXECUTION_TIME); // get final tick value after execution time
	STM_EVAL_LEDOn(LED5); // turn on red LED
	while (currentTick < finalTick) // loop while current tick value < final tick value
		currentTick = xTaskGetTickCount(); // update current tick value
	STM_EVAL_LEDOff(LED5); // turn off red LED
	complete_dd_task(2); // complete the task
}

void vPeriodicTask3 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount(); // get current tick value
	TickType_t finalTick = currentTick + pdMS_TO_TICKS(TASK_3_EXECUTION_TIME); // get final tick value after execution time 
	STM_EVAL_LEDOn(LED6); // turn on blue LED
	while (currentTick < finalTick) // loop while current tick value < final tick value
		currentTick = xTaskGetTickCount(); // update current tick value
	STM_EVAL_LEDOn(LED6); // turn off blue LED
	complete_dd_task(3); // complete the task
}
