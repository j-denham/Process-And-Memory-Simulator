#define NUM_OPTIONAL 2
#define MEM_INDX 0
#define QTM_INDX 1
#define LINE_BUFF 50

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "statistics.h"
#include "memory.h"




void strategy(char *filename, char *sched_alg, char *mem_alloc, int *optionals);
int fcfs(Deque *process_list, Statistics *stats, Memory *memory,
         int (*mem_strat)(int, Memory*, Deque*, Process*));
int round_robin(Deque *process_list, int quantum, Statistics *stats, Memory *memory,
                int (*mem_strat)(int, Memory*, Deque*, Process*));
int shortest_first(Deque *process_list, Statistics *stats, Memory *memory,
                   int (*mem_strat)(int, Memory*, Deque*, Process*));
// Reads all processes from file
Deque *read_processes(char *filename);
void testing(Deque* process_list);
void print_execution(int clock, Process* process, int load_time, int mem_usage);
void print_finishing(int clock, int pid, int proc_remaining);
void print_performance(Statistics *stats);
void print_evicted(int clock, int *evicted_pages, int num_evicted);
// Returns updated clock and restructures process_list/arrived as appropriate
int step(int clock, int step, Deque *process_list, Deque *arrived);
// Handles memory management using Swapping-X and returns load time
int swapping(int clock, Memory *memory, Deque* arrived, Process *process);
// Handles memory management using Swapping-X and returns load time
int virtual(int clock, Memory *memory, Deque* arrived, Process *process);
// Handles memory management by trying to evict equal pages from all arrived processes
int fair(int clock, Memory *memory, Deque *arrived, Process *process);
