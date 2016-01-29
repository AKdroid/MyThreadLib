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
/*
uthread* get_thread(manager* mgr,int id);
int get_next_id(manager* mgr);
*/
void init_manager(manager *mgr){
    int i;
    for(i=0;i<BUCKETS;i++){
        mgr->map[i] = NULL;
    }
    mgr->serial = 1;
}

void insert_thread(manager* mgr,uthread* thread){
    lnode* head, *new_node;
    int id;
    id = thread->id;
    head = mgr->map[id%BUCKETS];
    new_node = (lnode*) malloc(sizeof(lnode)); 
    new_node->payload = thread;
    new_node->next = head;
    mgr->map[id%BUCKETS] = new_node;
}

void delete_thread(manager* mgr, int id){
    lnode* head=NULL; 
    lnode* x=NULL;
    lnode* y=NULL;
    uthread* t;
    head = mgr->map[id%BUCKETS];
    x = head;
    while(x != NULL){
        if(x->payload != NULL && x->payload->id == id)
            break;
        y=x;
        x=x->next;
    }
    if(x == NULL)
        return;
    if(x == head){
        mgr->map[id%BUCKETS] = head->next;
    }else{
        y->next=x->next;
    }
    x->next = NULL;
    t=x->payload;
    x->payload = NULL;
    destroy_thread(t);
    free(x);
}

uthread* get_thread(manager* mgr,int id){
    uthread* result = NULL;
    lnode* x=NULL;
    lnode* head = mgr->map[id%BUCKETS];
    x = head;
    while(x!=NULL){
        if(x->payload != NULL && x->payload->id == id){
            result = x->payload;
            break;
        }
        x = x->next;
    }
    return result;
}

int get_next_id(manager* mgr){
    int id = mgr->serial;
    mgr->serial+=1;
    return id;
}

void cleanup_thread(manager* mgr){
    int i;
    node* x,*del;
    uthread* t;
    for(i=0; i< BUCKETS; i++){
        x = mgr->map[i];
        while(x!=NULL){
            del = x;
            x = x->next;
            t = del->payload;
            del->next = NULL;
            destroy_thread(t);
            free(del);
        }
        mgr->map[i]=NULL;
    }
}

uthread* init_thread(manager* mgr, ucontext_t* parent, char state, void(*func)(void*), void* args){

    // Initializes the components of a thread
    //if thr is NULL, allocate memory for thread
    char* stack=NULL;
    uthread* thr = NULL;

    thr = (uthread*) malloc (sizeof(uthread));
    if (thr == NULL){
        printf("THREAD: Init_thread # Malloc Failed for new thread allocation\n");
        return NULL;
    }        
    if(DEBUG_T){
            printf("THREAD: Init_thread # New thread memory allocation successful\n");
            printf("THREAD: Init_thread # Malloc Successful with new thread at: %llu with size %d\n",thr,sizeof(uthread));
    }   
    
    //Initialize the stack
    thr->id = get_next_id(mgr);
    stack = (char*) malloc (STACK_SIZE);
    if(stack == NULL){
        printf("THREAD: Init_thread # Malloc Failed for stack allocation\n");
        return NULL;
    }
    if(DEBUG_T){
            printf("THREAD: Init_thread # New Stack allocation successful with address %llu and size %d\n",stack,STACK_SIZE);
    }
    // Initialize the context of the new thread
    thr->stack = stack;
    getcontext(&(thr->context));
    thr->context.uc_link = parent;
    thr->context.uc_stack.ss_sp = stack;   
    thr->context.uc_stack.ss_size = STACK_SIZE;
    thr->context.uc_stack.ss_flags = 0;
    if(func != NULL)    
        makecontext(&(thr->context),(void(*)(void))func,1,args);

    if(DEBUG_T){
        printf("THREAD: Init_thread # Make Context Successful for thread : %llu\n",thr);
    }    
    // Set the state of the new thread
    thr->state = state;
    
    //Initialize the child queue of the new thread
    init_queue(&(thr->child_h),&(thr->child_t));
    
    //Initialize the join thread wait pointer
    thr->waiting = 0;

    //Initialized the blocked count to 0
    thr->blocked_count = 0;
    if(DEBUG_T){
        printf("THREAD: Init_thread # Thread Initialization Complete for thread: %llu\n",thr);
    }
    insert_thread(mgr,thr);
    return thr;
}

void destroy_thread(uthread* t){
    node* h;
    char* x;
    // Destroys the thread and frees the stack resources recursively
    if(t == NULL)
        return;
    //Empty the child queue
    h = (int)dequeue(&(t->child_h),&(t->child_t));
    while(h != 0)
        h = (int)dequeue(&(t->child_h),&(t->child_t));

    x = t->stack;
    t->stack =NULL;
    free(x); // Free the stack
    if(DEBUG_T){
        printf("THREAD: Destroy_thread # Stack for the thread at %d destroyed\n",t->id);
        printf("THREAD: Destroy_thread # Freed the resources used for the thread %d\n",t->id);
    }
    free(t); //Free the thread resources
}

void print_thread(uthread* thread){
    if(thread == NULL){
        printf("NULL Thread\n");
        return;
    }
    printf("Thread ID: %d\n ",thread->id);
    printf("State : %c\n",thread->state);
    printf("Context : %llu\n",&(thread->context));
    printf("Waiting : %llu\n",thread->waiting);
    printf("Blocked : %d\n",thread->blocked_count);
    printf("Children: \n");
    node* h=thread->child_h;
    while(h!= NULL){
        printf("%d ",(int)(h->payload));
        h=h->next;
    }
    printf("\n");
}
