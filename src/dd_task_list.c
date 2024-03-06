#include "dd_task_list.h"

// Function to create a new node with given task
struct dd_task_list* createNode(struct dd_task newTask) {
    struct dd_task_list* newNode = (struct dd_task_list*)pvPortMalloc(sizeof(struct dd_task_list));
    if (newNode == NULL) {
        // Handle memory allocation failure
    }
    newNode->task = newTask;
    newNode->next_task = NULL;
    return newNode;
}

// Function to insert a new node at the beginning of the linked list
void insertAtBeginning(struct dd_task_list** head, struct dd_task newTask) {
    struct dd_task_list* newNode = createNode(newTask);
    newNode->next_task = *head;
    *head = newNode;
}

// Function to insert a new node at the end of the linked list
void insertAtEnd(struct dd_task_list** head, struct dd_task newTask) {
    struct dd_task_list* newNode = createNode(newTask);
    if (*head == NULL) {
        *head = newNode;
        return;
    }
    struct dd_task_list* temp = *head;
    while (temp->next_task != NULL) {
        temp = temp->next_task;
    }
    temp->next_task = newNode;
}

// Function to print the elements of the linked list
void printList(struct dd_task_list* head) {
    struct dd_task_list* temp = head;
    while (temp != NULL) {
        // Assuming task has an integer member named task_id
        printf("%d -> ", temp->task.task_id);
        temp = temp->next_task;
    }
    printf("NULL\n");
}

// Function to free the memory allocated for the linked list
void freeList(struct dd_task_list** head) {
    struct dd_task_list* current = *head;
    struct dd_task_list* next;
    while (current != NULL) {
        next = current->next_task;
        vPortFree(current);
        current = next;
    }
    *head = NULL;
}