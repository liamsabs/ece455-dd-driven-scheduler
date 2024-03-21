#include "dd_task_gen.h"


void periodicTaskGenerator1 (void *pvParameters)
{
	while(1)
	{
		xTaskHandle pTask1Handle_t;
		create_dd_task(PERIODIC, pTask1Handle_t);
		vTaskDelay(pdMS_TO_TICKS(TASK_1_PERIOD));
	}
}

void periodicTaskGenerator2 (void *pvParameters)
{
	while(1)
	{

		vTaskDelay(pdMS_TO_TICKS(TASK_2_PERIOD));
	}
}


void periodicTaskGenerator3 (void *pvParamters)
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(TASK_3_PERIOD));
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
