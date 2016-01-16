#include<ucontext.h>
#include<stdio.h>
#include<stdlib.h>

void f1(void){
    int i=100;
    //for(i=0;i<99;i++)
        printf("f1 : %d\n",i);
}

void f2(){
    int i=200;
    //for(i=1000;i<1099;i++)
        printf("f2 : %d\n",i);
}

ucontext_t main_context;
ucontext_t f1_context;
ucontext_t f2_context;

int main(){

    int x,y;
    //char t1_stack[81920];
    //char t2_stack[81920];
    char t1_stack[8192];
    char t2_stack[8192];
    f1_context.uc_link = &main_context;
    f1_context.uc_stack.ss_sp = t1_stack;
    f1_context.uc_stack.ss_size = 8192;

    f2_context.uc_link = &main_context;
    f2_context.uc_stack.ss_sp = t2_stack;
    f2_context.uc_stack.ss_size = 8192;
    //f1_context.uc_link = &main_context;
    //f1_context.uc_stack.ss_flags = 0;
    getcontext(&f1_context);
    makecontext(&f1_context,(void (*) (void))f1,0);
    
    getcontext(&f2_context);
    makecontext(&f2_context,(void (*) (void))f2,0);
//    x = getcontext(&main_context);
    printf("Before Swapping Context");
    swapcontext(&main_context,&f1_context);
    printf("Before Swapping Context 2");
    swapcontext(&main_context,&f2_context);
    printf("We are here\n");

    return 0;
}
