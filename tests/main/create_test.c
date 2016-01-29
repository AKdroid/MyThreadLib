#include <stdio.h>
#include "mythread.h"

void t0(void * dummy)
{
  int i;
  printf("t0 start\n");
  for(i=0;i<20;i++){
    MyThreadCreate(t0,NULL);
  }  
  MyThreadExit();
}

int main()
{
  MyThreadInit(t0, NULL);
}
