#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include"myqueue.h"
#include"thread.h"

/*
void init_thread(thread *thr, thread* parent,char state);
void destroy_thread(thread* thr);
*/

void init_thread(thread *thr, ucontext_t* parent,char state, void(*func)(void *),void* args){

    
    char *stack= (char*)malloc(STACK_SIZE);
    
    if(thr == NULL)
        thr = (thread*)malloc(sizeof(thread));

    thr->blocked_count=0;

    thr->state = state;

    init_queue(&(thr->child_h),&(thr->child_t));
    init_queue(&(thr->blocked_h),&(thr->blocked_t));

    thr->context = (ucontext_t*)malloc(sizeof(ucontext_t*));
    getcontext(thr->context);

    thr->context->uc_stack.ss_sp = stack;
    thr->context->uc_stack.ss_size = STACK_SIZE;
    thr->context->uc_link = parent;
    
    makecontext(thr->context,(void (*)(void))func,1,args);

}

void destroy_thread(thread* thr){

    if(thr == NULL)
        return;
    thread *temp;
    free(thr->context->uc_stack.ss_sp);
    free(thr->context);
    temp=(thread*) peek(thr->child_h);
    while(temp!= NULL){
        temp=dequeue(&(thr->child_h),&(thr->child_t));
    }
    temp=(thread*) peek(thr->blocked_h);
    while(temp!= NULL){
        temp=dequeue(&(thr->blocked_h),&(thr->blocked_t));
    }

    free(thr);
}
