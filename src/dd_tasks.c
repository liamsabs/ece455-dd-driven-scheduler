#include "dd_tasks.h"
#include <stdint.h>

void release_dd_task(TaskHandle_t t_handle, task_type type, uint32_t task_id,
                    uint32_t absolute_deadline,
                    QueueHandle_t xTaskCreationQueue) {
  dd_task *new_task = pvPortMalloc(sizeof(dd_task));
  new_task->t_handle = t_handle;
  new_task->type = type;
  new_task->task_id = task_id;
  new_task->absolute_deadline = absolute_deadline;
  // Then call or send to DDS when done? Needs to "interface" with it
}

void complete_dd_task(uint32_t task_id) {
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
