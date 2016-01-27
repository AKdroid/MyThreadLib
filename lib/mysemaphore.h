#ifndef MYSEMAPHORE_H
#define MYSEMAPHORE_H
#define DEBUG_S 0

#include"myqueue.h"
#include"thread.h"

struct usemaphore{
  
    int status;
    int init_value;  
    node * wait_h;
    node * wait_t;
    uthread * owner;
};

typedef usemaphore usemaphore;

#endif /* MYSEMAPHORE_H */
