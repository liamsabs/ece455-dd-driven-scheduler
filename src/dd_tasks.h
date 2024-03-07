#ifndef DD_TASKS_H
#define DD_TASKS_H

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

#include "dd_task_list.h"

// enum for state
typedef enum {PERIODIC,APERIODIC} task_type;

// deadline-driven schedule handler struct
struct dd_task {
    TaskHandle_t t_handle;
    task_type type;
    uint32_t task_id;
    uint32_t release_time;
    uint32_t absolute_deadline;
    uint32_t completion_time;
};


void create_dd_task( TaskHandle_t t_handle,
task_type type,
uint32_t task_id,
uint32_t absolute_deadline
);

void delete_dd_task(uint32_t task_id);

**dd_task_list get_active_dd_task_list(void);

**dd_task_list get_complete_dd_task_list(void);

**dd_task_list get_overdue_dd_task_list(void);
