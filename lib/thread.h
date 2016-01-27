#ifndef THREAD_H
#define THREAD_H
#include<ucontext.h>
#include"myqueue.h"
#define DEBUG_T 1
#define READY 'R'
#define RUNNING 'X'
#define BLOCKED 'B'
#define ZOMBIE 'Z'
#define TERMINATED 'T'
#define BUCKETS 10


struct uthread {
    int id;
    char state; // state of the thread
    ucontext_t context; // context of the thread
    int waiting; //parent thread if blocked else null
    struct node* child_h; // head of the child queue/list 
    struct node* child_t; // tail of the child queue/list
    int blocked_count;
};

struct lnode{
    struct lnode* next;
    struct uthread* payload;
};

struct thread_manager{
    struct lnode* map[BUCKETS];
    int serial;
};

typedef struct uthread uthread;
typedef struct thread_manager manager;
typedef struct lnode lnode;

//manager functions
void init_manager(manager* mgr);
void insert_thread(manager* mgr,uthread* thread);
void delete_thread(manager* mgr,int id);
uthread* get_thread(manager* mgr,int id);
int get_next_id(manager* mgr);
void cleanup_thread(manager* mgr);

uthread* init_thread(manager* mgr, ucontext_t* parent, char state, void(*func)(void*), void* args);
void destroy_thread(uthread* thr);
void print_uthread(uthread* thr);

#endif /* THREAD_H */
