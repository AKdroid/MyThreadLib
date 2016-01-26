#ifndef THREAD_H
#define THREAD_H
#include<ucontext.h>
#include"myqueue.h"
#define DEBUG_T 1
#define READY 'R'
#define RUNNING 'X'
#define BLOCKED 'B'
#define ZOMBIE 'Z'
#define TERMINATED 'T'

struct uthread {
    char state; // state of the thread
    ucontext_t context; // context of the thread
    struct uthread* waiting; //parent thread if blocked else null
    struct node* child_h; // head of the child queue/list 
    struct node* child_t; // tail of the child queue/list
    int blocked_count;
};

typedef struct uthread uthread;

void init_thread(uthread** newthr, ucontext_t* parent, char state, void(*func)(void*), void* args);
void destroy_thread(uthread** thr);
void print_uthread(uthread* thr);

#endif /* THREAD_H */
