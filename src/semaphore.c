#include<stdio.h>
#include<stdlib.h>
#include"myqueue.h"
#include"mysemaphore.h"


void init_smanager(smanager* mgr){
    int i;
    for(i=0;i<BOXES;i++){
        mgr->map[i] = NULL;
    }
    mgr->serial = 1;
}

void insert_semaphore(smanager* mgr,usemaphore* sem){
    snode* head, *new_node;
    int id;
    id=sem->id;
    head = mgr->map[id%BOXES];
    new_node = (snode*) malloc(sizeof(lnode));
    new_node->payload = sem;
    new_node->next = head;
    mgr->map[id%BOXES] = new_node;
}

void delete_semaphore(smanager* mgr,int id){
    snode* head=NULL;
    snode* x=NULL;
    snode* y=NULL;
    usemaphore* t;
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
    free(t);
    free(x);
}

usemaphore* get_semaphore(smanager* mgr,int id){
    usemaphore* result = NULL;
    snode* x=NULL;
    snode* head = mgr->map[id%BOXES];
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

int get_next_sid(smanager* mgr){
    int id = mgr->serial;
    mgr->serial+=1;
    return id;
}

void cleanup_semaphores(smanager* mgr){
    int i;
    node* x,*del;
    usemaphore* t;
    int temp;
    for(i=0; i< BOXES; i++){
        x = mgr->map[i];
        while(x!=NULL){
            del = x;
            x = x->next;
            t = del->payload;
            del->next = NULL;

            temp = (int) dequeue(&(t->wait_h),&(t->wait_t));            
            while(temp != 0)
                temp = (int) dequeue(&(t->wait_h),&(t->wait_t));
            free(t);
            free(del);
        }
        mgr->map[i]=NULL;
    }
}
