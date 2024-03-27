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

#define active  	0
#define completed  	1
#define overdue  	2

extern xQueueHandle xTaskCreationQueue;
extern xQueueHandle xTaskCompletionQueue;
extern xQueueHandle xTaskListRequestQueue;
extern xQueueHandle xActiveTaskListQueue;
extern xQueueHandle xCompletedTaskListQueue;
extern xQueueHandle xOverdueTaskListQueue;

// enum for state
typedef enum {PERIODIC,APERIODIC} task_type;

typedef struct {
    TaskHandle_t t_handle;
    task_type type;
    uint32_t task_id;
    uint32_t release_time;
    uint32_t absolute_deadline;
    uint32_t completion_time;
} dd_task;

// Define the structure for a node in the linked list
// Define the structure for a node in the linked list
typedef struct dd_task_list {
    dd_task task;
    struct dd_task_list *next_task;
} dd_task_list;

void release_dd_task(TaskHandle_t t_handle, task_type type, uint32_t task_id, uint32_t absolute_deadline);

void complete_dd_task(uint32_t task_id);

uint32_t get_active_dd_task_list(void);

uint32_t get_complete_dd_task_list(void);

uint32_t get_overdue_dd_task_list(void);

// Linked List Functions

// Function to create a new node with given task
dd_task_list* createNode(dd_task newTask);


// Function to insert a new node at the end of the linked list
void insertAtEnd(dd_task_list** head, dd_task newTask);

// Delete Task From List
void deleteTask(dd_task_list** headRef, dd_task_list** nodeToDeleteRef);

// Count number of nodes in list
uint32_t countItems(dd_task_list* head);

// Function to print the elements of the linked list
void printList(dd_task_list* head);

// Function to free the memory allocated for the linked list
void freeList(dd_task_list** head);

#endif
