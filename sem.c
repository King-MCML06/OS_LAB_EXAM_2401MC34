#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#define NSEMS 8

static struct {
  struct spinlock lock;
  struct {
    int value;
    int inuse;
  } sem[NSEMS];
} semtable;

void seminit(void)
{
  initlock(&semtable.lock, "semtable");
  for(int i = 0; i < NSEMS; i++) 
  {
    semtable.sem[i].value = 0;
    semtable.sem[i].inuse = 0;
  }
}

int ksem_init(int id, int value)
{
  if(id < 0 || id >= NSEMS || value < 0)
    return -1;
  acquire(&semtable.lock);
  semtable.sem[id].value = value;
  semtable.sem[id].inuse = 1;
  release(&semtable.lock);
  return 0;
}

int ksem_wait(int id)
{
  if(id < 0 || id >= NSEMS)
    return -1;
  acquire(&semtable.lock);
  if(semtable.sem[id].inuse == 0) {
    release(&semtable.lock);
    return -1;
  }
  while(semtable.sem[id].value == 0) {
    sleep(&semtable.sem[id], &semtable.lock);
  }
  semtable.sem[id].value--;
  release(&semtable.lock);
  return 0;
}

int ksem_post(int id)
{
  if(id < 0 || id >= NSEMS)
    return -1;
  acquire(&semtable.lock);
  if(semtable.sem[id].inuse == 0) 
  {
    release(&semtable.lock);
    return -1;
  }
  semtable.sem[id].value++;
  wakeup(&semtable.sem[id]);
  release(&semtable.lock);
  return 0;
}