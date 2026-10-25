#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"


extern char end[]; // first address after kernel loaded from ELF file
                   // defined by the kernel linker script in kernel.ld

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
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

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//my syscall
int sys_setpriority(void) {
  int pid, priority;
  if (argint(0, &pid) < 0 || argint(1, &priority) < 0) {
    return -1; // Invalid arguments
  }
  return setpriority(pid, priority);
}

int sys_setburst(void) {
  int pid, bt;
  if (argint(0, &pid) < 0 || argint(1, &bt) < 0) {
    return -1; // Invalid arguments
  }
  return setburst(pid, bt);
}

int sys_getpriority(void) {
  int pid;
  if (argint(0, &pid) < 0) {
    return -1; // Invalid argument
  }
  return getpriority(pid);
}



int
sys_va2pa(void)
{
  int va;
  if(argint(0, &va) < 0)
    return -1;

  pde_t *pgdir = myproc()->pgdir;
  pte_t *pte = walkpgdir(pgdir, (void*)va, 0);
  if(!pte || !(*pte && PTE_P) || !(*pte && PTE_U))        //check if the page is present and user accessible 
    return -1;   
  
  uint pdx = PDX(va);
  uint ptx = PTX(va);
  uint offset = va & 0xFFF; // extract the offset within the page

  int p = (*pte & PTE_P) ? 1 : 0;
  int w = (*pte & PTE_W) ? 1 : 0;
  int u = (*pte & PTE_U) ? 1 : 0;

  uint pa = PTE_ADDR(*pte) | offset; // Calculate physical address

  cprintf("VA: 0x%x, PDX: %d, PTX: %d, Offset: %d, P: %d, W: %d, U: %d\n", va, pdx, ptx, offset, p, w, u);

  return pa;
}



#include "meminfo.h"

int
sys_getmeminfo(void)
{
  struct meminfo *mi;
  struct proc *curproc = myproc(); // Use curproc/myproc() based on your setup

  if(argptr(0, (char**)&mi, sizeof(*mi)) < 0)
    return -1;

  mi->total_pages = (PHYSTOP - V2P(end)) / PGSIZE;
  mi->free_pages = count_free_pages();
  mi->proc_user_pages = (curproc->sz + PGSIZE - 1) / PGSIZE;
  mi->proc_pt_pages = count_pt_pages(curproc->pgdir);

  return 0;
}