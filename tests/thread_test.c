#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include"myqueue.h"
#include"thread.h"

void f(void *x){
    int *p =(int*)x;
    int i,t = *p;
    for(i=0;i<t;i++ ){
        printf("Hey There %d\n",t);
    }
}

int main(){

    uthread *a=NULL,*b=NULL,*c=NULL;
    int t1=4;
    int t2=5;
    ucontext_t main;
    init_thread(&a,&main,RUNNING,f,&t1);
    init_thread(&b,&main,READY,f,&t2);
    swapcontext(&main,&(a->context));
    print_thread(a);
    a->state = TERMINATED;
    destroy_thread(&a);
    print_thread(a);
    swapcontext(&main,&(b->context));
    b->state = TERMINATED;
    destroy_thread(&b);
    printf("Success\n");
}
