#include <stdio.h>
#include "mythread.h"

int n;

void t0(void * dummy)
{
  int i;
  printf("t0 start: %d\n",n);
  n--; 
  if(n > 0)
    MyThreadCreate(t0,NULL);
    
  MyThreadExit();
}

int main()
{
  n = 5;
  MyThreadInit(t0, NULL);
}
