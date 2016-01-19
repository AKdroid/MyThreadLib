#include<stdio.h>
#include<stdlib.h>
#include"myqueue.h"
#define DEBUG 1
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
}

void enqueue(node ** head, node ** tail, void* t){
    node* new_tail =(node*) malloc(sizeof(node));
    new_tail->payload = t;
    new_tail->next = NULL;
    if(*head == NULL){
        *head = new_tail;
    }else{
        (*tail)->next = new_tail;
    }
    *tail = new_tail;
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
