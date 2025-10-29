#include "types.h"
#include "stat.h"
#include "user.h"
#include "procstat.h"

#define PNUM 5
#define PRINT_CYCLE   10000000
#define TOTAL_COUNTER 500000000

#define STDOUT_BUF_SIZE 1024
#define STDOUT 1

char *scheduler_name = "SSU scheduler";

typedef struct process_data{
    int pid;
    struct proc_stat proc_stat;
    uint tt, wt, rt;
} Pdata;

Pdata pdata[PNUM];

// [os-prj3] 종합 통계 변수
uint avg_waiting_time = 0;
uint avg_turnaround_time = 0;
uint avg_response_time = 0;
uint total_cpu_time = 0;

// [os-prj3] 총 시간 측정을 위한 변수
int flag_min = 1;
uint min_arrival_time = 0; // 가장 먼저 시작한 프로세스의 arrival_time
uint max_completion_time = 0; // 가장 늦게 종료된 프로세스의 completion_time

// [os-prj3] console에 여러 프로세스의 문자가 뒤섞여서 출력되는것을 막고자
// 자체 출력 함수를 개발했습니다.

static char stdout_buf[STDOUT_BUF_SIZE];
static int stdout_i = 0;

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



// [os-prj3] 프로세스 출력 함수
void print_each_process(Pdata *pdata)
{
    // [os-prj3] 개별 프로세스의 상세 결과 출력
    so_printf(" PID:%d, WEIGHT:%d | AT:%u, CT:%u, CPUT:%u, FT:%u | TT:%u, WT:%u, RT:%u\n",
        pdata->pid, 
        pdata->proc_stat.weight, 
        pdata->proc_stat.arrival_time, pdata->proc_stat.completion_time, 
        pdata->proc_stat.cpu_time, pdata->proc_stat.first_run_time,
        pdata->tt, pdata->wt, pdata->rt
    );
    so_flush();
}

void print_statistics()
{
    so_printf("%s Performance Analysis\n", scheduler_name);
    so_printf("1. Avg. Turnaround Time: %u ticks\n", avg_turnaround_time);
    so_printf("2. Avg. Waiting Time: %u ticks\n", avg_waiting_time);
    so_printf("3. Avg. Response Time: %u ticks\n", avg_response_time);
    so_printf("4. Total CPU Time: %u ticks\n", total_cpu_time);
    
    // 전체 경과 시간 계산
    uint total_elapsed_time = max_completion_time - min_arrival_time;
    so_printf("5. Total Elapsed Time: %u ticks\n", total_elapsed_time);
    so_flush();
}

void sdebug_func(void)
{
    int i;
    int pid;
    long start;
    
    volatile long counter = 0;
    
    int terminated_pid; // 종료 프로세스의 pid
    
    struct proc_stat proc_stat; // 프로세스 상태

    int weight;

    so_printf("Start sdebug command - %s\n", scheduler_name);
    so_flush();
    for(i = 0 ; i < PNUM ; i++){
        pid = fork();
        start = uptime(); 
        if(pid == 0){ // [os-prj3]  자식 프로세스
            weight = i+1;

            // [os-prj3] Set weight to i
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
            so_printf("[Check] PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), weight, (uptime()-start)*10);
            so_flush();

            while(counter < TOTAL_COUNTER)
              ++counter;
            so_printf("[Terminated] PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), weight, (uptime()-start)*10);

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

    
    // [os-prj3] 부모 프로세스의 통계 수집 및 계산

    uint turnaround, waiting, response; // 시간 변수

    for(i = 0 ; i < PNUM ; i++) {
        terminated_pid = waitx(&proc_stat); // 자식이 종료될때까지 대기하고 proc_stat을 받음

        if (terminated_pid > 0) {
            // 1. 반환 시간 (Turnaround Time): 종료 시간 - 도착 시간
            turnaround = proc_stat.completion_time - proc_stat.arrival_time;
            // 2. 대기 시간 (Waiting Time): 반환 시간 - CPU 사용 시간
            waiting = turnaround - proc_stat.cpu_time;
            // 3. 응답 시간 (Response Time): 첫 실행 시간 - 도착 시간
            response = proc_stat.first_run_time - proc_stat.arrival_time;

            // 총 시간 추적
            if (flag_min || proc_stat.arrival_time < min_arrival_time) {
                min_arrival_time = proc_stat.arrival_time;
                flag_min = 0;
            }
            if (proc_stat.completion_time > max_completion_time) {
                max_completion_time = proc_stat.completion_time;
            }

            / 통계 누적
            avg_turnaround_time += turnaround / PNUM;
            avg_waiting_time += waiting / PNUM;
            avg_response_time += response / PNUM;
            total_cpu_time += proc_stat.cpu_time;
            
            // 값 저장
            pdata[i].pid = terminated_pid;
            pdata[i].proc_stat = proc_stat;
            pdata[i].tt = turnaround;
            pdata[i].wt = waiting;
            pdata[i].rt = response;
        }
    }

    // [os-prj3] 최종 평균 성능 지표 출력
    so_printf("Individual Process Result of %s\n", scheduler_name);
    for(i=0 ; i < PNUM ; i++)
        print_each_process(pdata+i);
    so_printf("--------------------------------------------------------\n");
    print_statistics();
    so_printf("--------------------------------------------------------\n");

    so_printf("End of sdebug command - %s\n", scheduler_name);
    so_flush();
}


int main(void)
{
  sdebug_func();
  so_flush();
  exit();
}
