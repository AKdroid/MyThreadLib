#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include"myqueue.h"
#include"thread.h"
#include"mysemaphore.h"
#include"mythread.h"

#define DEBUG 0



struct scheduler{
    node* ready_h;
    node* ready_t;
    ucontext_t main_ctx;

} handler;

manager mgr;
manager smgr;

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
    node* temp;
    // Initialize the manager
    if(DEBUG)
        printf("MYTHREAD: MyThreadInit # Initializing Manager\n");
    init_manager(&mgr);
    init_smanager(&smgr);
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

    do{
        temp = dequeue(&(handler.ready_h),&(handler.ready_t));
    }
    while(temp != NULL);
    
    cleanup_semaphores(&smgr);
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

    uthread* running;
    uthread* ready;
    uthread* waiting;

    running = get_running_thread();
    if(DEBUG)
        printf("MYTHREAD: MyThreadYield # Invoking Thread : %d\n",running != NULL ? running->id : 0);

    if(running == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadYield # No Running Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }

    
    ready = get_next_ready_thread();

    if(DEBUG)
        printf("MYTHREAD: MyThreadYield # Next Ready Thread : %d\n",ready != NULL ? ready->id : 0);

    enqueue(&(handler.ready_h),&(handler.ready_t),(void*)running->id);

    if(ready == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadYield # No thread in ready queue. Resuming the invoking thread\n");
        return;
    }
    
    ready->state = RUNNING;
    running->state = READY;

    swapcontext(&(running->context),&(ready->context));
    
    if(DEBUG)
        printf("MYTHREAD: MyThreadYield # Swapcontext returned to thread: %d\n",running->id);

}

// Join with a child thread
int MyThreadJoin(MyThread thread){

    uthread* running;
    uthread* ready;
    uthread* target;

    running = get_running_thread();
    if(DEBUG)
        printf("MYTHREAD: MyThreadJoin # Invoking Thread : %d\n",running != NULL ? running->id : 0);

    if(running == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadJoin # No Running Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }

    if(! exists(running->child_h,thread)){
        if(DEBUG){
            printf("MYTHREAD: MyThreadJoin # Existence test failed for child node target: %d for running %d.\n",running->id,(int)thread);    
            print_thread(running);
        }
        return -1;
    }

    target = get_thread(&mgr,(int)thread);
    if(DEBUG)
        printf("MYTHREAD: MyThreadJoin # Target Thread : %d\n",target != NULL ? target->id : 0);

    if(target == NULL){
        if(DEBUG){
            printf("MYTHREAD: MyThreadJoin # Existence passed. The child has already terminated: Not Blocking.\n");
        }
        return 0;
    }

    ready = get_next_ready_thread();

    if(DEBUG)
        printf("MYTHREAD: MyThreadJoin # Next Ready Thread : %d\n",ready != NULL ? ready->id : 0);

    if(ready == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadJoin # No thread in ready queue. Exiting the threading framework\n");
        setcontext(&(handler.main_ctx));
    }
    
    running->state = BLOCKED;
    
    ready->waiting = running->id;
    running->blocked_count += 1;
    ready->state = RUNNING;

    swapcontext(&(running->context),&(ready->context));

    if(DEBUG)
        printf("MYTHREAD: MyThreadJoin # Swapcontext returned after join to the running thread : %d\n",running->id);

    return 0;
    
}

// Join with all children
void MyThreadJoinAll(void){

    uthread* running;
    node* child;    

    running = get_running_thread();
    if(DEBUG)
        printf("MYTHREAD: MyThreadJoinAll # Invoking Thread : %d\n",running != NULL ? running->id : 0);

    if(running == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MyThreadJoinAll # No Running Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }
    
    child = running->child_h;
    
    while(child != NULL){
        MyThreadJoin(child->payload);
        child = child->next;
    }
  
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

// ****** SEMAPHORE OPERATIONS ****** 
// Create a semaphore
MySemaphore MySemaphoreInit(int initialValue){

    //create a new semaphore
    usemaphore*  p;
    usemaphore* q;
    if(initialValue < 0)
        return NULL;

    p = (usemaphore*)malloc(sizeof(usemaphore));

    init_queue(&(p->wait_h),&(p->wait_t));

    p->max_value = initialValue;
    p->status = initialValue;
    p->id = get_next_sid(&smgr);
    if(DEBUG)
        printf("MYTHREAD: MySemaphoreInit # Created New Semaphore | id : %d | status : %d|\n",p->id,p->status);
    insert_semaphore(&smgr,p);
    if(DEBUG){
        q = get_semaphore(&smgr,p->id);
        if( q==p)
            printf("Success\n");
        else
            printf("Failure\n");
        
        printf("MYTHREAD: MySemaphoreInit # Created New Semaphore | id : %d | status : %d|\n",p->id,p->status);
    }
    return (MySemaphore) p->id;

}

// Signal a semaphore
void MySemaphoreSignal(MySemaphore sem){
    int x;
    usemaphore *p;
    uthread* t;
    if(sem == NULL)
        return;

    p = get_semaphore(&smgr, (int)sem);
    
    if(DEBUG)
        printf("MYTHREAD: MySemaphoreSignal # Signalled a semaphore | id : %d|\n",(int)sem);

    if(p == NULL)
        return NULL;
    
    if(DEBUG)
        printf("MYTHREAD: MySemaphoreSignal # Signalled a semaphore | id : %d | status : %d|\n",p->id,p->status);
    
    p->status += 1;
    
    x = (int) peek(p->wait_h);

    if(x == 0){
        return;
    }

    dequeue(&(p->wait_h),&(p->wait_t));

    t = get_thread(&mgr,x);

    if(t == NULL)
        return;

    t->blocked_count -= 1;

    if(t->blocked_count == 0){
        t->state = READY;        
        enqueue(&(handler.ready_h),&(handler.ready_t),(void*)x);
    }        

}

// Wait on a semaphore
void MySemaphoreWait(MySemaphore sem){
    int x;
    usemaphore *p;
    uthread* running;
    uthread* ready;
    if(sem == NULL)
        return;
    p = get_semaphore(&smgr,(int)sem);
    
    if(DEBUG)
        printf("MYTHREAD: MySemaphoreWait # Waiting on a semaphore | id : %d | status : %d|\n",p->id,p->status);

    if(p == NULL)
        return;

    p->status -= 1;
    if(p->status > 0){
        return; 
    }

    running = get_running_thread();

    if(DEBUG)
        printf("MYTHREAD: MySemaphoreWait # Invoking Thread : %d\n",running != NULL ? running->id : 0);

    if(running == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MySemaphoreWait # No Running Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }

    ready = get_next_ready_thread();

    if(DEBUG)
        printf("MYTHREAD: MySemaphoreWait # Next Ready Thread : %d\n",ready != NULL ? ready->id : 0);

    if(ready == NULL) {
        if(DEBUG)
            printf("MYTHREAD: MySemaphoreWait # No Ready Thread. Ready Queue Empty. Exiting Framework.\n");
        setcontext(&(handler.main_ctx));
    }

    running->state = BLOCKED;
    running->blocked_count+=1;
    enqueue(&(p->wait_h),&(p->wait_t),(void*)running->id);
    
    ready->state = RUNNING;
    swapcontext(&(running->context),&(ready->context));

}

// Destroy on a semaphore
int MySemaphoreDestroy(MySemaphore sem){
    usemaphore* temp;
    int x;
    if(sem == NULL)
        return;
    temp = get_semaphore(&smgr,(int)sem);
    if(DEBUG)
        printf("MYTHREAD: MySemaphoreDestroy # Destroying a semaphore | id : %d | status : %d|\n",temp->id,temp->status);
    if(temp->wait_h != NULL)
        return -1;    
    delete_semaphore(&smgr,temp);
    return 0;

}

