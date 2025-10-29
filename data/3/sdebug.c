#include "types.h"
#include "stat.h"
#include "user.h"
#include "procstat.h"

#define PNUM 20
#define PRINT_CYCLE   100000000
#define TOTAL_COUNTER 500000000

char *scheduler_name = "SSU scheduler"

typedef struct process_stat{
    int pid, weight;
    int at, ct, cput, ft;
    int tt, wt, rt;
}Pstat;

Pstat pstat[PNUM];

// [os-prj3] 종합 통계 변수
int total_waiting_time = 0;
int total_turnaround_time = 0;
int total_response_time = 0;
int total_cpu_time = 0;
int success_count = 0;

// [os-prj3] 총 시간 측정을 위한 변수
int min_arrival_time = -1; // 가장 먼저 시작한 프로세스의 arrival_time
int max_completion_time = 0; // 가장 늦게 종료된 프로세스의 completion_time

void print_each_process()
{
    int i;
    printf("Individual Process Result of %s\n", scheduler_name);
    for(i=0;i<PNUM;i++){
        // [os-prj3] 개별 프로세스의 상세 결과 출력
        printf(1, " PID:%d WEIGHT:%d | AT:%d, CT:%d, CPUT:%d, FT:%d | TT:%d, WT:%d, RT:%d\n",
            pstat[i].pid, 
            pstat[i].weight, 
            pstat[i].at, 
            pstat[i].ct, 
            pstat[i].cput, 
            pstat[i].ft,
            pstat[i].tt, 
            pstat[i].wt, 
            pstat[i].rt);                    
    }
}

void print_statistics()
{
    printf(1, "%s Performance Analysis (number of process:%d)", scheduler_name, success_count);
    printf(1, "1. Avg. Turnaround Time: %d ticks (%d ms)\n", 
            total_turnaround_time / success_count, 
            (total_turnaround_time * 10 / success_count)); // *10은 ms 단위 환산 가정
    printf(1, "2. Avg. Waiting Time: %d ticks (%d ms)\n", 
            total_waiting_time / success_count, 
            (total_waiting_time * 10 / success_count));
    printf(1, "3. Avg. Response Time: %d ticks (%d ms)\n", 
            total_response_time / success_count, 
            (total_response_time * 10 / success_count));
    printf(1, "4. Total CPU Time: %d ticks\n", total_cpu_time);
    
    // 전체 경과 시간 계산
    int total_elapsed_time = max_completion_time - min_arrival_time;
    printf(1, "5. Total Elapsed Time: %d ticks\n", total_elapsed_time);
    printf(1, "6. Throughput: %d개 / %d ticks \n", 
            success_count, total_elapsed_time);
}

void sdebug_func(void)
{
    int i;
    int pid;
    long start;
    volatile long counter = 0;
    
    int terminated_pid; // 종료 프로세스의 pid
    int turnaround, waiting, response; // 시간 변수
    struct proc_stat proc_stat; // 프로세스 상태

    printf(1, "Start sdebug command - %s\n", scheduler_name);
    for(i = 0 ; i < PNUM ; i++){
        pid = fork();
        start = uptime(); 
        if(pid == 0){ // [os-prj3]  자식 프로세스
			
            // [os-prj3] Set weight to i
			if(weightset(i+1) < 0){
				printf(1, "PID:%d, weightset error\n", getpid());	
				exit();
			} 
			
			/*
				[os-prj3] Count PRINT_CYCLE
				and print this process information.
			*/
            
            while(counter < PRINT_CYCLE)
              ++counter;
            printf(1, "[CK] PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), i+1, (uptime()-start)*10);
      
            while(counter < TOTAL_COUNTER)
              ++counter;
            printf(1, "[TM] PID: %d, WEIGHT: %d, TIMES: %d ms\n", getpid(), i+1, (uptime()-start)*10);

            exit();
        }
        else if(pid > 0) // [os-prj3]  부모 프로세스
          continue;
        else{
          printf(1, "ERROR: fork\n");
          break;
        }
    }

    // ==========================================================
    // [os-prj3] 부모 프로세스의 통계 수집 및 계산
    // ==========================================================
    for(i = 0 ; i < PNUM ; i++) {
        terminated_pid = wait(); // 자식이 종료될 때까지 대기하고 PID를 받음

        if (terminated_pid > 0) {
            // 새로 추가한 시스템 콜을 호출하여 커널 통계 데이터 추출
            if (get_pstats(terminated_pid, &proc_stat)) == 0) {
                // 1. 반환 시간 (Turnaround Time): 종료 시간 - 도착 시간
                turnaround = proc_stat.completion_time - proc_stat.arrival_time;
                
                // 2. 대기 시간 (Waiting Time): 반환 시간 - CPU 사용 시간
                waiting = turnaround - proc_stat.cpu_time;
                
                // 3. 응답 시간 (Response Time): 첫 실행 시간 - 도착 시간
                response = proc_stat.first_run_time - proc_stat.arrival_time;

				// 총 시간 추적
                if (min_arrival_time == -1 || proc_stat.arrival_time < min_arrival_time) {
                    min_arrival_time = proc_stat.arrival_time;
                }
                if (proc_stat.completion_time > max_completion_time) {
                    max_completion_time = proc_stat.completion_time;
                }

                // 통계 누적
                total_turnaround_time += turnaround;
                total_waiting_time += waiting;
                total_response_time += response;
                total_cpu_time += proc_stat.cpu_time;
                success_count++;
                
                // 데이터 저장
                pstat[i].pid = proc_stat.pid;
                pstat[i].weight = proc_stat.weight;
                pstat[i].at = proc_stat.arrival_time;
                pstat[i].ct = proc_stat.completion_time;
                pstat[i].cput = proc_stat.cpu_time;
                pstat[i].ft = proc_stat.first_run_time;
                pstat[i].tt = turnaround;
                pstat[i].wt = waiting;
                pstat[i].rt = response;

            } else {
                printf(1, "ERROR: Failed to get stats for PID %d\n", terminated_pid);
            }
        }
    }

    // ==========================================================
    // [os-prj3] 최종 평균 성능 지표 출력
    // ==========================================================
    if (success_count > 0) {
        print_each_process();
        printf(1, "--------------------------------------------------------\n");
        print_statistics();
        printf(1, "--------------------------------------------------------\n");
    }

    printf(1, "End of sdebug command - %s\n", scheduler_name);
}


int main(void)
{
  sdebug_func();
  exit();
}
