#include<ucontext.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#define THREAD_READY 'R'
#define THREAD_EXITED 'E'
#define THREAD_BLOCKED 'B'
#define THREAD_WAITING 'W'

struct fifo_node{
    struct fifo_node* next;
    void* payload;
};

struct my_thread{
    ucontext_t * context;
    char state;
    struct fifo_node* head;
    struct fifo_node* tail;
};

struct thread_handler{
    struct fifo_node* ready_head;
    struct fifo_node* ready_tail;
};

typedef struct my_thread MyThread;
typedef struct fifo_node node;

/**
 * Helper Functions for intializing, enqueuing, dequeuing and peeking the MyThread Queue
*/
void init_queue(node** head, node** tail){
    *head = NULL;
    *tail = NULL;
}

void enqueue_mythread(node ** head, node ** tail, MyThread* t){
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

MyThread* dequeue_mythread(node** head, node** tail){
    node* top = *head;
    if(top == NULL)
        return NULL;
    *head = top->next;
    if(*head == NULL)
        *tail = NULL;
    return (MyThread *)top->payload;
}

MyThread* peek_mythread(node* head){
    if(head == NULL)
        return NULL;
    return (MyThread*) head->payload;
}


