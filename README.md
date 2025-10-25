XV6의 스케줄링, 파일시스템 개선하기
===

# XV6란?
  * MIT에서 교육 목적으로 개발된 간단한 `유닉스(Unix)` 계열의 OS입니다.
  * 초기 유닉스 버전인 `유닉스 V6(Unix Version 6)`를 현대적인 멀티프로세서 환경(주로 `RISC-V` 또는 `x86`)에 맞게 `ANSI C` 언어로 다시 구현했습니다.
  * OS가 비교적 단순하지만,  __프로세스 관리__, __메모리 관리__, __파일 시스템__, __시스템 콜(System Call)__ 등
  `UNIX` OS의 주요 개념과 구성을 모두 포함하고 있습니다.


# 목표
  1. **XV6**의 **스케줄링 알고리즘**을 개선합니다. ([바로가기](https://github.com/simjeehoon/XV6/tree/master?tab=readme-ov-file#3-xv6%EC%9D%98-%EB%8D%94-%ED%9A%A8%EC%9C%A8%EC%A0%81%EC%9D%B8-%EC%8A%A4%EC%BC%80%EC%A4%84%EB%A7%81-%EC%95%8C%EA%B3%A0%EB%A6%AC%EC%A6%98%EC%9D%84-%EC%A0%81%EC%9A%A9))
  2. **XV6**에 **파일 시스템**을 추가합니다. ([바로가기](https://github.com/simjeehoon/XV6/tree/master?tab=readme-ov-file#5-xv6%EC%9D%98-%EB%8D%94-%ED%9A%A8%EC%9C%A8%EC%A0%81%EC%9D%B8-%ED%8C%8C%EC%9D%BC-%EC%8B%9C%EC%8A%A4%ED%85%9C%EC%9D%84-%EC%A0%81%EC%9A%A9))


# 설치
  *저는 `우분투 리눅스`에서 모든 과정을 진행했습니다.*

  `XV6` OS 수정 과정은 다음과 같습니다.

  1. `git clone https://github.com/mit-pdos/xv6-public.git`
   로 `clone`을 진행하고


  2. `sudo apt-get install qemu-kvm`
   로 QEMU x86 에뮬레이터를 설치한 뒤,


  3. 소스코드를 추가하거나 수정하고 `Makefile`을 수정하여


  4. `make` 명령어를 수행하여 xv6를 컴파일하고 `make qemu` 를 수행하여 qemu 에뮬레이터로 구동하면 됩니다.

---

## 위의 복잡한 과정을 `setup.sh`가 대신 실행해줍니다!
- 저는 위 과정을 전부 거쳤으나, 이 복잡한 과정들을 알기 쉽게 보여드리려고 `setup.sh`을 만들었습니다.

>  **즉, `sudo apt-get install qemu-kvm` 만 설치해주시면 됩니다.**
> ![APTGET](https://github.com/simjeehoon/src_repository/blob/master/XV6/master/0/aptget_qemukvm.png?raw=true)

- `setup.sh`을 실행하시면 `xv6-public`을 클론하고 수정된 `Makefile`을 알아서 적용합니다.

  - ![setup](https://github.com/simjeehoon/src_repository/blob/master/XV6/master/0/setupshANDcdmake.png?raw=true)

- 위 과정이 완료되었다면, `xv6-public` 디렉토리로 이동하셔서 `make`를 입력하시고(XV6 컴파일) `make qemu`을 입력하시면(에뮬레이터로 XV6 구동) 됩니다.

  - ![make_qemu](https://github.com/simjeehoon/src_repository/blob/master/XV6/master/0/makeqemu.png?raw=true)

---

- 이후의 과정들의 파일 세팅도 `setup.sh`이 진행합니다.
- `setup.sh` 에 인자로 **번호**를 넘기시면 해당 과정에 필요한 파일들을 알아서 세팅합니다.
> 즉, `./setup.sh 1` 이런식으로 하시면 됩니다.
> ![setup1_1](https://github.com/simjeehoon/src_repository/blob/master/XV6/master/0/setup1_1.png?raw=true)
> ![setup1_2](https://github.com/simjeehoon/src_repository/blob/master/XV6/master/0/setup1_2.png?raw=true)
>
> 그 뒤에 `xv6-public`으로 이동하셔서 `make` 하신 뒤 `make qemu`를 하시면 됩니다.

---

# 1. XV6를 어떻게 수정하는가?
 앞서 말했듯이, 본 과정을 보시려면 `./setup.sh 1`을 입력하시고 진행해주세요.

---

## (1) `helloworld.c`를 XV6에서 돌려보기

1. 프로그램 작성
 - `xv6-public` 디렉토리에서 `helloworld.c` 를 작성합니다.

   - ![image08](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image08.png?raw=true)
   - 위 사진은 `helloworld.c` 코드입니다.

2. Makefile 수정
 - `Makefile`의 `UPROGS`에 `_helloworld`를 추가하고 `EXTRA`에 `helloworld.c`를 추가합니다.

   - ![image09](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image09.png?raw=true)

3. make로 컴파일
 - `make clean` 이후 `make`를 진행합니다.

   - ![image10](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image10.png?raw=true)
 
4. xv6에서 확인
 - `helloworld` 명령어가 잘 수행됩니다.

   - ![image11](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image11.png?raw=true)

---

## (2) hcat 쉘 프로그램 작성
 
 - 첫 n줄만 출력해주는 hcat을 만듭니다.

 - `hcat.c`를 작성하고 이전과 동일한 과정을 진행합니다.

 - `hcat <정수> <파일명>`을 입력하면 파일이 입력한 정수줄만큼 출력됩니다. 아래는 사진입니다.

 - ![hcat](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image15a.png?raw=true)

---

## (3) 부팅 시 로그인 구현

- `init.c`에서 `ssu_login`을 fork-exec 하도록 수정합니다.

- ![forkexec](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image17.png?raw=true)

- 그렇게 함으로써 xv6 부팅시 Username과 Password를 입력받고, `list.txt`와 비교하여 일치하는 정보가 있을 때만 쉘 프롬프트를 출력하도록 합니다.

  - ![list.txt](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image18.png?raw=true) <--- `list.txt` 파일

  - ![ssu_login.c](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image19.png?raw=true) <--- `ssu_login.c`
        
- 정상적으로 **로그인**하는 경우

    - ![login_succeeded](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image23.png?raw=true)

- 정보가 잘못된 경우

    - ![login_failed](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj1/image24.png?raw=true)


---

# 2. XV6에 시스템콜 추가

 시스템콜 2개를 추가하겠습니다.
 - 프로세스의 메모리 사용량을 출력하는 `memsize`
 - 프로세스가 호출한 시스템콜을 추적할 수 있는 `trace`

 `memsize`를 이용하여 `malloc` 사용 전, 후의 메모리 사용량을 비교해보고 `malloc`도 분석해보겠습니다.

 본 과정을 보시려면 `./setup.sh 2`를 수행해주세요.

## 시스템콜 호출 과정

> 1. 사용자 프로그램에서 시스템콜을 호출하면 **시스템콜 인터럽트**가 발생합니다.
> 2. 이후 **인터럽트**를 처리하는 과정에서 `trap`을 호출하고, `trap`은 다시 `syscall`을 호출하며, 최종적으로 `syscalls[num]()`을 호출됩니다.

 - 시스템콜을 등록하기 위해선 `num`에 해당하는 시스템콜 번호를 정의하고, 시스템콜에 대한 구현 역시 작성해야 합니다.

> 등록을 위한 절차입니다.
>
> 1. `sysproc.c`에 시스템콜 구현부를 추가한다.
> 2. `syscall.h`에 `#define`을 이용해 시스템콜 번호를 등록한다.
> 3. `syscall.c`에서 시스템콜 선언을 하고, `syscalls` 함수 포인터 배열에 시스템콜 함수 포인터를 등록한다. 인덱스는 시스템콜 번호를 따른다.
> 4. `user.h`에 사용자에게 보여지는 시스템콜을 등록한다.
> 5. `usys.S`에 매크로를 이용하여 시스템콜을 등록한다.

---

## (1) memsize 시스템콜 추가

> ![syscall.h](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image01.png?raw=true)
> 
> `syscall.h`에 `SYS_memsize`를 등록합니다.

> ![syscall.c](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image01b.png?raw=true)
> 
> `syscall.c`에 시스템콜 함수 선언을 작성하고 `syscalls` 배열에 추가합니다.

> ![user.h](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image02.png?raw=true)
> 
> `user.h`에 사용자에게 보여지는 시스템콜 선언을 추가합니다.

> ![usys.S](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image03.png?raw=true)
> 
> `usys.S`에 시스템콜을 등록합니다.

> ![sysproc.c](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image04.png?raw=true)
> 
> `sysproc.c`의 시스템콜 구현부에서 `myproc()`을 이용해 현재 프로세스의 `proc 구조체`를 얻습니다. 이 구조체의 `sz`값을 시스템콜의 반환값으로 설정합니다.

> ![memsizetest](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image05.png?raw=true)
>
> `memsize` 시스템콜 테스트를 위해 `memsizetest` 쉘 프로그램을 작성합니다.

> ![memsizetest](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image06.png?raw=true)
> 
> 쉘 프로그램 `memsizetest`가 잘 동작하고 있습니다.

>> ***`memsizetest`에서 `malloc` 전, 후 차이가 2048 바이트가 아닌 이유***
>> 
>> - `umalloc.c`을 분석합니다.
>> 
>>  ![umalloc.c](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image07.png?raw=true)
>> 
>> * `malloc`의 동작 방식은 힙 영역에 `nbytes`를 할당할 수 있을 만큼의 여유 공간이 존재하면, 해당 공간의 헤더를 설정하고 사용자에게 실제 사용할 수 있는 메모리 공간을 제공하는 것입니다.
>> * 만약 여유 공간이 존재하지 않으면 `malloc`의 87번째 줄처럼 `morecore`을 호출하여 여유공간을 확보합니다.
>> 
>>  ![`morecore`](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image08.png?raw=true)
>> 
>> * `morecore`은 52번째 줄에서 보듯 새로 할당할 공간이 `4096` 바이트보다 적으면 `4096` 바이트만큼의 힙 영역을 확장합니다. 
>> * 그 이유는 `sbrk` 시스템콜의 비용이 비싸기 때문입니다. `sbrk`는 메모리 공간의 데이터영역을 확장하는 시스템콜입니다. 확장한 부분을 동적할당을 위한 공간(힙 영역)으로 사용하는 것입니다.

---

## (2) trace 시스템콜 추가

> ![proc_tracemask](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image09.png?raw=true)
> 
> `trace` 시스템콜 구현을 위해 `proc.h`의 `proc` 구조체에 `uint tracemask`를 추가했습니다. 

> ![sys_trace](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image10.png?raw=true)
>
> `sys_trace`에서 이 값을 사용자가 설정한 시스템콜 마스크값으로 설정합니다.

> ![syscall.c](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image11.png?raw=true)
>
> 시스템콜을 추적하기 위해 `syscall.c`의 `syscall`에서 마스크값을 검사합니다.

> ![syscall_names](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image12.png?raw=true)
> 
> 시스템콜의 이름을 얻기 위해 문자열 배열인 `syscall_names`을 추가하였습니다.

> ![procfork](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image13.png?raw=true)
> 
> `proc.c`의 `fork`에서 자식 프로세스의 `proc` 구조체에 마스크값을 복사합니다. 이로써 `fork`를 통해 만든 자식 프로세스에서도 시스템콜 추적이 유지됩니다.

> ![register_sys](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image13b.png?raw=true)
> 
> 최종적으로 `syscall.h`, `syscall.c`, `usys.S`에 등록 작업을 합니다.

> ![ssu_trace](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image14.png?raw=true)
> 
> 시스템콜 추적을 위하여 쉘 프로그램인 `ssu_trace`을 작성합니다. `trace`를 먼저 호출한 후 `exec`을 호출합니다.

> ![ssutrace1](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image16.png?raw=true)
> 
> ![ssutrace2](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj2/image17.png?raw=true)

> `ssu_trace`가 정상적으로 동작합니다.

---
# 3. **XV6**의 더 효율적인 **스케줄링 알고리즘**을 적용
 - XV6의 기존 프로세스 스케줄러를 수정하여 개선합니다.
 - `./setup.sh 3`을 입력하시고 진행해주세요.

> ## SSU Scheduler
> - 새로운 스케줄러 이름은 `SSU Scheduler`라고 명명하였습니다.
> - `SSU Scheduler`는 가중치(`weight`)와 우선순위(`priority`)를 이용하여 다음 프로세스를 결정합니다.

 - 개선된 스케쥴링 함수의 동작 과정을 확인하기 위해 `sdebug` 명령어와 디버깅 기능도 XV6에 추가하겠습니다.
 - `[os-prj3]` 주석을 달아 수정된 부분을 보기 쉽게 하였습니다.

## (1) SSU Scheduler의 scheduling 알고리즘으로 수정하기

 XV6에서 프로세스는 proc 구조체에 의해 관리됩니다.

---

`proc.h` 의 **`struct proc`**

```c
// Per-process state
struct proc {
  uint sz;                     // Size of process memory (bytes)
  pde_t* pgdir;                // Page table
  char *kstack;                // Bottom of kernel stack for this process
  enum procstate state;        // Process state
  int pid;                     // Process ID
  struct proc *parent;         // Parent process
  struct trapframe *tf;        // Trap frame for current syscall
  struct context *context;     // swtch() here to run process
  void *chan;                  // If non-zero, sleeping on chan
  int killed;                  // If non-zero, have been killed
  struct file *ofile[NOFILE];  // Open files
  struct inode *cwd;           // Current directory
  char name[16];               // Process name (debugging)

  unsigned long priority;	   // [os-prj3] priority variable for scheduling
  unsigned long weight;				   // [os-prj3] weight variable for scheduling
};
```
> 가중치(`weight`)와 우선순위(`priority`)를 `proc` 구조체에 추가했습니다.


`proc.c` 의 **`ptable`**

```c
struct {
  struct spinlock lock;
  struct proc proc[NPROC];
  unsigned long minpriority;			// [os-prj3] min priority for new process
} ptable;
```
> 프로세스를 생성하거나 wake up할때 `proc`.`priority`에 할당할 가장 작은 값을 `ptable`의 `minpriority` 변수로 추가합니다.


 * `proc` 은 프로세스의 정보들이 담기는 구조체입니다.
 * 런타임에 `proc`들은 `ptable.proc` 배열에 존재합니다.
 
 * 결국 처음에는 `NPROC` 개수만큼의, 상태가 `UNUSED`인 `proc` 구조체 변수가 생기게 됩니다.
 * 이 `ptable`은 공유 자원이므로 `acquire(&ptable.lock)` 및 `require(&ptable.lock)`을 이용해 접근을 제한해야 합니다.
 
---

`proc.c`의 **`allocproc`**

```c
//PAGEBREAK: 32
// Look in the process table for an UNUSED proc.
// If found, change state to EMBRYO and initialize
// state required to run in the kernel.
// Otherwise return 0.
static struct proc*
allocproc(void)
{
  struct proc *p;
  char *sp;

  acquire(&ptable.lock);

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == UNUSED)
      goto found;

  release(&ptable.lock);
  return 0;

found:
  p->state = EMBRYO;
  p->pid = nextpid++;
  p->weight = nextweight++; // [os-prj3] Set weight
  p->priority = ptable.minpriority; // [os-prj3] Set min priority

  release(&ptable.lock);

  // Allocate kernel stack.
  if((p->kstack = kalloc()) == 0){
    p->state = UNUSED;
    return 0;
  }
  sp = p->kstack + KSTACKSIZE;

  // Leave room for trap frame.
  sp -= sizeof *p->tf;
  p->tf = (struct trapframe*)sp;

  // Set up new context to start executing at forkret,
  // which returns to trapret.
  sp -= 4;
  *(uint*)sp = (uint)trapret;

  sp -= sizeof *p->context;
  p->context = (struct context*)sp;
  memset(p->context, 0, sizeof *p->context);
  p->context->eip = (uint)forkret;

  return p;
}
 ```

 - `allocproc()`은 `ptable`의 `proc` 배열에서 상태가 `UNUSED`인 것을 하나 골라 `EMBRYO`로 바꿉니다.
 - 이것을 커널에서 요구하는 상태로 바꿔서 프로세스를 반환합니다.
 - `allocproc()`은 `fork`를 수행할 때, `userinit`을 이용하여 최초의 프로세스를 생성할 때 사용합니다.

---
`proc.c`의 전역변수 `nextweight`

```c 
int nextpid = 1;
unsigned long nextweight = 1;  // [os-prj3] weight value for new process
extern void forkret(void);
```

 - SSU Scheduler의 구현을 위해 전역변수 `nextweight`을 추가하고 1로 할당하였습니다. 
 - 수정된 `allocproc`에서 `nextweight`과 `ptable.minpriority`를 새로운 프로세스에 할당합니다.
 - `nextweight`은 `allocproc`에서 프로세스를 할당할 때마다 1 증가합니다.

---
`proc.c`의 `userinit`
```c
//PAGEBREAK: 32
// Set up first user process.
void
userinit(void)
{
  struct proc *p;
  extern char _binary_initcode_start[], _binary_initcode_size[];

  acquire(&ptable.lock); 
  ptable.minpriority = 3; // [os-prj3] Set minpriority
  release(&ptable.lock);

  p = allocproc();
  
  initproc = p;
  if((p->pgdir = setupkvm()) == 0)
    panic("userinit: out of memory?");
  inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
  p->sz = PGSIZE;
  memset(p->tf, 0, sizeof(*p->tf));
  p->tf->cs = (SEG_UCODE << 3) | DPL_USER;
  p->tf->ds = (SEG_UDATA << 3) | DPL_USER;
  p->tf->es = p->tf->ds;
  p->tf->ss = p->tf->ds;
  p->tf->eflags = FL_IF;
  p->tf->esp = PGSIZE;
  p->tf->eip = 0;  // beginning of initcode.S

  safestrcpy(p->name, "initcode", sizeof(p->name));
  p->cwd = namei("/");

  // this assignment to p->state lets other cores
  // run this process. the acquire forces the above
  // writes to be visible, and the lock is also needed
  // because the assignment might not be atomic.
  acquire(&ptable.lock);

  p->state = RUNNABLE;

  release(&ptable.lock);
}
```

- `userinit`은 `allocproc`으로 `proc`을 할당받고, 최초의 유저 프로세스 `initcode`를 실행합니다.
- `initcode`는 `init`을 생성하고, `init`은 `sh`을 생성하게 됩니다. *(priority가 1과 2가 됩니다.)*
- 최초의 유저 프로세스는 `priority` 값이 3이어야 하므로, `ptable.minpriority`를 3으로 할당합니다.

---

`proc.c`의 `wakeup1`

```c
//PAGEBREAK!
// Wake up all processes sleeping on chan.
// The ptable lock must be held.
static void
wakeup1(void *chan)
{
  struct proc *p;

  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
    if(p->state == SLEEPING && p->chan == chan){
	  p->priority = ptable.minpriority;	// [os-prj3] Set priority to min priority.
      p->state = RUNNABLE;
	}
}
```
 
 - wake up시에도 `priority`를 가장 작은 값으로 할당해야 합니다.
 
 - `schduler` 관련 함수는 `scheduler`와 `sched`가 있습니다.
 
 1. `scheduler()`
    - 각 CPU가 자신의 설정을 마친 이후에 호출하는 함수입니다.
    - 다음에 수행될 프로세스를 계속해서 선택하며, 반환되지 않습니다.
    - 프로세스가 선택되면, `proc` 구조체에 담겨 있는 페이지 테이블 정보, 컨텍스트 정보를 가져와서 커널과 교환합니다. 이후 CPU의 제어권은 선택된 프로세스에게 넘어갑니다. 
    - 선택된 프로세스를 수행하다가 적절한 인터럽트가 발생하면 다시 컨텍스트 스위칭이 일어나고, `scheduler()`로 제어권이 넘어옵니다.
    - 이후 커널은 다시 프로세스를 선택하고 동일한 과정을 반복한다.
 
 2. `sched()`
    - `sched()`는 다시 `scheduler`로 진입하는 역할을 합니다.
    - `scheduler`에서 제어권을 선택된 프로세스에게 넘긴 뒤, 나중에 다시 `scheduler`로 돌아올 때 호출됩니다.

---

`proc.c`의 **`scheduler`** 원본 코드

```c
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      /*
      ** 순차탐색하는 기본 코드
      */
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.

      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}
```

- 기존 `scheduler`는 다음 수행될 프로세스를 순차적으로 탐색합니다.
- 특정 프로세스가 집중적으로 수행되어 제어권이 공정하게 전달되지 않을 수 있습니다.
- **SSU Scheduler에서는 이를 해결하려고 합니다.**

**개선된 SSU Scheduler의 `scheduler`**

```c
void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;

  struct proc *selected; // [os-prj3] proc variable for SSU Scheduler
  unsigned long minpriority;  // [os-prj3] min priority variable for SSU Scheduler
  int updated; // [os-prj3] If ptable.minpriority is updated, this will be set 1.
  
  for(;;){
	selected = (void*)0; // [os-prj3] Set NULL to selected process pointer.
	updated = 0;  // [os-prj3] Set update to 0.
```
> - 프로세스 선정을 위한 `struct proc` 포인터 `selected`를 추가하였습니다.
> - SSU Scheduler는 가장 작은 `priority`를 선택하는 스케줄러이므로 비교를 위한 변수 `minpriority`를 추가하였습니다.
> - 이외에 구현을 위해 플래그 변수인 `updated`를 추가하였습니다.


```c
    // Enable interrupts on this processor.
    sti();
    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state == RUNNABLE){ // [os-prj3] Only select RUNNABLE process.
		/** 
		   [os-prj3] Select process which has min priority.
		 */
		if(selected == (void*)0 || minpriority > p->priority){
		  minpriority = p->priority;
		  selected = p;
		}
	  } 
	}
```
> RUNNABLE 프로세스 중 가장 작은 `priority`를 가진 프로세스를 선택하여 `selected`에 배정합니다.

```c
	if(selected != (void*)0){ // [os-prj3] If process is selected

		/**
		  [os-prj3] If it's debug mode, print process id, name, weight, priority.
		  */
#ifdef DEBUG
		cprintf("PID: %d, NAME: %s, WEIGHT: %d, PRIORITY: ",
				selected->pid, selected->name, selected->weight);
		printul(selected->priority);
		cprintf("\n");
#endif

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = selected; // [os-prj3] Set selected process.
      switchuvm(selected); // [os-prj3] Switch to selected process.
      selected->state = RUNNING; // [os-prj3] Set state to RUNNING.

      swtch(&(c->scheduler), selected->context); // [os-prj3] context switching.
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.

	  // [os-prj3] Calculate new priority.
#define TIME_SLICE 10000000L
	  selected->priority = selected->priority + (TIME_SLICE / selected->weight);

	  /** 
		 [os-prj3] Search min priority.
		 */
	  for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
		if(p->state == RUNNABLE){ 
		  if(!updated || minpriority > p->priority){
			updated = 1; // [os-prj3] Set update to 1.
			minpriority = p->priority;
		  }
		}
	  }
	  if(updated){ // [os-prj3] Renew ptable.minpriority.
		ptable.minpriority = minpriority;
	  }

      c->proc = 0;
    }
    release(&ptable.lock);
  }
}
```
 > - 만일 `selected`에 프로세스가 배정되었다면, 사전 작업을 수행하고 해당 프로세스에 제어권을 전달합니다. **(콘텍스트 스위칭)**
 > - 다시 `scheduler`로 복귀하였다면 수행한 프로세스의 `priority`를 갱신합니다.
 > - 마지막으로, 최소 `priority`를 검색하고 `ptable.minpriority`값을 갱신합니다.

--- 

 ```Makefile
 ifndef CPUS
# [os-prj3] change cpus to 1 for ssu scheduler
CPUS := 1
endif
 ```

 > 스케줄링 함수의 동작 과정을 잘 살펴보기 위해 `Makefile`에서 `CPUS`를 `1`로 설정했습니다.

---

## (2) 스케줄링 과정 확인을 위해 `sdebug` 명령어를 추가하기

> ![sdebug_flowchart](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj3/image02.png?raw=true)
>
> * `sdebug`의 flow chart는 위와 같습니다.
> * 반복문을 이용하여 `PNUM`만큼 프로세스를 만듭니다.
> * 생성된 자식 프로세스는 `weightset` 시스템콜로 프로세스의 가중치를 임의로 설정합니다. 
> * 이후 반복적으로 `counter` 변수를 증가시킵니다. 변수를 `PRINT_CYCLE`만큼 증가시켰다면, 프로세스의 ID, 가중치, 출력까지 소요된 시간을 출력합니다. 
> * 최종적으로 `TOTAL_COUNTER`만큼 증가시켰다면, 해당 프로세스는 종료됩니다. 
> * 부모 프로세스는 자식을 모두 생성했다면, `wait` 시스템콜을 이용하여 자식이 모두 종료될 때까지 기다립니다.
> * 인자로 들어온 `weight`값으로 프로세스의 가중치를 결정하는 `weightset` 시스템콜을 구현하기 위해 `proc.c`에 `do_weightset` 함수를 만들었습니다. 구현부는 아래와 같습니다.

`proc.c`에 추가한 **`do_weightset`**의 구현부
```c
/**
 [os-prj3]
  This is called by system call weightset.
  Set weight in caller's proc struct to w
  */
void do_weightset(unsigned long w){
  acquire(&ptable.lock);
  struct proc *p = myproc(); 
  p->weight = w;
  release(&ptable.lock);
}
```

`defs.h`에 추가한 **`do_weightset`**의 선언

```c
void	do_weightset(unsigned long w); // [os-prj3] for weightset system call
```

> - `do_weightset`은 `ptable`을 잠그고 해당 프로세스의 가중치를 인자값으로 할당합니다.
> - `sysproc.c`의 `sys_weightset`에서 이를 사용하기 때문에 `defs.h`에 선언도 추가하였습니다.
> - `sysproc.c`에서는 다음과 같이 `do_weightset`으로 `weight`을 설정합니다. `weight`이 0 이하이면 –1을 리턴하며 작업을 중지합니다.

`sysproc.c`의 **`sys_weightset`**의 구현부

```c
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
```

> 마지막으로 이 `weightset` 시스템콜 등록을 위한 코드들을 아래와 같이 `syscall.h`, `syscall.c`, `user.h`, `usys.S`, `Makefile`에 추가합니다.


### `syscall.c`
```c
extern int sys_weightset(void); // [os-prj3]
```
```c
[SYS_weightset]   sys_weightset, // [os-prj3]
```

### `syscall.h`
```c
// [os-prj3] 
#define SYS_weightset 22
```

### `user.h`
```c
int weightset(int weight); // [os-prj3] weight set sys call
```

### `usys.S`
```c
SYSCALL(weightset)
```

### `Makefile`
```Makefile
EXTRA=\
	mkfs.c ulib.c user.h cat.c echo.c forktest.c grep.c kill.c\
	ln.c ls.c mkdir.c rm.c stressfs.c usertests.c wc.c zombie.c\
	printf.c umalloc.c\
	sdebug.c\ # <--- this
	README dot-bochsrc *.pl toc.* runoff runoff1 runoff.list\
	.gdbinit.tmpl gdbutil\
```
```Makefile
UPROGS=\
	_cat\
	_echo\
	_forktest\
	_grep\
	_init\
	_kill\
	_ln\
	_ls\
	_mkdir\
	_rm\
	_sh\
	_stressfs\
	_usertests\
	_wc\
	_zombie\
	_sdebug\ # <--- this
```

### `sdebug.c`
```c
#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5
#define PRINT_CYCLE 10000000
#define TOTAL_COUNTER 500000000

void sdebug_func(void)
{
  int i;
  int pid;
  long start;
  volatile long counter = 0;

  printf(1, "start sdebug command\n");
  for(i = 1 ; i <= PNUM ; i++){
	pid = fork();
	start = uptime(); // [os-prj3] Check start time
	if(pid == 0){ // [os-prj3] Child process
	  if(weightset(i) < 0){
		printf(1, "PID:%d, weightset error\n", getpid());	
		exit();
	  } // [os-prj3] Set weight to i
	  /*
		 [os-prj3] Count PRINT_CYCLE
		 and print this process information.
	  */
	  while(counter < PRINT_CYCLE)
		++counter;
	  printf(1, "PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), i, (uptime()-start)*10);

	  /*
		 [os-prj3] Count TOTAL_COUNTER
		 and exit this process.
	  */
	  while(counter < TOTAL_COUNTER)
		++counter;
	  printf(1, "PID: %d terminated\n", getpid());
	  exit();
	}
	else if(pid > 0) // [os-prj3] Parent process
	  continue; // [os-prj3] Loop for making child process.
	else{ // Error
	  printf(1, "ERROR: fork\n");
	  break;
	}
  }
  for(i = 1 ; i <= PNUM ; i++) // [os-prj3] Wait childs.
	wait();

  printf(1, "end of sdebug command\n");
}

int main(void)
{
  sdebug_func();
  exit();
}

```
 > flow chart와 같이 자식 프로세스를 생성하여 `weightset`으로 `weight`을 지정하고, 프로세스 정보를 출력한 뒤 종료하는 코드를 `sdebug.c`에 작성했습니다. 

---

## (3) 다음에 실행될 프로세스 선정 과정 디버깅 기능 구현

 `make`에서 `debug=1` 옵션을 주면 프로세스 선정 과정을 확인할 수 있는 화면이 나오도록 구현하겠습니다.

### `Makefile`에 디버그 코드 추가
```Makefile
# [os-prj3] debug option
ifeq ($(debug), 1)
CFLAGS += -DDEBUG
endif
```

 > `Makefile`에서 `debug=1`일 때 `CFLAGS`에 `–DDEBUG`가 추가되도록 합니다. 컴파일할 때 C 소스의 처음에 `#define DEBUG`를 작성해주는 것과 같습니다.

### `console.c`에 `printul` 구현
```c
/*
   [os-prj3] function to print unsigned long integer in cernel
   */
void
printul(unsigned long x)
{
  static char digits[] = "0123456789abcdef";
  char buf[48];
  int i = 0;
  do{
    buf[i++] = digits[x % 10];
  }while((x /= 10) != 0);

  while(--i >= 0)
    consputc(buf[i]);
}
```

### `defs.h`에 `printul` 선언
```c
void	printul(unsigned long); // [os-prj3] for debug option
```

 > - `priority`를 부호 있는 integer 형식으로 출력하게 되면 오버플로우가 발생한 상태로 출력될 수 있습니다. 
 > - 이를 방지하기 위해 `unsigned long`을 출력할 수 있는 `printul` 함수를 `console.c`에 구현했습니다.
 > - 이후 `defs.h`에 함수의 선언을 작성했습니다.

### `proc.c`에 있는 `scheduler`에서 프로세스가 선택되었을 때 프로세스 정보를 출력
```c
if(selected != (void*)0){ // [os-prj3] If process is selected

		/**
		  [os-prj3] If it's debug mode, print process id, name, weight, priority.
		  */
#ifdef DEBUG
		cprintf("PID: %d, NAME: %s, WEIGHT: %d, PRIORITY: ",
				selected->pid, selected->name, selected->weight);
		printul(selected->priority);
		cprintf("\n");
#endif
```

 > - `proc.c`의 `scheduler`에서 프로세스가 선택되었을 때 해당 프로세스 정보를 출력하고, `PRIORITY`는 `printul`을 이용하여 출력합니다.

## (4) 최종 실행 결과

### `sdebug` 명령

> - ![sdebug](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj3/image002c.png?raw=true)
>
> - `sdebug` 명령이 잘 출력됩니다.

### `debug=1` 옵션 컴파일
> - ![debug1_1](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj3/image03.png?raw=true)
> - ![debug1_2](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj3/image002d.png?raw=true)
>
> - 잘 출력됩니다.

> - ![after_sdebug](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj3/image04.png?raw=true)
>
> - `sdebug` 명령 수행 후에도 `priority`가 오버플로없이 잘 출력됩니다.

---
# 4. 메모리 개선
 페이지 교체 알고리즘을 시뮬레이션하여 각 알고리즘의 성능을 비교하기 위해 별도로 프로젝트를 진행하였습니다.
 > 링크: [페이지 시뮬레이터](https://github.com/simjeehoon/page-simulator)

---
# 5. **XV6**의 더 효율적인 **파일 시스템**을 적용
 - XV6의 기존 파일 시스템에 새로운 파일 시스템을 추가합니다.
 - `./setup.sh 5`을 입력하시고 진행해주세요.

> ## CS 파일 시스템
> - 새로운 파일 시스템의 이름을 `CS 파일 시스템`이라고 명명하였습니다.
> - 파일에 연속적인 데이터 블록을 할당함으로써, 파일의 최대 크기를 늘리고 디스크 탐색시간을 단축하는 파일 시스템입니다.
> - XV6의 기존 파일 시스템은 그대로 유지하면서 CS 파일 시스템을 추가합니다.
> - `CS 파일 시스템`을 이용할 때, inode 구조체에 있는 direct block의 할당 방식을 수정할 것입니다.

 - `[os-prj5]` 주석을 달아 수정된 부분을 보기 쉽게 하였습니다.



## (1) 파일 생성 과정

> ![생성과정](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/image01.png?raw=true)
> 
> - `open`이 호출되는 과정입니다.
> - 시스템콜 `open`에 인자로 `O_CREATE`를 지정하면 파일이 생성됩니다.
> - `O_CS`라는 인자를 넘기면 **CS 파일**을 생성되도록 설계하겠습니다.

### fcntl.h
```h
#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_CS 0x020
```

### stat.h
```h
#define T_DIR  1   // Directory
#define T_FILE 2   // File
#define T_DEV  3   // Device
#define T_CS 4	   // Continuous Sector based File

struct stat {
  short type;  // Type of file
  int dev;     // File system's disk device
  uint ino;    // Inode number
  short nlink; // Number of links to file
  uint size;   // Size of file in bytes
};
```

> `fcntl.h`에 `O_CS` 플래그를, `stat.h`에 `T_CS` 매크로를 추가하였습니다.

### sysfile.c

```c
    if(omode & O_CREATE){
    // [os-prj5] CS 처리
	if(omode & O_CS)
	  ip = create(path, T_CS, 0, 0);
	else
	  ip = create(path, T_FILE, 0, 0);
	if(ip == 0){
	  end_op();
	  return -1;
	}
```

> `sysfile.c`에서 `O_CS` 플래그가 존재하면 `create`에 `type`으로 `T_CS`를 넘깁니다.


### `sysfile.c`의 `create`
```c
static struct inode*
create(char *path, short type, short major, short minor)
{
  struct inode *ip, *dp;
  char name[DIRSIZ];

  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);

  if((ip = dirlookup(dp, name, 0)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
	else if(type == T_CS && ip->type == T_CS)
	  // [os-prj5] CS 처리
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(dp->dev, type)) == 0)
    panic("create: ialloc");
```

> - `sysfile.c`에서 `T_CS`일 때 `T_FILE`과 같은 방식으로 처리했습니다.
> - `ialloc`에 `T_CS` 정보가 넘어가면, `type`이 `T_CS`인 `inode` 구조체가 생성됩니다.
> - `ialloc`의 내용은 `fs.c`에 있습니다.

### `sysfile.c` 의 `sys_open` 중 일부
```c
  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  return fd;
}
```
> - `sys_open`에서 생성된 `inode` 구조체는 `file` 구조체에 담깁니다.
> - 최종적으로 `sys_open`은 `file` 구조체를 가리키는 파일 디스크립터를 반환합니다.

---
## (2) 파일 쓰기 과정
![write_file](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/image02.png?raw=true)

 * 시스템콜 `write`를 이용하여 파일에 내용을 기록할 때 호출되는 순서를 표로 나타내었습니다.
 * `sys_write`, `filewrite`까지는 코드를 수정하지 않았습니다
 * `writei`부터는 코드 내용을 일부 수정했습니다. 파일이 **CS 파일 시스템**에 해당할 경우 다른 방식으로 쓰기 처리를 합니다.
 * **CS 파일 시스템**에서는 디스크 블록을 매핑할 때 오른쪽 노란색으로 칠해져 있는 함수들을 이용하고, 일반 파일 시스템에선 왼쪽의 매핑 함수들을 이용합니다.


### sysfile.c
```c
int
sys_write(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return filewrite(f, p, n); // <--this
}
```

### file.c
```c
int
filewrite(struct file *f, char *addr, int n)
{
  int r;

  if(f->writable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return pipewrite(f->pipe, addr, n);
  if(f->type == FD_INODE){
    // write a few blocks at a time to avoid exceeding
    // the maximum log transaction size, including
    // i-node, indirect block, allocation blocks,
    // and 2 blocks of slop for non-aligned writes.
    // this really belongs lower down, since writei()
    // might be writing a device like the console.
    int max = ((MAXOPBLOCKS-1-1-2) / 2) * 512;
    int i = 0;
    while(i < n){
      int n1 = n - i;
      if(n1 > max)
        n1 = max;

      begin_op();
      ilock(f->ip);
      if ((r = writei(f->ip, addr + i, f->off, n1)) > 0) // <--this
        f->off += r;
      iunlock(f->ip);
      end_op();
```

> - `sysfile.c`의 `sys_write`와 `file.c`의 `filewrite` 코드입니다.
> - `filewrite`에서 `writei`를 호출합니다.

### fs.c
```c
int
writei(struct inode *ip, char *src, uint off, uint n)
{
  uint tot, m;
  struct buf *bp;

  if(ip->type == T_DEV){
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].write)
      return -1;
    return devsw[ip->major].write(ip, src, n);
  }

  if(off > ip->size || off + n < off)
    return -1;

  // [os-prj5] writei for cs
  if(ip->type == T_CS){
	for(tot=0; tot<n; tot+=m, off+=m, src+=m){
	  bp = bread(ip->dev, bcsmap(ip, off, n-tot)); //<--this
	  m = min(n - tot, BSIZE - off%BSIZE);
	  memmove(bp->data + off%BSIZE, src, m);
	  log_write(bp);
	  brelse(bp);
	}
  }
  else {
	if(off + n > MAXFILE*BSIZE)
	  return -1;

	for(tot=0; tot<n; tot+=m, off+=m, src+=m){
	  bp = bread(ip->dev, bmap(ip, off/BSIZE)); //<--this
	  m = min(n - tot, BSIZE - off%BSIZE);
	  memmove(bp->data + off%BSIZE, src, m);
	  log_write(bp);
	  brelse(bp);
	}
  }
```
 > - `fs.c`의 `writei`입니다. 
 > - 기존 파일 시스템에서는 `bmap`을 이용하여 파일의 `offset`에 따라 디스크 블록을 할당합니다. 
 > - `bmap`은 필요시 디스크 블록을 할당하고, 할당된 블록 주소를 반환하는 함수입니다. 
 > - 이 블록 주소를 `bread`에 넘겨 디스크 블록 정보가 담긴 `struct buf`를 얻어옵니다. 
 > - 이곳에 `src`의 내용을 기록하고 `log_write`와 `brelse`를 호출하여 쓰기 작업을 마칩니다.

 > - **CS 파일 시스템**을 구현하기 위해서 `inode`의 `type`이 `T_CS`인 경우와 아닌 경우로 흐름을 나누었습니다. 
 > - `T_CS`일 때는?
 >    -  **CS 파일 시스템**을 위한 블록 매핑 함수인 `bcsmap` 함수를 호출합니다. 
 >    - `bcsmap`은 필요시 디스크 블록을 추가로 할당합니다. 그리하여 데이터가 쓰일 단일 블록 주소를 반환합니다. 
 >    - 이 주소는 다시 `bread`의 인자로 넘깁니다. 이후 과정은 기존 파일 시스템과 동일합니다.

### fs.c
```c
// If there is no such block, bmap allocates one.
static uint
bmap(struct inode *ip, uint bn)
{
  uint addr, *a;
  struct buf *bp;

  if(bn < NDIRECT){
    if((addr = ip->addrs[bn]) == 0)
      ip->addrs[bn] = addr = balloc(ip->dev);
    return addr;
  }
  bn -= NDIRECT;

  if(bn < NINDIRECT){
    // Load indirect block, allocating if necessary.
    if((addr = ip->addrs[NDIRECT]) == 0)
      ip->addrs[NDIRECT] = addr = balloc(ip->dev);
    bp = bread(ip->dev, addr);
    a = (uint*)bp->data;
    if((addr = a[bn]) == 0){
      a[bn] = addr = balloc(ip->dev);
      log_write(bp);
    }
    brelse(bp);
    return addr;
  }

  panic("bmap: out of range");
}
```

> - 기존 파일 시스템이 사용하는 `bmap`은 구조가 단순합니다. 
> - `inode`의 `addrs`에 블록 포인터가 없다면 `balloc`을 이용해 새로 할당하고, 그것을 반환합니다.
> - 이미 존재하면 존재하는 블록의 포인터를 반환합니다.
> - 만일 파일 크기가 커질 경우 `INDIRECT` 포인터를 위한 디스크 블록을 할당하고, 그 블록 내부에 블록 포인터를 저장하고 반환합니다.
>
> - 블록 할당에 이용하는 함수는 `balloc`입니다. 이 함수는 디스크에 유효한 블록 1개를 찾아서 반환합니다.

### fs.c
```c
// [os-prj5] CS를 위한 블록 매핑 함수
static uint
bcsmap(struct inode *ip, uint off, uint n)
{
  uint staddr;
  uint prevaddr;
  uint alloclen;

  uint bn;
  uint position = off/BSIZE;
  uint accum;
  uint seqlen;
  uint needlen = n % BSIZE ? n / BSIZE + 1 : n / BSIZE;

  for(bn = 0, accum = 0 ; bn < NDIRECT && ip->addrs[bn] ; bn++){
	accum += ip->addrs[bn] & 255; // [os-prj5] accum 변수에 길이를 더함
	if(accum > position){ // [os-prj5] 위치 발견
	  accum -= ip->addrs[bn] & 255;
	  return (ip->addrs[bn] >> 8) + (position - accum); // [os-prj5] 블럭의 위치를 리턴
	}
  }

  if(bn != 0){ // [os-prj5] 마지막 addrs 확장
	accum -= ip->addrs[bn-1] & 255; 
	staddr = ip->addrs[bn-1] >> 8;
	alloclen = ip->addrs[bn-1] & 255;
	if(alloclen < 255){ // [os-prj5] 255 미만일때만 확장 가능
	  prevaddr = staddr+alloclen - 1;
	  int b;
	  if(255-alloclen < needlen)
		b = bcsalloc(ip->dev, prevaddr, 255-alloclen, &seqlen);
	  else
		b = bcsalloc(ip->dev, prevaddr, needlen, &seqlen);
	  if(b != -1){ // [os-prj5] 확장 성공
		alloclen += seqlen;
		ip->addrs[bn-1] = (staddr << 8) | (alloclen & 255);
		if(accum + alloclen > position){ // [os-prj5] 데이터를 위치시킬 공간을 얻었다면
		  return staddr + (position - accum);
		}
	  }
	}
	accum += alloclen;
  }

  if(bn >= NDIRECT){ // [os-prj5] 블록을 더이상 할당 불가능
	panic("CS : no more block in inode.");
	return -1;
  }

  if(needlen <= 255) // [os-prj5] 최대 연속 할당 길이는 255
	staddr = bcsalloc(ip->dev, -1, needlen, &seqlen); 
  else
	staddr = bcsalloc(ip->dev, -1, 255, &seqlen); 
  alloclen = seqlen;
  ip->addrs[bn] = (staddr << 8) | (alloclen & 255);
  return staddr + (position - accum);
}
```

 > - **CS 파일 시스템**에서 사용하는 `bcsmap`의 흐름은 `bmap`과 다르게 동작합니다.
 > - `inode` 구조체 내 `addrs`의 각 원소를 해석하는 방법이 다르고, 블록을 할당하는 방식이 다르기 때문입니다.

 > - * **CS 파일 시스템**에서 `addrs`의 각 원소를 해석할 때, **상위 3바이트**는 블록의 주소, **하위 1바이트**는 연속된 블록의 개수를 의미합니다. 

 >>  - 이를 이용하여, 인자로 넘겨받은 `offset`이 몇 번째 블록에 해당하는지 `addrs[0]`부터 순차적으로 탐색합니다.
 >>  - 만일 `offset`이 위치한 단일 블록이 이미 존재하면, 해당 블록의 주소를 반환합니다.

 > 블록이 존재하지 않는 경우, 처리하는 방법은 2가지입니다. 마지막 `addrs`의 원소가 가리키는 블록의 길이를 확장하거나, 아예 새로운 블록을 할당하는 것입니다.
 >
 >>   * 전자의 방법을 통하여 확장에 실패하는 경우는 연속 할당에 실패하거나, 블록 길이가 255를 초과하는 경우입니다.
 >>
 >>   * 할당에 이용하는 함수는 `bcsalloc`입니다. 이 함수에 이전 블록 주소와 필요한 공간의 길이를 인자로 넘겨, 새로 할당한 공간의 시작 주소와 새로 할당한 영역의 길이를 반환합니다. 
 >>
 >>   * 필요한 공간의 길이는 `bcsmap`의 3번째 인자 n을 이용하여 구합니다. n은 실제로 필요한 데이터 용량입니다. 
 >>
 >>   * 이전 블록 주소와 연속적인 블록을 할당할 수 없다면 `bcsalloc`은 `–1`를 반환합니다.
 >
 > * 연속 할당에 실패하여 `addrs`에 새로운 값을 할당하는 경우엔 이전 블록 주소를 `–1`로 설정하여 넘깁니다.
 >
 >   * 그러면 `bcsalloc`은 이전 블록과의 연속성을 고려하지 않습니다.
 >
 > * 두 방법 모두 실패했다면 `panic`을 호출합니다.

 
 블록 할당을 비교하기 위해 `balloc`과 `bcsalloc`을 분석하였습니다.

### fs.c
```c
// Allocate a zeroed disk block.
static uint
balloc(uint dev)
{
  int b, bi, m;
  struct buf *bp;

  bp = 0;
  for(b = 0; b < sb.size; b += BPB){
    bp = bread(dev, BBLOCK(b, sb));
    for(bi = 0; bi < BPB && b + bi < sb.size; bi++){
      m = 1 << (bi % 8);
      if((bp->data[bi/8] & m) == 0){  // Is block free?
        bp->data[bi/8] |= m;  // Mark block in use.
        log_write(bp);
        brelse(bp);
        bzero(dev, b + bi);
        return b + bi;
      }
    }
    brelse(bp);
  }
  panic("balloc: out of blocks");
}
```

 * `balloc`에서는 가장 먼저 블록의 할당 정보 비트를 읽습니다. 이 데이터를 이용하여 할당할 수 있는 블록을 순차적으로 찾습니다. 
 * 할당 가능한 블록을 발견하였다면 해당 블록을 0으로 초기화하고, 블록 주소를 반환합니다.

### fs.c
```c
// [os-prj5] CS를 위한 block 할당 함수
static uint
bcsalloc(uint dev, int prevbnum, uint needlen, uint *alloclen)
{
  int b, bi, m;
  struct buf *bp;
  bp = 0;
  if(prevbnum == -1){ // [os-prj5] 완전히 새로운 영역을 할당하려고 할때
	b=0;
	bi=0;
  }
  else{ // [os-prj5] 이전 영역에 이어서 할당하려고 할 때
	int nextbnum = prevbnum+1;
	b = nextbnum / BPB;
	bi = nextbnum % BPB;
  }
  int staddr = -1;
  int length = 0;
  int modified;
  while(b < sb.size){
    bp = bread(dev, BBLOCK(b, sb));
	modified = 0;
	while(bi < BPB && b+bi < sb.size){
      m = 1 << (bi % 8);
	  if(prevbnum == -1) {  // [os-prj5] 완전히 새로운 영역을 할당하는 경우
		if((bp->data[bi/8] & m) == 0) {  // [os-prj5] Is block free?
		  if(staddr == -1)
			staddr = b+bi;
		  bp->data[bi/8] |= m;  // [os-prj5]  Mark block in use.
		  modified = 1;
		  length++;
		  if(needlen <= length){
			log_write(bp);
			brelse(bp);
			for(int i = staddr; i < b + bi; i++)
			  bzero(dev, i);
			*alloclen = length;
			return staddr;
		  }
		}
		else if(staddr != -1){ // [os-prj5] end sequence
		  log_write(bp);
		  brelse(bp);
		  for(int i = staddr; i < b + bi; i++)
			bzero(dev, i);
		  *alloclen = length;
		  return staddr;
		}
	  }
	  else { // [os-prj5] 이전 공간에 이어서 할당하는 경우
		if((bp->data[bi/8] & m) == 0) {  //  [os-prj5] Is block free?
		  if(staddr == -1)
			staddr = b+bi;
		  bp->data[bi/8] |= m;  // [os-prj5]  Mark block in use.
		  modified = 1;
		  length++;
		  if(needlen <= length){
			log_write(bp);
			brelse(bp);
			for(int i = staddr; i < b + bi; i++)
			  bzero(dev, i);
			*alloclen = length;
			return staddr;
		  }
		}
		else{ // [os-prj5] not free
		  if(staddr == -1){ // [os-prj5] not sequence
			brelse(bp);
			return -1;
		  }
		  else{  // [os-prj5] sequence
			log_write(bp);
			brelse(bp);
			for(int i = staddr; i < b + bi; i++) 
			  bzero(dev, i);
			*alloclen = length;
			return staddr;
		  }
		}
	  }
	  ++bi;
	}
	if(modified){ //[os-prj5] 변경 사항 적용
	  log_write(bp);
	  brelse(bp);
	  for(int i = staddr; i < b + bi; i++)
		bzero(dev, i);
	}
	else{
	  brelse(bp);
	}
	b += BPB;
	bi=0;
  }
  if(staddr == -1){ // [os-prj5] 디스크가 꽉참
	panic("CS : no more free block");
	return -1;
  }
  else{  // [os-prj5] 디스크가 꽉 찼고, 할당한 것 반환
	*alloclen = length;
	return staddr;
  }
}

```

 - `bcsalloc`의 블록 탐색 방식은 `balloc`과 동일하지만, 연속된 블록을 반환하여야 하므로 `balloc`보다 코드가 길어집니다.

 * 이전 블록 주소인 `prevbnum`이 `–1`이 아닌 값으로 설정되었다면, `prevbnum`의 다음 블록 주소로부터 b와 bi 값을 얻어냅니다. 이 주소로부터 `needlen`만큼의 연속으로 할당 가능한 블록을 탐색합니다. 
 * 만일 `prevbnum` 다음 블록이 할당 불가능하다면 `–1`을 반환합니다. 
 * `needlen`만큼 연속된 블록들을 찾았거나, 탐색 과정중 할당 불가능한 블록을 마주하게 되었다면, 연속적으로 할당한 블록의 길이와 새로 할당한 블록 중 가장 처음 블록의 주소를 반환합니다. 

 - `prevbnum`이 `–1`로 설정된 경우에는 더 간단합니다. 가장 먼저 할당 가능한 블록을 찾고, 시작 블록으로 설정합니다. 
 - 이후에 그 시작 블록으로부터 `needlen`만큼 연속으로 할당 가능한 블록들을 탐색합니다. 
 - `needlen`만큼 연속된 블록들을 찾았거나, 탐색 과정중 할당 불가능한 블록을 마주하게 되었다면, 연속적으로 할당한 블록의 길이와 새로 할당한 블록 중 가장 처음 블록의 주소를 반환합니다. 

 * 블록의 할당 정보 비트가 담긴 블록이 수정되었다면 `log_write`와 `brelse`를 순차적으로 호출합니다. 
 * 만일 수정되지 않았다면 `brelse`만 호출합니다.

---

## (3) 파일 읽기 과정
![read_file](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/image04.png?raw=true)

 * 시스템콜 `read`를 이용하여 파일의 내용을 읽을 때의 과정을 표로 나타냈습니다. 
 * 쓰기와 마찬가지로 `sys_read`, `fileread`까지는 코드를 수정하지 않았습니다. 
 * `readi`부터는 코드 내용을 일부 수정했습니다. 
 * 파일이 **CS 파일 시스템**에 해당할 경우 다른 처리를 합니다. **CS 파일 시스템**에서 호출하는 디스크 블록 매핑 함수는 `bcsmap`, 일반 파일 시스템에서의 함수는 `bmap`입니다.

### sysfile.c
```c
int
sys_read(void)
{
  struct file *f;
  int n;
  char *p;

  if(argfd(0, 0, &f) < 0 || argint(2, &n) < 0 || argptr(1, &p, n) < 0)
    return -1;
  return fileread(f, p, n); //<--this
}
```

### file.c
```c
int
fileread(struct file *f, char *addr, int n)
{
  int r;

  if(f->readable == 0)
    return -1;
  if(f->type == FD_PIPE)
    return piperead(f->pipe, addr, n);
  if(f->type == FD_INODE){
    ilock(f->ip);
    if((r = readi(f->ip, addr, f->off, n)) > 0) // <--this
      f->off += r;
    iunlock(f->ip);
    return r;
  }
  panic("fileread");
}
```

> `sysfile.c`의 `sys_read`와 `file.c`의 `fileread` 코드입니다. `fileread`에서 `readi`를 호출합니다.

### fs.c
```c
int
readi(struct inode *ip, char *dst, uint off, uint n)
{
  uint tot, m;
  struct buf *bp;

  if(ip->type == T_DEV){
    if(ip->major < 0 || ip->major >= NDEV || !devsw[ip->major].read)
      return -1;
    return devsw[ip->major].read(ip, dst, n);
  }

  if(off > ip->size || off + n < off)
    return -1;
  if(off + n > ip->size)
    n = ip->size - off;

  // [os-prj5] readi for cs
  if(ip->type == T_CS){
	for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
	  bp = bread(ip->dev, bcsmap(ip, off, n-tot));
	  m = min(n - tot, BSIZE - off%BSIZE);
	  memmove(dst, bp->data + off%BSIZE, m);
	  brelse(bp);
	}
  }
  else{
	for(tot=0; tot<n; tot+=m, off+=m, dst+=m){
	  bp = bread(ip->dev, bmap(ip, off/BSIZE));
	  m = min(n - tot, BSIZE - off%BSIZE);
	  memmove(dst, bp->data + off%BSIZE, m);
	  brelse(bp);
	}
  }
  return n;
}
```

> * 수정한 `fs.c`의 `readi` 코드입니다. `inode`의 `type`이 `T_CS`일 때의 처리가 **CS 파일시스템** 처리입니다. 
> * `writei`와 반대로 디스크의 내용을 `dst`로 복사합니다. 따라서 블록 매핑 함수를 그대로 이용하면 됩니다.

> * `bcsmap`의 경우 `off`에 해당하는 블록이 이미 있을 경우 해당 블록의 주소를 반환합니다. 
> * 따라서 `readi`가 이상 없이 작동합니다.

---
## (4) 파일 제거 과정
 ![delete_file](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/image03.png?raw=true)

 * 시스템콜 `unlink`를 이용하여 파일을 제거하는 과정을 표로 나타냈습니다. 
 * `iput`에서 `inode`의 `ref`가 1일 경우 `itrunc`를 호출하여 파일을 삭제합니다. 
 * 따라서 `itrunc`에서 **CS 파일 시스템**을 위한 제거 과정을 추가하면, **CS 파일**을 삭제할 수 있게 됩니다.

 ![code_delete](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/image05.png?raw=true)

 위의 사진은 삭제 과정의 코드들을 나타낸 것입니다.

### `fs.c`
```c
 static void
itrunc(struct inode *ip)
{
  int i, j;
  struct buf *bp;
  uint addr, length;
  uint *a;
  if(ip->type == T_CS){ // [os-prj5] CS 시스템 전용 삭제 처리
	for(i = 0; i < NDIRECT; i++){
	  if(ip->addrs[i]){
		addr = ip->addrs[i] >> 8;
		length = ip->addrs[i] & 255;
		for(j = 0 ; j < length ; j++)
		  bfree(ip->dev, addr+j);
		ip->addrs[i] = 0;
	  }
	}
	ip->size = 0;
	iupdate(ip);
  }
  else{
	for(i = 0; i < NDIRECT; i++){
	  if(ip->addrs[i]){
		bfree(ip->dev, ip->addrs[i]);
		ip->addrs[i] = 0;
	  }
	}

	if(ip->addrs[NDIRECT]){
	  bp = bread(ip->dev, ip->addrs[NDIRECT]);
	  a = (uint*)bp->data;
	  for(j = 0; j < NINDIRECT; j++){
		if(a[j])
		  bfree(ip->dev, a[j]);
	  }
	  brelse(bp);
	  bfree(ip->dev, ip->addrs[NDIRECT]);
	  ip->addrs[NDIRECT] = 0;
	}

	ip->size = 0;
	iupdate(ip);
  }
}
```
 > - `itrunc`에서 `inode`의 `type`이 `T_CS`일 경우 **CS 파일 시스템**에 맞게 삭제합니다.
 > - 각 `inode`의 `addr`로부터 블록의 시작 주소와 연속된 블록 개수를 추출하고, 연속된 블록에 대해 `bfree`를 호출합니다.

---

## (5) `printinfo` 구현
 파일 정보를 출력하는 `printinfo`를 시스템콜로 구현하였습니다.
```c
//[os-prj5] printinfo
int
sys_printinfo(void)
{
  struct file *f;
  char *fname;
  if(argfd(0, 0, &f) < 0 || argstr(1, &fname) < 0)
    return -1;
  
  int num, length;
  struct inode *ip = f->ip;
  const char *typestr[] = {"X", "DIR", "FILE", "DEV", "CS"};
  cprintf("FILE NAME: %s\n", fname);
  cprintf("INODE NUM: %d\n", ip->inum);
  cprintf("FILE TYPE: %s\n", typestr[ip->type]);
  cprintf("FILE SIZE: %d Bytes\n", ip->size);
  cprintf("DIRECT BLOCK INFO:\n");
  if(ip->type == T_FILE){
	for(int i = 0 ; i < NDIRECT ; i++){
	  if(ip->addrs[i])
		cprintf("[%d] %d\n",i,ip->addrs[i]);
	}
  }
  else if(ip->type == T_CS){
	for(int i = 0 ; i < NDIRECT ; i++){
	  if(!ip->addrs[i])
		continue;
	  num = ip->addrs[i] >> 8;
	  length = ip->addrs[i] & 255;
	  cprintf("[%d] %d (num: %d, length: %d)\n",
		i, ip->addrs[i], num, length);
	}
  }
  cprintf("\n");
  return 0;
}
```

> 위와 같이 `printinfo`를 호출하면 
> * 파일 이름
> * inode 번호
> * 파일 타입
> * 파일 크기
> * DIRECT BLOCK 정보
> 
> 가 출력됩니다.
 
 - 기존 파일 시스템의 경우 `DIRECT BLOCK`의 정보로써 사용하고 있는 `block`에 저장된 내용이 출력되고,
 - **CS 파일 시스템**일 경우 내용뿐만 아니라 블록 시작 번호, 연속된 블록 개수 정보가 같이 출력됩니다.

### syscall.c
```c
extern int sys_printinfo(void); // [os-prj5] printinfo
```

```c
[SYS_printinfo]   sys_printinfo,  // [os-prj5] printinfo
```

 > `printinfo` 시스템콜을 추가하기 위해 `syscall.c`에 해당 시스템콜을 위한 코드를 추가했습니다.

### syscall.h
```h
#define SYS_printinfo  22
```

### user.h
```h
int printinfo(int, const char*); // [os-prj5] printinfo
```

### usys.S
```S
SYSCALL(printinfo)
```
 >  `syscall.h`, `user.h`, `usys.S`에도 `printinfo` 시스템콜을 위한 코드를 추가했습니다.

---
## (6) 테스트 파일 실행
### A. 연속적으로 데이터 쓰기

 - CS 기반 파일을 생성하여 데이터를 1024B씩 130번 `write` 합니다. 
 - 이후 이 파일에 대해 `printinfo`를 호출합니다.

![ftesta](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p1.png?raw=true)

 > 우측 사진에서 보듯, 2개의 **DIRECT BLOCK**을 사용하여 할당한 것을 확인할 수 있습니다.

### B. 불연속적으로 데이터 쓰기

 - 1과 마찬가지로 CS 기반 파일을 생성하여 데이터를 1024B씩 130번 write합니다. 
 - 단, 51번째에 기존 파일 시스템 기반 파일을 생성하여 데이터를 1024B씩 2번 write한 뒤 이 파일에 대해 `printinfo`를 호출합니다. 
 - CS파일도 130번 write를 한 뒤 `printinfo`를 호출합니다. 

![ftestb](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p2.png?raw=true)

 - 우측 사진에서 보듯, **일반 파일**은 4개의 `DIRECT` 블록이 할당되었습니다. 이는 2048B만큼 데이터를 `write`하였기 때문입니다. 
 - **CS 기반 파일**은 1번부터 51번째까지 총 51KB를 write했습니다. 따라서 0번째 `direct block`에 102개의 연속 블록이 할당되었습니다. 이후 새로운 `direct block`을 할당받아 52번부터 130번까지 총 79KB를 write했습니다. 따라서 158개의 연속 블록이 할당되었습니다.

![ftestb2](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p3.png?raw=true)

 > `ls`와 `wc` 명령어를 사용하여 파일의 크기를 확인하였습니다. 모두 정상적으로 쓰기 작업이 수행되었습니다.

### C. 파일 읽기

![ftestc](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p4.png?raw=true)

 > 정상적으로 읽히는 것을 확인할 수 있었습니다.

### D. 파일 제거

![ftestd](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p5.png?raw=true)
 > 정상적으로 제거되는 것을 확인할 수 있었습니다.

### E. 할당할 수 있는 데이터 블록이나 direct 블록의 범위를 초과할 경우 범위 내까지만 데이터 할당 후 에러 메시지 출력

 - 할당할 수 있는 데이터 블록을 초과한 경우 `bcsalloc`에서 `panic`을 호출합니다.

   - ![fteste](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p6.png?raw=true)

 - 또는 CS 파일 시스템의 블록 포인터를 모두 사용해서 더 이상 할당이 불가능하면 `bcsmap`에서 다음과 같이 `panic`이 호출됩니다.

   - ![fteste2](https://github.com/simjeehoon/src_repository/blob/master/xv6-public/os-prj5/p7.png?raw=true)
