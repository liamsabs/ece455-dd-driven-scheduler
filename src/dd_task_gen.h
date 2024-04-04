#ifndef DD_TASK_GEN_H_
#define DD_TASK_GEN_H_

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
#include "stm32f4_discovery.h"
#include "dd_tasks.h"

#define TASK_1_PERIOD 250
#define TASK_2_PERIOD 500
#define TASK_3_PERIOD 750

#define TASK_1_EXECUTION_TIME 95
#define TASK_2_EXECUTION_TIME 150
#define TASK_3_EXECUTION_TIME 250
#define IDLE_TASK_PRIORITY (configMAX_PRIORITIES - 4) 

/* Dispatches PeriodicTask1 */
void periodicTaskGenerator1 (void *pvParameters);

/* Dispatches PeriodicTask2 */
void periodicTaskGenerator2 (void *pvParameters);

/* Dispatches PeriodicTask3 */
void periodicTaskGenerator3 (void *pvParamters);

/* 1st Periodic Task to be scheduled */
void vPeriodicTask1 (void *pvParameters);

/* 2nd Periodic Task to be scheduled */
void vPeriodicTask2 (void *pvParameters);

/* 3rd Periodic Task to be scheduled */
void vPeriodicTask3 (void *pvParameters);

#endif
