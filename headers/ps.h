#ifndef MY_PS_H
#define MY_PS_H
#include <stdint.h>

enum {
    STRING_RESTRICTION = 1 << 1
};


enum sortMode {
    SORT_BY_NAME,
    SORT_BY_PID,
    SORT_UP_BY_VM_RSS,
    SORT_DOWN_BY_VM_RSS,
    SORT_UP_BY_VM_SIZE,
    SORT_DOWN_BY_VM_SIZE,
    SORT_UP_BY_MEMORY_PERCENT,
    SORT_DOWN_BY_MEMORY_PERCENT,
    NOT_SORTED
};

typedef struct options {
    int flags;
    enum sortMode sortMode;
    long limits;   
} options;

typedef struct {
    /* 1  */ int32_t  pid;
    /* 2  */ char     comm[16];      // TASK_COMM_LEN = 16 in kernel
    /* 3  */ char     state;
    /* 4  */ int32_t  ppid;
    /* 5  */ int32_t  pgrp;
    /* 6  */ int32_t  session;
    /* 7  */ int32_t  tty_nr;
    /* 8  */ int32_t  tpgid;
    /* 9  */ uint32_t flags;
    /* 10 */ uint64_t minflt;
    /* 11 */ uint64_t cminflt;
    /* 12 */ uint64_t majflt;
    /* 13 */ uint64_t cmajflt;
    /* 14 */ uint64_t   utime;
    /* 15 */ uint64_t stime;
    /* 16 */ int64_t  cutime;
    /* 17 */ int64_t  cstime;
    /* 18 */ int64_t  priority;
    /* 19 */ int64_t  nice;
    /* 20 */ int64_t  num_threads;
    /* 21 */ int64_t  itrealvalue;   //always 0 since Linux 2.6
    /* 22 */ uint64_t starttime;
    /* 23 */ uint64_t vsize;
    /* 24 */ int64_t  rss;
    /* 25 */ uint64_t rsslim;
    /* 26 */ uint64_t startcode;
    /* 27 */ uint64_t endcode;
    /* 28 */ uint64_t startstack;
    /* 29 */ uint64_t kstkesp;
    /* 30 */ uint64_t kstkeip;
    /* 31 */ uint64_t signal;        //deprecated /proc/pid/status
    /* 32 */ uint64_t blocked;
    /* 33 */ uint64_t sigignore;
    /* 34 */ uint64_t sigcatch;
    /* 35 */ uint64_t wchan;
    /* 36 */ uint64_t nswap;         // always  0
    /* 37 */ uint64_t cnswap;        // always 0
    /* 38 */ int32_t  exit_signal;
    /* 39 */ int32_t  processor;
    /* 40 */ uint32_t rt_priority;
    /* 41 */ uint32_t policy;
    /* 42 */ uint64_t delayacct_blkio_ticks;
    /* 43 */ uint64_t guest_time;
    /* 44 */ int64_t  cguest_time;
    /* 45 */ uint64_t start_data;
    /* 46 */ uint64_t end_data;
    /* 47 */ uint64_t start_brk;
    /* 48 */ uint64_t arg_start;
    /* 49 */ uint64_t arg_end;
    /* 50 */ uint64_t env_start;
    /* 51 */ uint64_t env_end;
    /* 52 */ int32_t  exit_code;
    double memoryPercent;
    double cpuPercent;
} proc;

typedef struct {
    proc* ps;
    int capacity;
    int size;
} procList;

#endif