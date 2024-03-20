#include "dd_tasks.h"

// Function to create a new node with given task
dd_task_list* createNode(dd_task newTask) {
    dd_task_list* newNode = (dd_task_list*)pvPortMalloc(sizeof(dd_task_list));
    if (newNode == NULL) {
        // Handle memory allocation failure
    }
    newNode->task = newTask;
    newNode->next_task = NULL;
    return newNode;
}

// Function to insert a new node at the beginning of the linked list
void insertAtBeginning(dd_task_list** head, dd_task newTask) {
    dd_task_list* newNode = createNode(newTask);
    newNode->next_task = *head;
    *head = newNode;
}

// Function to insert a new node at the end of the linked list
void insertAtEnd(dd_task_list** head, dd_task newTask) {
    dd_task_list* newNode = createNode(newTask);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    dd_task_list* temp = *head;
    while (temp->next_task != NULL) {
        temp = temp->next_task;
    }
    temp->next_task = newNode;
}


// Really not sure how to make this work, pointers are iffy for me
void deleteTask(dd_task_list** head, dd_task Task){
    if (*head == NULL) {
        return;
    }
    
    if ((*head)->task.task_id == Task.task_id){
        dd_task_list* del_node = *head;
        *head = (*head)->next_task;
        freeList(&(del_node));
        return;
    }

    deleteTask(&(*head)->next_task, Task);
}

// Implementation of the countItems function
unsigned int countItems(dd_task_list* head) {
    unsigned int count = 0;
    dd_task_list* current = head;

    while (current != NULL) {
        count++;
        current = current->next_task;
    }

    return count;
}

// Function to print the elements of the linked list
void printList(dd_task_list* head) {
    dd_task_list* temp = head;
    while (temp != NULL) {
        // Assuming task has an integer member named task_id
        printf("%d -> ", temp->task.task_id);
        temp = temp->next_task;
    }
    printf("NULL\n");
}

// Function to free the memory allocated for the linked list
void freeList(dd_task_list** head) {
    dd_task_list* current = *head;
    dd_task_list* next;
    while (current != NULL) {
        next = current->next_task;
        vPortFree(current);
        current = next;
    }
    *head = NULL;
}
