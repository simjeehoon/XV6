#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 20
#define PRINT_CYCLE 100000000
#define TOTAL_COUNTER 500000000


void sdebug_func(void)
{
    int i;
    int pid;
    long start;
    volatile long counter = 0;
    
    // ==========================================================
    // [os-prj3] 통계 변수 선언
    // ==========================================================
    int terminated_pid;
    int total_waiting_time = 0;
    int total_turnaround_time = 0;
    int total_response_time = 0;
    int total_cpu_time = 0;
    int num_completed_procs = 0;
    struct proc_stat stats;

	// 총 시간 측정을 위한 변수
    int min_arrival_time = -1; // 가장 먼저 시작한 프로세스의 arrival_time
    int max_completion_time = 0; // 가장 늦게 종료된 프로세스의 completion_time

    printf(1, "start sdebug command\n");
    for(i = 1 ; i <= PNUM ; i++){
        pid = fork();
        start = uptime(); 
        if(pid == 0){ // 자식 프로세스
			
			/*
				[os-prj3] Count PRINT_CYCLE
				and print this process information.
			*/
            
            while(counter < PRINT_CYCLE)
              ++counter;
            printf(1, "[CK] PID: %d, TIMES: %d ms\n", getpid(), (uptime()-start)*10);
      
            while(counter < TOTAL_COUNTER)
              ++counter;
            printf(1, "[TM] PID: %d, TIMES: %d ms\n", getpid(), (uptime()-start)*10);

            exit();
        }
        else if(pid > 0)
          continue;
        else{
          printf(1, "ERROR: fork\n");
          break;
        }
    }

    // ==========================================================
    // [os-prj3] 통계 수집 및 계산 로직
    // ==========================================================
    for(i = 1 ; i <= PNUM ; i++) {
        terminated_pid = wait(); // 자식이 종료될 때까지 대기하고 PID를 받음

        if (terminated_pid > 0) {
            // 새로 추가한 시스템 콜을 호출하여 커널 통계 데이터 추출
            if (get_proc_stats(terminated_pid, &stats) == 0) {
                // 1. 반환 시간 (Turnaround Time): 종료 시간 - 도착 시간
                int turnaround = stats.completion_time - stats.arrival_time;
                
                // 2. 대기 시간 (Waiting Time): 반환 시간 - CPU 사용 시간
                int waiting = turnaround - stats.cpu_time;
                
                // 3. 응답 시간 (Response Time): 첫 실행 시간 - 도착 시간
                int response = stats.first_run_time - stats.arrival_time;

				// 총 시간 추적 로직
                if (min_arrival_time == -1 || stats.arrival_time < min_arrival_time) {
                    min_arrival_time = stats.arrival_time;
                }
                if (stats.completion_time > max_completion_time) {
                    max_completion_time = stats.completion_time;
                }

                // 통계 누적
                total_turnaround_time += turnaround;
                total_waiting_time += waiting;
                total_response_time += response;
                total_cpu_time += stats.cpu_time;
                num_completed_procs++;
                
                // 개별 프로세스의 상세 결과 출력 (선택 사항, 디버깅에 유용)
                printf(1, "STAT| PID:%d | AT:%d, CT:%d, CPUT:%d, FT:%d | TT:%d, WT:%d, RT:%d\n",
                       stats.pid, 
                       stats.arrival_time, stats.completion_time, stats.cpu_time, stats.first_run_time,
                       turnaround, waiting, response);

            } else {
                printf(1, "ERROR: Failed to get stats for PID %d\n", terminated_pid);
            }
        }
    }

    // ==========================================================
    // [os-prj3] 최종 평균 성능 지표 출력
    // ==========================================================
    if (num_completed_procs > 0) {
        printf(1, "\n--- SSU Scheduler 최종 성능 분석 (%d개 프로세스) ---\n", num_completed_procs);
        printf(1, "1. 평균 반환 시간 (Avg. Turnaround Time): %d ticks (%d ms)\n", 
               total_turnaround_time / num_completed_procs, 
               (total_turnaround_time / num_completed_procs) * 10); // *10은 ms 단위 환산 가정
        printf(1, "2. 평균 대기 시간 (Avg. Waiting Time): %d ticks (%d ms)\n", 
               total_waiting_time / num_completed_procs, 
               (total_waiting_time / num_completed_procs) * 10);
        printf(1, "3. 평균 응답 시간 (Avg. Response Time): %d ticks (%d ms)\n", 
               total_response_time / num_completed_procs, 
               (total_response_time / num_completed_procs) * 10);
        printf(1, "4. 총 CPU 사용 시간 (Total CPU Time): %d ticks\n", total_cpu_time);
        
		// 전체 경과 시간 계산
        int total_elapsed_time = max_completion_time - min_arrival_time;
		printf(1, "5. 전체 테스트 경과 시간 (Total Elapsed Time): %d ticks\n", total_elapsed_time);
        printf(1, "6. 처리량 (Throughput): %d개 / %d ticks \n", 
               num_completed_procs, total_elapsed_time);
        
        
        printf(1, "--------------------------------------------------------\n");
    }

    printf(1, "end of sdebug command\n");
}


int main(void)
{
  sdebug_func();
  exit();
}
