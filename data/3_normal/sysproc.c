#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "procstat.h" // [os-prj3] 프로세스 통계를 위한 헤더

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

/**
  [os-prj3] It is used for sdebug.
  Set proc's weight to user's argument.
   */
int
sys_weightset(void)
{
  int weight;
  if(argint(0, &weight) < 0)
    return -1;
  if(weight <= 0)
	return -1; 
  do_weightset((unsigned long)weight);
  return weight;
}

// [os-prj3] wait을 하되, 자식의 프로세스 상태를 넘긴다.
int
sys_waitx(void)
{
    struct proc_stat *proc_stat;

    // [os-prj3] 사용자로부터 pid와 통계 구조체의 주소(포인터)를 가져옴
    if(argptr(0, (void*)&proc_stat, sizeof(struct proc_stat)) < 0)
        return -1;

    return waitx(proc_stat);
}