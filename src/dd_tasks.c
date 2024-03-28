#include "dd_tasks.h"
#include <stdint.h>

void release_dd_task(TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t absolute_deadline ) {

	dd_task *new_task = (dd_task *)pvPortMalloc(sizeof(dd_task)); // allocate memory for new released task
	*new_task = (dd_task){ .t_handle = t_handle, .type = type, .task_id = task_id, .absolute_deadline = absolute_deadline }; // instantiate new_task
	xQueueSend(xTaskCreationQueue, &new_task, 0); // send new task to DDS
}

void complete_dd_task(uint32_t task_id) {
  xQueueSend(xTaskCompletionQueue, &task_id, 0); //send to xTaskCompletionQueue
}

// Will need to confirm and check pointer locations to get this working
uint32_t get_active_dd_task_list(void){
    uint32_t activeCount; // used to store queue value
    xQueueReceive(xActiveTaskListQueue, &activeCount, 0); // recieve from the queue the DDS sent to 
    return activeCount; // return count
}

uint32_t get_complete_dd_task_list(void){
    uint32_t completedCount; // used to store queue value
    xQueueReceive(xCompletedTaskListQueue, &completedCount, 0); // recieve from the queue the DDS sent to 
    return completedCount; // return count
}

uint32_t get_overdue_dd_task_list(void){
    uint32_t overdueCount; // used to store queue value
    xQueueReceive(xOverdueTaskListQueue, &overdueCount, 0); // recieve from the queue the DDS sent to 
    return overdueCount; // return count
}
