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

dd_task_list* swap(dd_task_list* ptr1, dd_task_list* ptr2){
    dd_task_list* temp_list = ptr2->next_task;
    ptr2->next_task = ptr1;
    ptr1->next_task = temp_list;
    return ptr2;
}

void sortList(dd_task_list** head){
    dd_task_list** other_head;

    unsigned int i, j, swapped;

    unsigned int count = countItems(*head);

    for(i = 0; i <= count; i++){
        other_head = head;
        swapped = 0;

        for(j = 0; j < count - i - 1; j++){
            dd_task_list* p1 = *other_head;
            dd_task_list* p2 = p1->next_task;

            if(p1->task.absolute_deadline > p2->task.absolute_deadline){
                *other_head = swap(p1, p2);
                swapped = 1;
            }

            other_head = &(*other_head)->next_task;
        }

        if(swapped == 0) break;
    }
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
