#ifndef DD_TASK_LIST_H
#define DD_TASK_LIST_H

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"

#include "dd_tasks.h"

// Define the structure for a node in the linked list
struct dd_task_list {
    struct dd_task task;
    struct dd_task_list* next_task;
};

// Function to create a new node with given task
struct dd_task_list* createNode(struct dd_task newTask);

// Function to insert a new node at the beginning of the linked list
void insertAtBeginning(struct dd_task_list** head, struct dd_task newTask);

// Function to insert a new node at the end of the linked list
void insertAtEnd(struct dd_task_list** head, struct dd_task newTask);

// Count number of nodes in list
unsigned int countItems(struct dd_task_list* head);

// Function to print the elements of the linked list
void printList(struct dd_task_list* head);

// Function to free the memory allocated for the linked list
void freeList(struct dd_task_list** head);

#endif