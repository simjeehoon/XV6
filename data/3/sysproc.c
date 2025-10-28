#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "procstat.h"
#include "spinlock.h"

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


// [os-prj3]
int
sys_get_proc_stats(void)
{
    int pid;
    struct proc_stat *stats;
    struct proc *p;

    // [os-prj3] 1. 사용자로부터 pid와 통계 구조체의 주소(포인터)를 가져옴
    if(argint(0, &pid) < 0)
        return -1;
    if(argptr(1, (void*)&stats, sizeof(*stats)) < 0)
        return -1;

    // [os-prj3] 2. 프로세스 테이블을 잠그고 (lock) 해당 pid의 프로세스를 찾음
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
        if(p->pid == pid){
            // 3. [os-prj3] 커널 데이터(p->...)를 사용자 구조체(*stats)로 복사
            stats->pid = p->pid;
            stats->weight = p->weight;
            stats->arrival_time = p->arrival_time;
            stats->completion_time = p->completion_time;
            stats->cpu_time = p->cpu_time;
            stats->first_run_time = p->first_run_time;

            release(&ptable.lock);
            return 0; // [os-prj3] 성공
        }
    }

    release(&ptable.lock);
    return -1; // [os-prj3] 프로세스를 찾지 못함
}