#include "memory.h"


Memory *init_memory(int mem_size) {
    Memory *new_memory;

    new_memory = (Memory*)malloc(sizeof(Memory));
    new_memory->total_pages = mem_size/KB_PER_PAGE;
    new_memory->empty_pages = new_memory->total_pages;
    new_memory->total_processes = 0;
    new_memory->mem_usage = 0;
    new_memory->pages = (int*)malloc((new_memory->total_pages)*sizeof(int));
    if (new_memory->total_pages) {
        memset(new_memory->pages, 0, new_memory->total_pages*sizeof(int));
    }

    return new_memory;
}

// Allocate pages in memory to a process, and returns amount of pages allocated
int allocate_memory(Memory *memory, Process *process, int pages) {
    int i=0;
    int allocated_pages=0;

    if (memory->empty_pages < pages) {
        fprintf(stderr, "Trying to allocate more memory then there actually is\n");
        exit(1);
    }

    while ((i < memory->total_pages) && (allocated_pages < pages)) {
        if (memory->pages[i] == EMPTY) {
            memory->pages[i] = OCCUPIED;
            process->mem_index[process->pages_used] = i;
            process->pages_used += 1;
            memory->empty_pages -= 1;
            allocated_pages += 1;
        }
        i++;
    }

    if (allocated_pages > 0) {
        memory->total_processes += 1;
        memory->mem_usage = rounded_average((memory->total_pages - memory->empty_pages)*100,
                                             memory->total_pages);
    }

    qsort(process->mem_index, process->mem_req, sizeof(int), page_comp);

    return allocated_pages;
}

// Evicts amount of memory given by pages and returns the indexes of the evicted pages
int *evict_memory(Memory *memory, Process *process, int pages) {
    int i=0;

    if (pages == 0) {
        fprintf(stderr, "trying to allocate 0 pages");
        return NULL;
    }

    int *evicted_pages=(int*)malloc(pages*sizeof(int));

    while (i < pages) {
        if (process->mem_index[i] == NO_INDEX) {
            fprintf(stderr, "Expected to evict more pages than there are\n");
            break;
        }
        memory->pages[process->mem_index[i]] = EMPTY;
        evicted_pages[i] = process->mem_index[i];
        process->mem_index[i] = NO_INDEX;
        process->pages_used -= 1;
        memory->empty_pages += 1;
        i++;
    }

    if (process->pages_used != 0) {
        qsort(process->mem_index, process->mem_req, sizeof(int), page_comp);
    }

    memory->mem_usage = rounded_average((memory->total_pages - memory->empty_pages)*100,
                                         memory->total_pages);

    return evicted_pages;
}

// Typical ascending sort comparator, but moves -1 to the back
int page_comp(const void *first, const void *second) {
    int *x, *y;
    x = (int*)first;
    y = (int*)second;
    if (*x == -1) {
        return 1;
    } else if (*y == -1) {
        return -1;
    } else {
        return *x - *y;
    }
}

void free_memory(Memory *memory) {
    free(memory->pages);
    free(memory);
}

int get_min_pages(Process *process) {
    if (process->mem_req < MIN_MEM_REQ) {
        return process->mem_req - process->pages_used;
    } else {
        return MIN_MEM_REQ - process->pages_used;
    }
}

// Allocates as many pages of a process as possible
int allocate_all(Memory *memory, Process *process) {
    int alloc, load_time;

    if (memory->empty_pages > (process->mem_req - process->pages_used)) {
        alloc = process->mem_req - process->pages_used;
    } else {
        alloc = memory->empty_pages;
    }

    load_time = 2 * allocate_memory(memory, process, alloc);
    process->remaining_time += process->mem_req - process->pages_used;
    return load_time;
}

// Implements heuristic to decide how much memory should be allocated
int fair_alloc(Memory *memory, Deque *deque, Process *process, int min_pages) {
	float time_proportion;
	int heuristic;
	int total_time=process->remaining_time;
	Node *curr = deque->head;

	// Get total remaining time
	while (curr != NULL) {
		total_time += curr->process->remaining_time;
		curr = curr->next;
	}

	time_proportion = (float)process->remaining_time / total_time;

	heuristic = memory->total_pages * time_proportion;

    if (heuristic > process->mem_req) {
        return process->mem_req - process->pages_used;
    } else if (heuristic < (min_pages + process->pages_used)) {
        return min_pages;
    } else {
        return heuristic - process->pages_used;
    }
}
