#ifndef THREAD_H
#define THREAD_H
#define THREAD_READY 'R'
#define THREAD_TERMINATED 'T'
#define THREAD_BLOCKED 'B'
#define THREAD_WAITING 'W'
#define THREAD_RUNNING 'X'
#define STACK_SIZE 32768
#include<ucontext.h>
#include"myqueue.h"

struct my_thread{
    ucontext_t context;
    char state;
    node* child_h;
    node* child_t;
    node* blocked_t;
    node* blocked_h;   
    int blocked_count; 
};

struct thread_handler{
    ucontext_t* root_context;
    struct my_thread* root_thread;
    node* ready_h;
    node* ready_t;
};

typedef struct my_thread thread;
typedef struct thread_handler thread_handler;

void init_thread(thread *thr, ucontext_t* parent,char state, void(*func)(void *),void* args);
void destroy_thread(thread** thr);

#endif
