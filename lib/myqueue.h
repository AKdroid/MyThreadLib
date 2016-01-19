#ifndef MYQUEUE_H
#define MYQUEUE_H

struct node{
    struct node* next;
    void* payload;    
};

typedef struct node node;

void init_queue(node** head, node** tail);
void enqueue(node** head, node** tail, void* payload);
void* dequeue(node** head, node** tail);
void* peek(node* head);
int exists(node* head, void* query);

#endif /* MYQUEUE_H */
