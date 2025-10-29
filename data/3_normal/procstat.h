#ifndef _PROCSTAT_H_
#define _PROCSTAT_H_

// [os-prj3] 통계를 위한 구조체
struct proc_stat {
    uint arrival_time;
    uint completion_time;
    uint cpu_time;
    uint first_run_time;
};

#endif 