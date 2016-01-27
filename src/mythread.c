#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include"myqueue.h"
#include"thread.h"
#include"mythread.h"
#define DEBUG 1

struct scheduler{
    node* ready_h;
    node* ready_t;
    ucontext_t main_ctx;

} handler;

manager mgr;

uthread* get_running_thread(){
    int x = 0;
    x = (int) peek(handler.ready_h);
    if(x == 0)
        return NULL;
    else    
        return get_thread(&mgr,x);
}

uthread* get_next_ready_thread(){
    int x = 0;
    uthread* t;
    x = (int) peek (handler.ready_h);
    while(x != 0){
        t = get_thread(&mgr,x);
        if(t != NULL && t->state == READY){
            return t;
        }
        dequeue(&(handler.ready_h),&(handler.ready_t));
        x = (int) peek (handler.ready_h);
    }
    return NULL;
}

void print_ready_queue(){
    int x = 0;
    node* h = handler.ready_h;
    printf("---- Ready Queue ----\n");
    while( h != NULL){
        x = (int) h->payload;    
        print_thread(get_thread(&mgr,x));
        printf("---");
        h = h->next;
    }
    printf("----x-x-x-x-x-x----\n");

}

// Create and run the "main" thread
void MyThreadInit(void(*start_funct)(void *), void *args){

    uthread* t;
    // Initialize the manager
    if(DEBUG)
        printf("MYTHREAD: MyThreadInit # Initializing Manager\n");
    init_manager(&mgr);

    if(DEBUG)
        printf("MYTHREAD: MyThreadInit # Creating the root thread\n");
    //Create a new root thread, add it to the manager.
    t = init_thread(&mgr,&(handler.main_ctx),RUNNING,start_funct,args);
    
    if(DEBUG)
        printf("MYTHREAD: MyThreadInit # Root Thread initialized with ID: %d\n",t->id);

    //Initialize the ready queue
    init_queue(&(handler.ready_h),&(handler.ready_t));

    if(DEBUG)
        printf("MYTHREAD: MyThreadInit # Ready Queue Initialized\n");
    
    //Enqueue the new thread id into the ready queue
    enqueue(&(handler.ready_h),&(handler.ready_t),(void*)t->id);

    if(DEBUG){
        print_ready_queue();
        printf("MYTHREAD: MyThreadInit # Root thread added to the ready queue. Swapping Context\n");
    }
    //Perform the context switch 
    swapcontext(&(handler.main_ctx),&(t->context));

    //Clean up all the threads once there are no threads in the ready queue.
    if(DEBUG)
        printf("MYTHREAD: MyThreadInit # Destroying all remaining threads\n");
    cleanup_thread(&mgr);
}

// Create a new thread.
MyThread MyThreadCreate(void(*start_funct)(void *), void *args){

    uthread* running, *new_thread;
    ucontext_t* link_ctx;

    // GEt the invoking thread
    running = get_running_thread();
    if(DEBUG)
            printf("MYTHREAD: MyThreadCreate # Invoking Thread : %d\n",running != NULL ? running->id : 0);
    
    if(running == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadCreate # No Running Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }
    
    // Create the new thread
    link_ctx = &(running->context);
    new_thread = init_thread(&mgr, link_ctx, READY, start_funct, args);
    
    if (DEBUG){
         printf("MYTHREAD: MyThreadCreate # New Thread : %d\n",new_thread != NULL ? new_thread->id : 0);
    }
    
    // Enqueue the thread to the back of ready queue
    enqueue(&(handler.ready_h),&(handler.ready_t),(void*)new_thread->id);

    if(DEBUG){
        //print_ready_queue();
        printf("MYTHREAD: MyThreadCreate # New thread added to the ready queue.\n");
    }
    
    // Enlist the new thread as the child of the running thread;
    enqueue(&(running->child_h),&(running->child_t),(void*)new_thread->id);
    if(DEBUG){
        //print_thread(running);
        printf("MYTHREAD: MyThreadCreate # New thread added to the child queue of the running thread.\n");
    }
    return (MyThread) new_thread->id;

}
// Yield invoking thread
void MyThreadYield(void){


}

void MyThreadExit(void){

    uthread* running;
    uthread* ready;
    uthread* waiting;

    running = get_running_thread();
    if(DEBUG)
        printf("MYTHREAD: MyThreadExit # Invoking Thread : %d\n",running != NULL ? running->id : 0);

    if(running == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadExit # No Running Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }
    
    running->state = TERMINATED;
    waiting = get_thread(&mgr,running->waiting);
    running->waiting = 0;
    if(DEBUG)
        printf("MYTHREAD: MyThreadExit # Waiting Thread : %d\n",waiting != NULL ? waiting->id : 0);
    
    if(waiting != NULL){
        waiting->blocked_count -= 1;
        if (waiting->state == BLOCKED && waiting->blocked_count == 0){
            waiting->state = READY;
            enqueue(&(handler.ready_h),&(handler.ready_t),(void*)waiting->id);
            if(DEBUG){
                print_thread(waiting);
                print_ready_queue();
                printf("MYTHREAD: MyThreadExit # Requeued Waiting Thread : %d back to ready queue\n",waiting->id);
            }
        }
    }
    ready = get_next_ready_thread();

    if(DEBUG)
        printf("MYTHREAD: MyThreadExit # Next Ready Thread : %d\n",ready != NULL ? ready->id : 0);    

    if(ready == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadExit # No Ready Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }
    
    delete_thread(&mgr,running->id);
    ready->state=RUNNING;    
    setcontext(&(ready->context));
}
