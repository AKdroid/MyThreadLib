#include<stdio.h>
#include "mythread.c"

/*
    To compile run as follows:
    gcc queue_test.c -I ../src -o queue_test.o
    To execute:
    ./queue_test.o
*/

void init_thread(MyThread* x, char state){
    x->state=state;
    init_queue(&(x->head),&(x->tail));
}

int main(){
    node *h, *t;
    MyThread a,b,c;
    void *temp;
    init_queue(&h,&t);

    init_thread(&a, THREAD_READY);
    init_thread(&b, THREAD_BLOCKED);
    init_thread(&c, THREAD_EXITED);
    

    enqueue_mythread(&h,&t,&a);
    enqueue_mythread(&h,&t,&b);
    enqueue_mythread(&h,&t,&c);
    
    printf("1: %c\n", dequeue_mythread(&h,&t)->state);
    printf("2: %c\n", dequeue_mythread(&h,&t)->state);
    printf("3: %c\n", dequeue_mythread(&h,&t)->state);

    enqueue_mythread(&h,&t,&b);
    enqueue_mythread(&h,&t,&c);
    enqueue_mythread(&h,&t,&a);

    printf("1: %c\n", peek_mythread(h)->state);

    printf("1: %c\n", dequeue_mythread(&h,&t)->state);
    printf("2: %c\n", dequeue_mythread(&h,&t)->state);
    printf("3: %c\n", dequeue_mythread(&h,&t)->state);    

    if (peek_mythread(h) == NULL) {
        printf("SUCCESS: Empty Queue As Expected\n");
    }
    else{
        printf("FAIL: Expected an Empty Queue\n");
    }

    return 0;
}
