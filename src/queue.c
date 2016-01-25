#include<stdio.h>
#include<stdlib.h>
#include"myqueue.h"



void init_queue(node** head,node **tail){

    // Initialize the head and tail pointers to NULL
    *head = NULL;
    *tail = NULL;
    if (DEBUG_Q)
        printf("QUEUE: Init Queue # head : %llu -> %llu | tail : %llu -> %llu\n",head,*head,tail,*tail);
}

void enqueue(node** head, node** tail, void* data){
    
    // Enqueues a new node to the queue with head and tail of the queue pointed by head and tail
    // Payload for new node is provided by data
    node* new_node;

    if(DEBUG_Q){
        printf("QUEUE: Enqueue # Before Enqueue head %llu -> %llu | tail : %llu -> %llu\n",head,*head,tail,*tail);
    }

    // Create a new node with payload data
    new_node = (node*) malloc (sizeof(node));
    
    if (new_node == NULL){
        //Handle malloc failure
        printf("QUEUE: Enqueue # Malloc Failed while creating new node\n");
        return;
    }
    
    if (DEBUG_Q){
        printf("QUEUE: Enqueue #Malloc successful new_node at %u of size %d\n",new_node,sizeof(node));
    }
    //Assign the data to payload
    new_node->payload = data;
    //Next is null since insertion at the end of queue
    new_node->next = NULL;

    //if head is null, set the head to new node
    //else add the new node as next to the current tail node
    //update new node as the new tail node
    if (*head == NULL){
        *head = new_node;
    } else {
        (*tail)->next = new_node;
    }
    (*tail) = new_node;
    if(DEBUG_Q){
        printf("QUEUE: Enqueue # Enqueue complete head %llu -> %llu | tail : %llu -> %llu\n",head,*head,tail,*tail);
    }
}

void* dequeue(node** head, node** tail){
    
    //Removes the node at the head of the queue and returns its payload
    //if the head is null, null is returned   
    
    void *temp;
    node* del;
    if(DEBUG_Q){
        printf("QUEUE: Dequeue # Before Dequeue head %llu -> %llu | tail : %llu -> %llu\n",head,*head,tail,*tail);
    }
    
    if(*head == NULL){
        if(DEBUG_Q){
            printf("QUEUE: Dequeue # Attempted dequeue of an empty queue\n");
        }
        return NULL;
    }
    // Get the node at head and save its contents in temp;
    del = *head;
    temp = del->payload;
    //Free the memory for del 
    //Assign head to the next node
    *head = del->next;
    free(del);
    // If new head is null, set tail to NULL as well
    if( *head == NULL)
        *tail = NULL;
    if(DEBUG_Q){
        printf("QUEUE: Dequeue # Dequeue complete head %llu -> %llu | tail : %llu -> %llu | dequeued payload : %llu\n",head,*head,tail,*tail,temp);
    }
    return temp;
}

void* peek(node* head){
    
    // Returns the payload on the head of the queue
    // If head is null, NULL is returned
    if(DEBUG_Q){
        printf("QUEUE: Peek # head %llu \n",head);
    }
    if(head == NULL)
        return NULL;
    return head->payload;

}

int exists(node* head, void* query){
    
    // Returns 1 if a node exists such that its payload is equal to query else 0
    node* runner;
    if(DEBUG_Q){
        printf("QUEUE: Exists #  %llu | query : %llu\n",head,query);
    }
    runner = head;
    while(runner != NULL){

        if(runner->payload == query){
            if(DEBUG_Q){
                printf("QUEUE: Exists #  %llu | query : %llu | result : FOUND\n",head,query);
            }
            return 1;
        }    
        runner = runner->next;
    }
    if(DEBUG_Q){
        printf("QUEUE: Exists #  %llu | query : %llu | result : NOT FOUND\n",head,query);
    }
    return 0;

}



