#include "dd_tasks.h"

void create_dd_task( TaskHandle_t t_handle,
                    task_type type,
                    uint32_t task_id,
                    uint32_t absolute_deadline
){
    dd_task* new_task = (dd_task*)pvPortMalloc(sizeof(dd_task));
    new_task->t_handle = t_handle;
    new_task->type = type;
    new_task->task_id = task_id;
    new_task->absolute_deadline = absolute_deadline;
  // Then call or send to DDS when done? Needs to "interface" with it
    
}

void delete_dd_task(uint32_t task_id){

}

dd_task_list** get_active_dd_task_list(void){

}

dd_task_list** get_complete_dd_task_list(void){

}

dd_task_list** get_overdue_dd_task_list(void){

}