#ifndef STATISTICS_H
#define STATISTICS_H
#define TH_AVG 0
#define TH_MIN 1
#define TH_MAX 2
#define TH_INTRVL 3
#define TH_PROC 4
#define OVR_MAX 0
#define OVR_AVG 1
#define INTRVL_RANGE 60

#include <stdio.h>
#include <stdlib.h>
#include "process-deque.h"

typedef struct statistics Statistics;

struct statistics {
    int throughput[5];
    int turnaround;
    float overhead[2];
    int makespan;
    int finished_processes;
};

Statistics *init_stats();
// Updates stats when a process has been completed
void update_stats(int clock, Statistics *stats, Process *process);
// Assign makespan and averages once all processes are finished
void finalise_stats(int clock, Statistics *stats);
// Calculates average and rounds up to an integer
int rounded_average(int numerator, int divisor);

#endif
