#include<stdio.h>
#include<stdlib.h>
#include"myqueue.h"
#define DEBUG_Q 0
/*
void init_queue(node** head, node** tail);
void enqueue(node** head, node** tail, void* payload);
void* dequeue(node** head, node** tail);
void* peek(node* head);
int exists(node* head, void* query);
*/

void init_queue(node** head, node** tail){
    *head = NULL;
    *tail = NULL;
    if(DEBUG_Q){
        printf("Queue: init_queue Initial head and tail: %llu %llu %llu %llu \n",head,tail,*head,*tail);
    }

}

void enqueue(node ** head, node ** tail, void* t){
    node* new_tail =(node*) malloc(sizeof(node));
    if(DEBUG_Q){
        printf("Queue: Enqueue Initial head and tail: %llu %llu %llu %llu\n",head,tail,*head,*tail);
    }
    if(new_tail == NULL){
        printf("Memory Allocation Error\n");
    }
    if(DEBUG_Q){
        printf("Queue: Enqueue New_tail: %llu\n",new_tail);
    }
    new_tail->payload = t;
    new_tail->next = NULL;
    if(*head == NULL){
        *head = new_tail;
    }else{
        (*tail)->next = new_tail;
    }
    *tail = new_tail;
    if(DEBUG_Q){
        printf("Queue: Enqueue head and tail: %llu %llu %llu %llu \n",head,tail,*head,*tail);
    }
}

void* dequeue(node** head, node** tail){
    node* top = *head;
    void* payload;
    if(top == NULL)
        return NULL;
    *head = top->next;
    payload = top->payload;
    free(top);
    if(*head == NULL)
        *tail = NULL;
    if(DEBUG_Q){
        printf("Queue: Dequeue head and tail: %llu %llu %llu %llu \n",head,tail,*head,*tail);
    }
    return payload;
}

void* peek(node* head){
    if(head == NULL)
        return NULL;
    return head->payload;
}

int exists(node* head, void* query){
    node* temp=head;
    while(temp!=NULL){
        if (temp->payload == query)
            return 1;
        temp=temp->next;
    }   
    return 0;
}
