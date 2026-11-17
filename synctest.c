#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
  int pid;
  if(sem_init(0, 1) < 0 || sem_init(1, 0) < 0 || sem_init(2, 0) < 0) 
  {
    printf(1, "Semaphore initialization failed\n");
    exit();
  }
  pid = fork();
  if(pid == 0) 
  {
    for(int i = 0; i < 5; i++) 
    {
      sem_wait(0);
      printf(1, "A");
      sem_post(1);
    }
    exit();
  }
  if(pid < 0) 
  {
    printf(1, "Fork failed\n");
    exit();
  }

  pid = fork();
  if(pid == 0) 
  {
    for(int i = 0; i < 5; i++) 
    {
      sem_wait(1);
      printf(1, "B");
      sem_post(2);
    }
    exit();
  }
  if(pid < 0) 
  {
    printf(1, "Fork failed\n");
    exit();
  }

  pid = fork();
  if(pid == 0) 
  {
    for(int i = 0; i < 5; i++) 
    {
      sem_wait(2);
      printf(1, "C");
      sem_post(0);
    }
    exit();
  }
  if(pid < 0) 
  {
    printf(1, "Fork failed\n");
    exit();
  }

  for(int i = 0; i < 3; i++)
    wait();
  printf(1, "Done\n");
  exit();
}