#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>
#include"myqueue.h"
#include"thread.h"

manager mgr;

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
    init_manager(&mgr);
    a = init_thread(&mgr,&main,RUNNING,f,&t1);
    b = init_thread(&mgr,&main,READY,f,&t2);
    
    c = get_thread(&mgr,0);

    if(c == a)
        printf("C Equals A\n");
    
    c = get_thread(&mgr,1);
    
    if(c == b){
        printf("C equals B\n");
    }
    
    swapcontext(&main,&(a->context));
    print_thread(a);
    print_thread(b);
    swapcontext(&main,&(b->context));
    printf("Success\n");
    cleanup_thread(&mgr);
}
