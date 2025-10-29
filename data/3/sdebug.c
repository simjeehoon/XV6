#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 10
#define PRINT_CYCLE   10000000
#define TOTAL_COUNTER 500000000

// [os-prj3] 스케줄러 이름
char *scheduler_name = "SSU scheduler";

/**
 * [os-prj3] 출력 함수 정의
 * console에 여러 프로세스의 문자가 뒤섞여서 출력되는것을 막고자
 * 자체 출력 함수를 개발했습니다.
 */

#define STDOUT_BUF_SIZE 1024
#define STDOUT 1

static char stdout_buf[STDOUT_BUF_SIZE];  // [os-prj3] 출력버퍼
static int stdout_i = 0;                  // [os-prj3] 출력버퍼 인덱스

// [os-prj3] 비로소 console에 출력해주는 함수. 프로그램 종료 전 반드시 호출되어야 함.
static void 
so_flush(){
    write(STDOUT, stdout_buf, sizeof(char) * stdout_i);
    stdout_i = 0;
}

// [os-prj3] 글자 1개를 STDOUT 버퍼에 넘기는 함수
static void
so_putc(char c)
{
    if(stdout_i >= STDOUT_BUF_SIZE){ // [os-prj3] 출력버퍼 비우기
        so_flush();
    }
    stdout_buf[stdout_i++] = c;
}

static void
so_printint(int xx, int base, int sgn)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    so_putc(buf[i]);
}

/*
   [os-prj3] function to print unsigned long integer in cernel
   */
static void
so_printuint(uint x, uint base)
{
  static char digits[] = "0123456789ABCDEF";
  char buf[48];
  int i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);

  while(--i >= 0)
    so_putc(buf[i]);
}

// Print to the given fd. Only understands %d, %x, %p, %s.
void
so_printf(const char *fmt, ...)
{
  char *s;
  int c, i, state;
  uint *ap;

  state = 0;
  ap = (uint*)(void*)&fmt + 1;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        so_putc(c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        so_printint(*ap, 10, 1);
        ap++;
      } else if(c == 'u'){
        so_printuint(*ap, 10u);
        ap++;
      } else if(c == 'x' || c == 'p'){
        so_printint(*ap, 16, 0);
        ap++;
      } else if(c == 's'){
        s = (char*)*ap;
        ap++;
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          so_putc(*s);
          s++;
        }
      } else if(c == 'c'){
        so_putc(*ap);
        ap++;
      } else if(c == '%'){
        so_putc(c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        so_putc('%');
        so_putc(c);
      }
      state = 0;
    }
  }
}

void sdebug_func(void)
{
    int i;
    int pid;
    
    volatile long counter = 0;
    
    int terminated_pid; // 종료 프로세스의 pid

    int weight;

    so_printf("Start sdebug command - %s\n", scheduler_name);
    so_flush();

    for(i = 0 ; i < PNUM ; i++){
        pid = fork();

        if(pid == 0){ // [os-prj3]  자식 프로세스
          // [os-prj3] 시작시간 측정
          so_printf("[Start] PID: %d, TIME:%d\n", 
            getpid(), uptime());
          so_flush();
          
          // [os-prj3] Set weight to i+1
          weight = i+1;
          if(weightset(weight) < 0){
            so_printf("PID:%d, weightset error\n", getpid());
                    so_flush();	
            exit();
          } 
			
          /*
            [os-prj3] Count PRINT_CYCLE
            and print this process information.
          */
            
          while(counter < PRINT_CYCLE)
            ++counter;

          // [os-prj3] 체크시간 측정
          so_printf("[Print] PID: %d, TIME:%d\n", 
            getpid(), uptime());
          so_flush();

          while(counter < TOTAL_COUNTER)
            ++counter;
          
          // [os-prj3] 종료시간 측정
          so_printf("[Terminated] PID: %d, TIME:%d\n", 
            getpid(), uptime());
          so_flush();

          exit();
        }
        else if(pid > 0) // [os-prj3]  부모 프로세스
          continue;
        else{
          so_printf("ERROR: fork\n");
          break;
        }
    }
    for(i = 0 ; i < PNUM ; i++) {
      terminated_pid = wait(); // 자식이 종료될 때까지 대기하고 PID를 받음
      if(terminated_pid < 0){
        so_printf("wait error");
      }
    }
}


int main(void)
{
  sdebug_func();
  so_flush();
  exit();
}
