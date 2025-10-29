#ifndef _PROCSTAT_H_
#define _PROCSTAT_H_

// [os-prj3] 통계를 위한 구조체
struct proc_stat {
    int pid;
    int weight;
    int arrival_time;
    int completion_time;
    int cpu_time;
    int first_run_time;
};

#endif 