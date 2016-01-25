#include<ucontext.h>
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include"myqueue.h"
#include"thread.h"
#include"mythread.h"
#define DEBUG 1
#define _XOPEN_SOURCE 700
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

/*
    TODO: thread* next= *(thread**) peek(handler.ready_h);
                |
                | 
                V
    thread* next
    thread **temp = (thread**) peek(handler.ready_h);
    next = temp != NULL ? *temp : NULL;
*/

thread_handler handler;

thread * get_next_ready_thread(void){
    thread **temp;
    thread* next;
    temp= (thread**) peek(handler.ready_h);
    next = temp != NULL ? *temp : NULL;
    while(next != NULL){
        if(next->state == THREAD_READY)
            return next;
        dequeue(&(handler.ready_h),&(handler.ready_t));
        temp= (thread**)peek(handler.ready_h);
        next = temp != NULL ? *temp : NULL;
    }
    return NULL;
}

void print_ready_queue(){
    node* n = handler.ready_h;
    thread * thr;
    printf("##########Ready#Queue#########\n");
    while(n != NULL){
        printf("n->payload %llu\n",n->payload);
        if (n-> payload != NULL){
            thr = *(thread**)n->payload;
            printf("%llu %llu %c %llu %llu %llu %llu \n",n,&(thr->context),thr->state,thr->child_h,thr->child_t,thr->blocked_h,thr->blocked_t);
        }
        n=n->next; 
    }
    printf("##########X#X#X#X#X#X#######\n");
}

void MyThreadInit(void(*start_funct)(void *), void *args){
    printf("%d",sizeof(thread));
    init_queue(&(handler.ready_h),&(handler.ready_t));
    handler.root_thread=(thread *)malloc(sizeof(thread));
    handler.root_context=(ucontext_t*)malloc(sizeof(ucontext_t));
    getcontext(handler.root_context);
    init_thread(handler.root_thread,handler.root_context, THREAD_RUNNING,
                    start_funct, args);
    enqueue(&(handler.ready_h),&(handler.ready_t),&(handler.root_thread));
    //printf("peek=%u %u\n",peek(handler.ready_h),&(handler.root_thread));
    print_ready_queue();
    swapcontext(handler.root_context,&(handler.root_thread->context));
    // joinall on root_thread should be called
   printf("Out of the thread framework\n"); 
}

MyThread MyThreadCreate(void(*start_funct)(void *), void *args){
    char state=THREAD_RUNNING;
    thread** temp;
    thread *parent_thread;
    ucontext_t * parent_context;
    thread *thr = (thread*) malloc(sizeof(thread));
    parent_context = handler.root_context;
    temp = (thread**) peek(handler.ready_h);
    parent_thread = temp != NULL ? *temp : NULL;
    
    if (parent_thread != NULL){
        parent_context = &(parent_thread->context);
        state = THREAD_READY;
        if(DEBUG){
        printf("Non NULL parent_thread \n");
        }
    } else {
        printf("Creation of Thread invoked from uninitilized thread framework\n");
        return NULL;
    }
    if(DEBUG)
        printf("parent_thread  : %llu  %llu %llu %llu \n",&(parent_thread->child_h),&(parent_thread->child_t),(parent_thread->child_h),(parent_thread->child_t));
    init_thread( thr, parent_context, state, start_funct, args);
    if(DEBUG){
        printf("Enqueuing new thread into thee back of ready queue\n");
    }
    enqueue(&(handler.ready_h),&(handler.ready_t),&thr);
    if(DEBUG){
        printf("Enqueuing new thread into the back of child queue\n");
    }
    enqueue(&(parent_thread->child_h),&(parent_thread->child_t),&thr);
    if(DEBUG){
        printf("Enqueue Complete\n");
    }
    print_ready_queue();
    return (MyThread) thr;
}

void MyThreadYield(void){
    thread **temp;
    thread* next_thread;    
    thread* running;
    int res;

    temp = (thread**) dequeue(&(handler.ready_h),&(handler.ready_t));
    running = temp != NULL ? *temp : NULL;  
 
    if(running == NULL){
        printf("MyThreadYield called from a non-thread context\n");
        return;
    }
    
    next_thread = get_next_ready_thread();
   
    enqueue(&(handler.ready_h),&(handler.ready_t),&running);
    if(DEBUG)
        printf("Thread Yield next_thread: %llu \n",next_thread);

    if(next_thread != NULL){
        running->state = THREAD_READY;
        next_thread->state = THREAD_RUNNING;
        if(DEBUG)
            printf("next_thread  : %llu  %llu %llu %llu \n",&(next_thread->child_h),&(next_thread->child_t),(next_thread->child_h),(next_thread->child_t));
        print_ready_queue();
        printf("%llu %llu\n",&(running->context),&(next_thread->context));
        res = swapcontext(&(running->context),&(next_thread->context));
        print_ready_queue();
        printf("Result of swapcontext :%d \n",res);
    }
    
}

int MyThreadJoin(MyThread thr){
    
    thread** temp; 
    thread* ready;
    thread* running;
    thread* target = (thread*) thr;
    temp = (thread**)peek(handler.ready_h);
    running = temp != NULL ? *temp : NULL;
        
    if(running == NULL){
        printf("MyThreadJoin called from a non-thread context\n");
        return;
    }

    if(target == NULL)
        return 0;
    
    if (!exists(handler.ready_h, &thr))    {
        printf("Target thread not an immediate child of the invoking thread\n");
        return -1;
    }
    
    if(target->state == THREAD_TERMINATED){
        return 0;
    }

    dequeue(&(handler.ready_h),&(handler.ready_t)); 
    ready = get_next_ready_thread();
    enqueue(&(target->blocked_h),&(target->blocked_t),&running);
    target->blocked_count += 1;
    
    ready->state = THREAD_RUNNING;
    running->state = THREAD_BLOCKED;

    swapcontext(&(running->context),&(ready->context));
    return 0;

}

void MyThreadJoinAll(void){
    thread** temp;
    thread* running;
    thread* x;
    node* child;
    
    temp = (thread**) peek(handler.ready_h);
    running = temp != NULL ? *temp : NULL;


    if(running == NULL){
        printf("MyThreadJoinAll called from a non-thread context\n");
        return;
    }

    child = running->child_h;
    while(child != NULL){

        temp = (thread**)(child->payload);
        x = temp != NULL ? *temp : NULL;
        MyThreadJoin( (MyThread) x );
        child = child->next;

    }
}

void MyThreadExit(void){
    thread** temp;
    thread* ready;
    thread* running;
    thread* blocked;
   
    print_ready_queue();
 
    temp = (thread**) peek(handler.ready_h);
    running = temp != NULL ? *temp : NULL;

    if(running == NULL){
        printf("MyThreadExit called from a non-thread context\n");
        return;
    }

    if(DEBUG)
        printf("running  : %llu  %llu %llu %llu \n",&(running->child_h),&(running->child_t),(running->child_h),(running->child_t));
    
    running->state = THREAD_TERMINATED;    
    temp = (thread**) peek(running->blocked_h);
    blocked = temp != NULL ? *temp : NULL;
    
    while(blocked != NULL) {
        blocked->state = THREAD_READY;
        enqueue(&(handler.ready_h),&(handler.ready_t),blocked);
        running->blocked_count -= 1;    
        dequeue(&(running->blocked_h),&(running->blocked_t));
        temp = (thread**) peek(running->blocked_h);
        blocked = temp != NULL ? *temp : NULL; 
    }

    dequeue(&(handler.ready_h),&(handler.ready_t));    
    ready = get_next_ready_thread();
    if(ready == NULL)
        return;
    ready->state = THREAD_RUNNING;
    destroy_thread(&running);
    print_ready_queue();
    setcontext(&(ready->context));
    
}
