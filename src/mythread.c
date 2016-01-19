#include<ucontext.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include"myqueue.h"
#include"thread.h"
#include"mythread.h"
/*
MyThread MyThreadCreate(void(*start_funct)(void *), void *args);

// Yield invoking thread
void MyThreadYield(void);

// Join with a child thread
int MyThreadJoin(MyThread thread);

// Join with all children
void MyThreadJoinAll(void);

// Terminate invoking thread
void MyThreadExit(void);

void MyThreadInit(void(*start_funct)(void *), void *args);
*/

thread_handler handler;

thread * get_next_ready_thread(void){
    thread* next= peek(handler.ready_h);
    while(next != NULL){
        if(next->state == THREAD_READY)
            return next;
        else if(next->state == THREAD_TERMINATED){
            destroy_thread(next);   
        }
        dequeue(&(handler.ready_h),&(handler.ready_t));
        next= peek(handler.ready_h);
    }
    return NULL;
}

void MyThreadInit(void(*start_funct)(void *), void *args){

    node* temp;
    init_queue(&(handler.ready_h),&(handler.ready_t));
    handler.root_context=(ucontext_t*)malloc(sizeof(ucontext_t));
    init_thread(&(handler.root_thread),handler.root_context, THREAD_RUNNING,
                    start_funct, args);
    enqueue(&(handler.ready_h),&(handler.ready_t),&(handler.root_thread));
    //printf("peek=%u %u\n",peek(handler.ready_h),&(handler.root_thread));
    swapcontext(handler.root_context,handler.root_thread.context);
    // joinall on root_thread should be called
    
}

MyThread MyThreadCreate(void(*start_funct)(void *), void *args){
    char state=THREAD_RUNNING;
    thread *parent_thread;
    ucontext_t * parent_context;
    thread *thr = (thread*) malloc(sizeof(thread));
    parent_context = handler.root_context;
    parent_thread = peek(handler.ready_h);
    
    if (parent_thread != NULL){
        parent_context = parent_thread->context;
        state = THREAD_READY;
    }
    init_thread( thr, parent_context, state, start_funct, args);
    enqueue(&(handler.ready_h),&(handler.ready_t),thr);
    enqueue(&(parent_thread->child_h),&(parent_thread->child_t),thr);
    return (MyThread) thr;
}

void MyThreadYield(void){

    thread* next_thread;    
    thread* running = (thread*) dequeue(&(handler.ready_h),&(handler.ready_t));
    
    if(running == NULL){
        printf("MyThreadYield called from a non-thread context\n");
        return;
    }
    
    next_thread = get_next_ready_thread();
    
    enqueue(&(handler.ready_h),&(handler.ready_t),running);

    if(next_thread != NULL){
        running->state = THREAD_READY;
        next_thread->state = THREAD_RUNNING;
        swapcontext(running->context,next_thread->context);
    }
    
}

int MyThreadJoin(MyThread thr){
    
    thread* ready;
    thread* target = (thread*) thr;
    thread* running = (thread* )peek(handler.ready_h);
        
    if(running == NULL){
        printf("MyThreadJoin called from a non-thread context\n");
        return;
    }

    if(target == NULL)
        return 0;
    
    if (!exists(handler.ready_h, thr))    {
        printf("target thread not an immediate child of the invoking thread\n");
        return -1;
    }
    
    if(target->state == THREAD_TERMINATED){
        destroy_thread(target);
        return 0;
    }
    dequeue(&(handler.ready_h),&(handler.ready_t)); 
    ready = get_next_ready_thread();
    enqueue(&(target->blocked_h),&(target->blocked_t),running);
    target->blocked_count += 1;
    
    ready->state = THREAD_RUNNING;
    running->state = THREAD_BLOCKED;

    swapcontext(running->context,ready->context);
        
    return 0;

}

void MyThreadJoinAll(void){
    
    thread* running;
    node* child;
    running = (thread*) peek(handler.ready_h);


    if(running == NULL){
        printf("MyThreadJoinAll called from a non-thread context\n");
        return;
    }

    child = running->child_h;
    while(child != NULL){
        MyThreadJoin((MyThread)child->payload);
        child = child->next;
    }
}

void MyThreadExit(void){
    thread* ready;
    thread* running;
    thread* blocked;
    running = (thread*) peek(handler.ready_h);

    printf("Exiting Thread\n");

    if(running == NULL){
        printf("MyThreadExit called from a non-thread context\n");
        return;
    }

    
    running->state = THREAD_TERMINATED;    
    blocked = (thread*) peek(running->blocked_h);
    
    while(blocked != NULL) {
        blocked->state = THREAD_READY;
        enqueue(&(handler.ready_h),&(handler.ready_t),blocked);
        running->blocked_count -= 1;    
        dequeue(&(running->blocked_h),&(running->blocked_t));
        blocked = (thread*) peek(running->blocked_h);
    }

    dequeue(&(handler.ready_h),&(handler.ready_t));    
    ready = get_next_ready_thread();
    if(ready == NULL)
        return;
    ready->state = THREAD_RUNNING;
    setcontext(ready->context);
    
}
