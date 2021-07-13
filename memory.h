#ifndef MEMORY_H
#define MEMORY_H
#define KB_PER_PAGE 4
#define EMPTY 0
#define OCCUPIED 1
#define MIN_MEM_REQ 4

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process-deque.h"
#include "statistics.h"


typedef struct memory Memory;

struct memory {
    int *pages;
    int total_pages;
    int empty_pages;
    int total_processes;
    int mem_usage;
};


Memory *init_memory(int mem_size);
// Allocate pages in memory to a process, and returns amount of pages allocated
int allocate_memory(Memory *memory, Process *process, int pages);
// Evicts amount of memory given by pages and returns the indexes of the evicted pages
int *evict_memory(Memory *memory, Process *process, int pages);
// Typical ascending sort comparator, but moves -1 to the back
int page_comp(const void *first, const void *second);
void free_memory(Memory *memory);
int get_min_pages(Process *process);
// Allocates as many pages of a process as possible
int allocate_all(Memory *memory, Process *process);
// Implements heuristic to decide how much memory should be allocated
int fair_alloc(Memory *memory, Deque *deque, Process *process, int min_pages);

#endif
