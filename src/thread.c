#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include"myqueue.h"
#include"thread.h"
#define STACK_SIZE 8192
/*

struct uthread {
    char state; // state of the thread
    ucontext_t context; // context of the thread
    struct uthread* waiting; //parent thread if blocked else null
    struct node* child_h; // head of the child queue/list 
    struct node* child_t; // tail of the child queue/list
    int blocked_count;
};

typedef struct uthread uthread;
*/

void init_thread(uthread** uthr, ucontext_t* parent, char state, void(*func)(void*), void* args){

    // Initializes the components of a thread
    //if thr is NULL, allocate memory for thread
    char* stack;
    uthread* thr = *uthr;

    if(thr == NULL){
        thr = (uthread*) malloc (sizeof(uthread));
        if (thr == NULL){
            printf("THREAD: Init_thread # Malloc Failed for new thread allocation\n");
            return;
        }        
        if(DEBUG_T){
            printf("THREAD: Init_thread # New thread memory allocation successful\n");
            printf("THREAD: Init_thread # Malloc Successful with new thread at: %llu with size %d\n",thr,sizeof(uthread));
        }   
    }
   
    //Initialize the stack
    stack = (char*) malloc (STACK_SIZE);
    if(stack == NULL){
        printf("THREAD: Init_thread # Malloc Failed for stack allocation\n");
        return;
    }
    if(DEBUG_T){
            printf("THREAD: Init_thread # New Stack allocation successful with address %llu and size %d\n",stack,STACK_SIZE);
    }
    // Initialize the context of the new thread
    getcontext(&(thr->context));
    thr->context.uc_link = parent;
    thr->context.uc_stack.ss_sp = stack;   
    thr->context.uc_stack.ss_size = STACK_SIZE;
    thr->context.uc_stack.ss_flags = 0;
    makecontext(&(thr->context),(void(*)(void))func,1,args);
    if(DEBUG_T){
        printf("THREAD: Init_thread # Make Context Successful for thread : %llu\n",thr);
    }    
    // Set the state of the new thread
    thr->state = state;
    
    //Initialize the child queue of the new thread
    init_queue(&(thr->child_h),&(thr->child_t));
    
    //Initialize the join thread wait pointer
    thr->waiting = NULL;

    //Initialized the blocked count to 0
    thr->blocked_count = 0;
    if(DEBUG_T){
        printf("THREAD: Init_thread # Thread Initialization Complete for thread: %llu\n",thr);
    }
    *uthr = thr; 
}

void destroy_thread(uthread** thr){
    
    // Destroys the thread and frees the stack resources recursively
    uthread* t;
    if(thr == NULL)
        return;
    
    t = *thr;
    t->state = TERMINATED; //Set the state to terminated
    free(t->context.uc_stack.ss_sp); // Free the stack
    if(DEBUG_T){
        printf("THREAD: Destroy_thread # Stack for the thread at %llu destroyed\n",thr);
    }
    if(t->waiting != NULL && t->waiting->state == ZOMBIE)    {
        if(DEBUG_T)
            printf("THREAD: Destroy_thread # Clearing the zombie thread %llu\n",t->waiting);
        destroy_thread(&(t->waiting)); // Destroy the parent zombie thread
    }
    if(t->blocked_count == 0){
        if(DEBUG_T)
            printf("THREAD: Destroy_thread # Freeing the resources used for the thread %llu\n",thr);
        free(t);      //Free the thread memory
        *thr = NULL;  //Set the pointer to thread pointer as NULL
    }
}

void print_thread(uthread* thread){
    if(thread == NULL){
        printf("NULL Thread\n");
        return;
    }
    printf("Thread : %llu\n ",thread);
    printf("State : %c\n",thread->state);
    printf("Context : %llu\n",&(thread->context));
    printf("Waiting : %llu\n",thread->waiting);
    printf("Blocked : %d\n",thread->blocked_count);
    printf("Children: \n");
    node* h=thread->child_h;
    while(h!= NULL){
        printf("%llu ",*(uthread**)(h->payload));
        h=h->next;
    }
    printf("\n");
}
