#ifndef MYSEMAPHORE_H
#define MYSEMAPHORE_H
#define DEBUG_S 0

#include"myqueue.h"
#include"thread.h"
#define BOXES 5
struct usemaphore{
    int id;
    int status;  
    node * wait_h;
    node * wait_t;
};

struct snode{
    struct snode* next;
    struct usemaphore* payload;
};

struct semaphore_manager{
    struct snode* map[BOXES];
    int serial;
};

typedef struct usemaphore usemaphore;
typedef struct snode snode;
typedef struct semaphore_manager smanager;

void init_smanager(smanager* mgr);
void insert_semaphore(smanager* mgr,usemaphore* thread);
void delete_semaphore(smanager* mgr,int id);
usemaphore* get_semaphore(smanager* mgr,int id);
int get_next_sid(smanager* mgr);
void cleanup_semaphores(smanager* mgr);


#endif /* MYSEMAPHORE_H */
