#include <stdio.h>
#include "mythread.h"

int n;

void t0(void * dummy)
{
  int i;
  printf("t0 start: %d\n",n);
  n--; 
  for(i=0;i<n;i++)
    MyThreadCreate(t0,NULL);    
  MyThreadJoinAll();
  MyThreadExit();
}

int main()
{
  n = 10;
  MyThreadInit(t0, NULL);
}
