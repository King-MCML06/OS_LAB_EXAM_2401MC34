
#include "types.h"
#include "stat.h"
#include "user.h"
int global_var = 123;

int
main(void)
{
  int initial_free;
  int after_sbrk;
  int before_fork;
  int after_fork;
  int after_wait;
  int va, pa;
  int pid;
  int p[2];
  char *memory;
  char token = 'x';
  initial_free = freepages();
  printf(1, "Free pages at startup: %d\n", initial_free);
  memory = sbrk(10 * 4096);
  if(memory == (char*)-1) {
    printf(1, "sbrk failed\n");
    exit();
  }
  after_sbrk = freepages();
  printf(1, "Free pages after allocating 10 pages: %d\n", after_sbrk);
  va = (int)&global_var;
  pa = va2pa(va);
  printf(1, "Global variable virtual address: 0x%x\n", va);
  printf(1, "Global variable physical address: 0x%x\n", pa);
  printf(1, "va2pa(0x7FFFFFFF) = %d\n", va2pa(0x7FFFFFFF));
  if(pipe(p) < 0) {
    printf(1, "Pipe creation failed\n");
    exit();
  }
  before_fork = freepages();
  printf(1, "Free pages before fork: %d\n",before_fork);
  pid = fork();
  if(pid < 0) {
    printf(1, "Fork failed\n");
    close(p[0]);
    close(p[1]);
    exit();
  }
  if(pid == 0) {
    int child_va;
    int child_pa;
    close(p[1]);
    child_va = (int)&global_var;
    child_pa = va2pa(child_va);
    printf(1, "Child: virtual address = 0x%x\n", child_va);
    printf(1, "Child: physical address = 0x%x\n", child_pa);
    if(read(p[0], &token, 1) != 1) {
      printf(1, "Pipe read failed\n");
    }
    close(p[0]);
    exit();
  }
  close(p[0]);
  printf(1, "Parent: virtual address = 0x%x\n",va);
  printf(1, "Parent: physical address = 0x%x\n",va2pa(va));
  after_fork = freepages();
  printf(1, "Free pages after fork: %d\n",after_fork);
  write(p[1], &token, 1);
  close(p[1]);
  wait();
  after_wait = freepages();
  printf(1, "Free pages after child exit and wait: %d\n",after_wait);
  printf(1, "Free pages before fork were: %d\n",before_fork);
  if(after_wait == before_fork)
    printf(1, "Memory successfully reclaimed. No leak detected.\n");
  else
    printf(1, "Free page count mismatch!\n");
  exit();
}