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

// Function to Delete Node from task given a pointer to the node
void deleteTask(dd_task_list** headRef, dd_task_list** nodeToDeleteRef) {
    if (*headRef == NULL || *nodeToDeleteRef == NULL)
        return;

    if (*headRef == *nodeToDeleteRef) {
        *headRef = (*headRef)->next_task;
        vPortFree(*nodeToDeleteRef);
        *nodeToDeleteRef = NULL;
        return;
    }

    dd_task_list* current = *headRef;
    while (current->next_task != *nodeToDeleteRef) {
        current = current->next_task;
        if (current == NULL)
            return;
    }

    current->next_task = (*nodeToDeleteRef)->next_task;
    vPortFree(*nodeToDeleteRef);
    *nodeToDeleteRef = NULL;
}

// Implementation of the countItems function
uint32_t countItems(dd_task_list* head) {
    uint32_t count = 0;
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
