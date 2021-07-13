#include "statistics.h"


Statistics *init_stats() {
    Statistics *new_stats;
    new_stats = (Statistics*)malloc(sizeof(Statistics));

    /* All fields that store averages will store the total until
       until finalise_stats() is called */
    new_stats->throughput[TH_AVG] = 0;
    new_stats->throughput[TH_MIN] = -1;
    new_stats->throughput[TH_MAX] = 0;
    // Stores the current 60s interval through clock / 60
    new_stats->throughput[TH_INTRVL] = 0;
    // Total processes completed given a 60s interval
    new_stats->throughput[TH_PROC] = 0;
    new_stats->turnaround = 0;
    new_stats->overhead[OVR_MAX] = 0.0;
    new_stats->overhead[OVR_AVG] = 0.0;
    new_stats->makespan = 0;
    new_stats->finished_processes = 0;

    return new_stats;
}

// Updates stats when a process has been completed
void update_stats(int clock, Statistics *stats, Process *process) {
    int process_turnaround, interval;
    float process_overhead;

    process_turnaround = clock - process->arrival_time;
    process_overhead = (float)process_turnaround / process->job_time;
    // Increment totals
    stats->turnaround += process_turnaround;
    stats->overhead[OVR_AVG] += process_overhead;
    stats->finished_processes += 1;

    if (process_overhead > stats->overhead[OVR_MAX]) {
        stats->overhead[OVR_MAX] = process_overhead;
    }

    // Update throughput on new 60s interval
    interval = (clock-1) / INTRVL_RANGE;
    if (interval > stats->throughput[TH_INTRVL]) {
        // Interval has been skipped, implying throughput of 0.
        if (interval - stats->throughput[TH_INTRVL] > 1) {
            stats->throughput[TH_MIN] = 0;
        }
        stats->throughput[TH_INTRVL] = interval;
        // Second statement deals with initial case
        if ((stats->throughput[TH_PROC] < stats->throughput[TH_MIN]) || (stats->throughput[TH_MIN] < 0)) {
            stats->throughput[TH_MIN] = stats->throughput[TH_PROC];
        }
        stats->throughput[TH_PROC] = 1;
        if (stats->throughput[TH_PROC] > stats->throughput[TH_MAX]) {
            stats->throughput[TH_MAX] = stats->throughput[TH_PROC];
        }
    } else { // Still in old interval, increment completed processes
        stats->throughput[TH_PROC] += 1;
        if (stats->throughput[TH_PROC] > stats->throughput[TH_MAX]) {
            stats->throughput[TH_MAX] = stats->throughput[TH_PROC];
        }
    }

    return;
}

// Calculate the averages and assign makespan once all processes are finished
void finalise_stats(int clock, Statistics *stats) {
    stats->makespan = clock;
    stats->throughput[TH_AVG] = rounded_average(stats->finished_processes,
                                                stats->throughput[TH_INTRVL]+1);
    stats->turnaround = rounded_average(stats->turnaround, stats->finished_processes);
    stats->overhead[OVR_AVG] = (float)stats->overhead[OVR_AVG] / stats->finished_processes;
    // Final check of min throughput in case last step is min
    if (stats->throughput[TH_PROC] < stats->throughput[TH_MIN]) {
        stats->throughput[TH_MIN] = stats->throughput[TH_PROC];
    }
}

// Calculates average and rounds up to an integer
int rounded_average(int numerator, int divisor) {
    int int_avg;
    float true_avg;

    true_avg = (float)numerator / divisor;
    int_avg = (int)true_avg;
    if (true_avg != int_avg) {
        int_avg++;
    }
    return int_avg;
}
