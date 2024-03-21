#include "dd_tasks.h"
#include <stdint.h>

void release_dd_task(TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t absolute_deadline ) {
  dd_task *new_task = (dd_task *)pvPortMalloc(sizeof(dd_task)); //allocate memory for new released task
  *new_task = (dd_task){ .t_handle = t_handle, .type = type, .task_id = task_id, .absolute_deadline = absolute_deadline }; // instantiate new_task
  xQueueSend(xTaskCreationQueue, new_task, 0); //send new task to DDS
  vportfree(new_task);
}

void complete_dd_task(uint32_t task_id) {
xQueueSend(xTaskCompletionQueue, task_id, 0); //send to xTaskCompletionQueue
}

// Will need to confirm and check pointer locations to get this working
dd_task_list** get_active_dd_task_list(void){

  uint16_t list_type = active;
  dd_task_list* active_list;
  xQueueSend(xTaskListRequestQueue, &list_type, 100);

  while(!xQueueReceive(xActiveTaskListQueue, &active_list, 100));

  return active_list;

}

dd_task_list** get_complete_dd_task_list(void){

  uint16_t list_type = completed;
  dd_task_list* completed_list;
  xQueueSend(xTaskListRequestQueue, &list_type, 100);

  while(!xQueueReceive(xCompletedTaskListQueue, &completed_list, 100));

  return completed_list;

}

dd_task_list** get_overdue_dd_task_list(void){

  uint16_t list_type = overdue;
  dd_task_list* overdue_list;
  xQueueSend(xTaskListRequestQueue, &list_type, 100);

  while(!xQueueReceive(xOverdueTaskListQueue, &overdue_list, 100));

  return overdue_list;

}
