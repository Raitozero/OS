#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL

#define UPPERBOUND 64
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 va, dst;
  int len;
  if(argaddr(0, &va) | argint(1, &len) | argaddr(2, &dst)) return -1;
  len = len > UPPERBOUND ? UPPERBOUND: len;
  uint64 buffer = 0;
  for(int i = 0; i < len; i++){
    pte_t* pte = walk(myproc()->pagetable, va, 0);
    if(*pte & PTE_A){
      //if the 1st page is accessed, buffer = ...0001; and if 2nd page, buffer = ...0011;
      buffer = buffer | (1L << i);
      //clear PTE_A after checking if it is set
      *pte ^= PTE_A;
    }
    va += PGSIZE;
  }
  //notice this has to be &buffer because copyout() copies from (char* src), an addr
  if(copyout(myproc()->pagetable, dst, (char*)&buffer, sizeof(buffer))) return -1;
  return 0;
}

#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
