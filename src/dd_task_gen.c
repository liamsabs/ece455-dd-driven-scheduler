#include "dd_task_gen.h"


void periodicTaskGenerator1 (void *pvParameters)
{
	TaskHandle_t xTaskHandle1; 
	while(1)
	{
		xTaskCreate(vPeriodicTask1, "PeriodicTask1", configMINIMAL_STACK_SIZE, NULL, (configMAX_PRIORITIES-4), &xTaskHandle1); //create task
		vTaskSuspend(xTaskHandle1); // Suspend Task for DDS to handle
		release_dd_task(xTaskHandle1, PERIODIC, 1, xTaskGetTickCount() + pdMS_TO_TICKS(TASK_1_PERIOD)); //release task to DDS
		vTaskDelay(TASK_1_PERIOD); //block periodic task for task 1 period
	}
}

void periodicTaskGenerator2 (void *pvParameters)
{
	TaskHandle_t xTaskHandle2;
	while(1)
	{
		xTaskCreate(vPeriodicTask2, "PeriodicTask2", configMINIMAL_STACK_SIZE, NULL, (configMAX_PRIORITIES-4), &xTaskHandle2); //create task
		vTaskSuspend(xTaskHandle2); // Suspend Task for DDS to handle
		release_dd_task(xTaskHandle2, PERIODIC, 2, xTaskGetTickCount() + pdMS_TO_TICKS(TASK_2_PERIOD)); //release task to DDS
		vTaskDelay(TASK_2_PERIOD); //block periodic task for task 2 period
	}
}


void periodicTaskGenerator3 (void *pvParamters)
{
	TaskHandle_t xTaskHandle3;
	while(1)
	{
		xTaskCreate(vPeriodicTask1, "PeriodicTask3", configMINIMAL_STACK_SIZE, NULL, (configMAX_PRIORITIES-4), &xTaskHandle3); //create task
		vTaskSuspend(xTaskHandle3); // Suspend Task for DDS to handle
		release_dd_task(xTaskHandle3, PERIODIC, 3, xTaskGetTickCount() + pdMS_TO_TICKS(TASK_3_PERIOD)); //release task to DDS
		vTaskDelay(TASK_3_PERIOD); //block periodic task for task 3 period
	}
}

void vPeriodicTask1 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount();
	TickType_t finalTick = currentTick + pdMS_TO_TICKS(TASK_1_EXECUTION_TIME);
	while (currentTick < finalTick)
	    currentTick = xTaskGetTickCount();
	complete_dd_task(1);
}

void vPeriodicTask2 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount();
	TickType_t finalTick = currentTick + pdMS_TO_TICKS(TASK_2_EXECUTION_TIME);
	while (currentTick < finalTick)
		currentTick = xTaskGetTickCount();
	complete_dd_task(2);
}

void vPeriodicTask3 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount();
	TickType_t finalTick = currentTick + pdMS_TO_TICKS(TASK_3_EXECUTION_TIME);
	while (currentTick < finalTick)
		currentTick = xTaskGetTickCount();
	complete_dd_task(3);
}
