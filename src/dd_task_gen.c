#include "dd_task_gen.h"


void periodicTaskGenerator1 (void *pvParameters)
{
	while(1)
	{
		vTaskDelay(TASK_1_PERIOD);
	}
}

void periodicTaskGenerator2 (void *pvParameters)
{
	while(1)
	{
		vTaskDelay(TASK_2_PERIOD);
	}
}


void periodicTaskGenerator3 (void *pvParamters)
{
	while(1)
	{
		vTaskDelay(TASK_3_PERIOD);
	}
}

void periodicTask1 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount();
	TickType_t finalTick = currentTick + (TASK_1_EXECUTION_TIME / portTICK_PERIOD_MS);
	while (currentTick < finalTick)
	    currentTick = xTaskGetTickCount();
}

void periodicTask2 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount();
	TickType_t finalTick = currentTick + (TASK_2_EXECUTION_TIME / portTICK_PERIOD_MS);
	while (currentTick < finalTick)
		currentTick = xTaskGetTickCount();
}

void periodicTask3 (void *pvParameters)
{
	TickType_t currentTick = xTaskGetTickCount();
	TickType_t finalTick = currentTick + (TASK_3_EXECUTION_TIME / portTICK_PERIOD_MS);
	while (currentTick < finalTick)
		currentTick = xTaskGetTickCount();
}
