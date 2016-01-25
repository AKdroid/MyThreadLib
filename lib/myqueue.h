#ifndef MYQUEUE_H
#define MYQUEUE_H
#define DEBUG_Q 0
struct node{

    struct node*  next;
    void* payload;
};

typedef struct node node;

void init_queue(node** head,node **tail);
void enqueue(node** head, node** tail, void* data);
void* dequeue(node** head, node** tail);
void* peek(node* head);
int exists(node* head, void* query);

#endif /* MYQUEUE_H */
