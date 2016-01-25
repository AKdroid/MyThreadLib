#include<stdio.h>
#include"myqueue.h"
/*
    To compile run as follows:
    gcc queue_test.c -I ../src -o queue_test.o
    To execute:
    ./queue_test.o
*/


void traverse_queue(node *a){
    
    while(a!=NULL){
        printf("Node : %d\n",*(int*)a->payload);
        a = a->next;
    }
    
}

int main(){
    node *h, *t;
    int a,b,c,d;
    a=3;
    b=5;
    c=6;
    d=10;
    init_queue(&h,&t);
    printf("Enqueue a\n");
    enqueue(&h,&t,&a);
    printf("Enqueue b\n");
    enqueue(&h,&t,&b);
    printf("Enqueue c\n");
    enqueue(&h,&t,&c);
       
    traverse_queue(h);
 
    printf("1: %d\n", *(int*)dequeue(&h,&t));
    printf("2: %d\n", *(int*)dequeue(&h,&t));
    printf("3: %d\n", *(int*)dequeue(&h,&t));

    printf("Enqueue b\n");
    enqueue(&h,&t,&b);
    printf("Enqueue c\n");
    enqueue(&h,&t,&c);
    printf("Enqueue a\n");
    enqueue(&h,&t,&a);

    traverse_queue(h);   

    printf("c in queue: %d\n",exists(h,&c));
    
    traverse_queue(h);    

    printf("d in queue: %d\n",exists(h,&d));    

    traverse_queue(h);

    printf("1: %d\n", *(int*)peek(h));


    printf("1: %d\n", *(int*)dequeue(&h,&t));
    printf("2: %d\n", *(int*)dequeue(&h,&t));
    printf("3: %d\n", *(int*)dequeue(&h,&t));    

    if (peek(h) == NULL) {
        printf("SUCCESS: Empty Queue As Expected\n");
    }
    else{
        printf("FAIL: Expected an Empty Queue\n");
    }

    return 0;
}
