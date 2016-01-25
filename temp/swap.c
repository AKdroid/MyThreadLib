#include<stdio.h>
#include<stdlib.h>
#include<ucontext.h>


ucontext_t * uc[3];

void f(int p){
    int count =5;
    while(count > 0){
        count--;
        printf("Thread execution no. %d and count: %d\n",p,count);
        swapcontext(uc[p],uc[3-p]);
    }
}

int main(){

    int i;
    char* stack1,*stack2;
    stack1 = (char*)malloc(8192);
    stack2 = (char*)malloc(8192);
    for(i=0;i<3;i++){
        uc[i]=(ucontext_t*)malloc(sizeof(ucontext_t));
        getcontext(uc[i]);
    }
    
    uc[1]->uc_link = uc[0];
    uc[2]->uc_link = uc[0];

    uc[1]->uc_stack.ss_sp = stack1;
    uc[2]->uc_stack.ss_sp = stack2;
    
    uc[1]->uc_stack.ss_size = 8192;
    uc[2]->uc_stack.ss_size = 8192;
    
    makecontext(uc[1],(void(*)(void))f,1,1);
    makecontext(uc[2],(void(*)(void))f,1,2);
    
    swapcontext(uc[0],uc[1]);

    printf("Execution completed\n");
    
    return 0;
}
