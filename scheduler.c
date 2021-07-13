#include "scheduler.h"


int main(int argc, char **argv)
{
    int opt;
    int optionals[NUM_OPTIONAL];
    char *filename, *sched_alg, *mem_alloc;

    /* Flags: -f filename -a scheduling-alg {ff, rr, cs}
              -m memory-allocation {u,p,v,cm}
              OPTIONAL: -s memory-size
                        -q quantum for rr               */

    while ((opt = getopt(argc, argv, "f:a:m:s:q::")) != -1) {
        if (opt == 'f') {
            filename = optarg;
        }
        if (opt == 'a') {
            sched_alg = optarg;
        }
        if (opt == 'm') {
            mem_alloc = optarg;
            if (!strcmp(mem_alloc, "u")) {
                optionals[MEM_INDX] = 0;
            }
        }
        if ( opt == 's') {
            if (optarg == NULL) {
                // Default value
                optionals[MEM_INDX] = 0;
            } else {
                optionals[MEM_INDX] = atoi(optarg);
            }
        }
        if ( opt == 'q') {
            if (optarg == NULL) {
                // Default value
                optionals[QTM_INDX] = 10;
            } else {
                optionals[QTM_INDX] = atoi(optarg);
            }
        }
        if ( opt == '?') {
            fprintf(stderr, "Wrong Command Line Arguments");
            exit(1);
        }
    }

    strategy(filename, sched_alg, mem_alloc, optionals);

    return 0;
}

/* Calls scheduling/memory allocation algorithms and sets variables
   according to the specified options */

void strategy(char *filename, char *sched_alg, char *mem_alloc, int *optionals)
{
    Deque *process_list;
    Statistics *stats;
    int completion_time;
    Memory* memory;
    int (*mem_strat)(int, Memory*, Deque*, Process*);

    // First read input from file
    process_list = read_processes(filename);
    // Pre-emptive sorting confirmed to be allowed, do it here
    order_deque(process_list);
    //testing(process_list);
    stats = init_stats();

    /* MEMORY MANAGEMENT ALGORITHMS */
    memory = init_memory(optionals[MEM_INDX]);
    if (!strcmp(mem_alloc, "u")) {
        // -1 indicates to print_execution() that unlimited memory is being used
        memory->mem_usage = -1;
        mem_strat = NULL;
    } else if (!strcmp(mem_alloc, "p")) {
        mem_strat = swapping;
    } else if (!strcmp(mem_alloc, "v")) {
        mem_strat = virtual;
    } else {
        mem_strat = fair;
    }

    /* SCHEDULING ALGORITHMS */
    // First come, first served
    if (!strcmp(sched_alg, "ff")) {
        completion_time = fcfs(process_list, stats, memory, mem_strat);
    }
    //Round Robin
    if (!strcmp(sched_alg, "rr")) {
        completion_time = round_robin(process_list, optionals[QTM_INDX],
                                      stats, memory, mem_strat);
    }
    // Shortest First
    if (!strcmp(sched_alg, "cs")) {
        completion_time = shortest_first(process_list, stats, memory, mem_strat);
    }

    // All processes finished, summarise performace
    finalise_stats(completion_time, stats);
    print_performance(stats);
    free_deque(process_list);
    free_memory(memory);
    free(stats);
    return;
}

/* Reads all processes from file */
Deque *read_processes(char *filename)
{
    Deque *process_list = new_deque();
    Process *created;
    char process_line[LINE_BUFF];
    FILE *input = fopen(filename, "r");

    while (fgets(process_line, sizeof(process_line), input) != NULL) {
        created = new_process(process_line);
        deque_append(process_list, created);
    }
    fclose(input);

    return process_list;
}

void testing(Deque *process_list)
{
    Process *prints;
    Node *next_node;

    next_node = process_list->head;
    prints = next_node->process;
    while (next_node != NULL) {
        prints = next_node->process;
        printf("FOUR TUPLE\n");
        printf("%d, %d, %d, %d", prints->arrival_time, prints->pid, prints->mem_req, prints->job_time);
        printf("\n");
        next_node = next_node->next;

    }
}

/* First come, first served */
int fcfs(Deque *process_list, Statistics *stats, Memory *memory,
         int (*mem_strat)(int, Memory*, Deque*, Process*))
{
    int clock = 0;
    int load_time = 0;
    int num_evicted;
    int* evicted_pages;
    Deque *arrived = new_deque();
    Process *curr;


    while ((!deque_null(process_list)) || (!deque_null(arrived))) {
        update_deque(clock, process_list, arrived);
        if (!deque_null(arrived)) {
            curr = deque_pop(arrived);
            // First, load job
            if (mem_strat != NULL) {
                load_time = (*mem_strat)(clock, memory, arrived, curr);
            }
            // Start job
            print_execution(clock, curr, load_time, memory->mem_usage);
            clock = step(clock, curr->job_time+load_time, process_list, arrived);
            // Evict then finish
            if (mem_strat != NULL) {
                num_evicted = curr->pages_used;
                evicted_pages = evict_memory(memory, curr, num_evicted);
                print_evicted(clock, evicted_pages, num_evicted);
                free(evicted_pages);
            }
            update_stats(clock, stats, curr);
            print_finishing(clock, curr->pid, arrived->size);
            free(curr);
        } else { // Wait for new arrivals
            clock++;
        }
    }

    free_deque(arrived);
    return clock;
}

int round_robin(Deque *process_list, int quantum, Statistics *stats, Memory *memory,
                int (*mem_strat)(int, Memory*, Deque*, Process*))
{
    int clock = 0;
    int load_time = 0;
    int num_evicted;
    int *evicted_pages;
    Deque *arrived = new_deque();
    Process *curr;

    while ((!deque_null(process_list)) || (!deque_null(arrived))) {
        update_deque(clock, process_list, arrived);
        //testing(arrived);
        if (!deque_null(arrived)) {
            curr = deque_pop(arrived);
            // Load job
            if (mem_strat != NULL) {
                load_time = (*mem_strat)(clock, memory, arrived, curr);
            }
            // Start current job
            print_execution(clock, curr, load_time, memory->mem_usage);
            // Finish job within quantum
            if (curr->remaining_time <= quantum) {
                clock = step(clock, curr->remaining_time+load_time, process_list, arrived);
                if (mem_strat != NULL) {
                    num_evicted = curr->pages_used;
                    evicted_pages = evict_memory(memory, curr, num_evicted);
                    print_evicted(clock, evicted_pages, num_evicted);
                    free(evicted_pages);
                }
                update_stats(clock, stats, curr);
                print_finishing(clock, curr->pid, arrived->size);
                free_process(curr);
            } else { // Quantum elapsed, process goes to the back
                clock = step(clock, quantum+load_time, process_list, arrived);
                curr->remaining_time -= quantum;
                deque_append(arrived, curr);
            }
        } else { // Wait for new arrivals
            clock++;
        }
    }
    free_deque(arrived);
    return clock;
}

int shortest_first(Deque *process_list, Statistics *stats, Memory *memory,
                   int (*mem_strat)(int, Memory*, Deque*, Process*))
{
    int clock = 0;
    int load_time = 0;
    int num_evicted;
    int *evicted_pages;
    Deque *arrived = new_deque();
    Process *curr;

    while ((!deque_null(process_list)) || (!deque_null(arrived))) {
        update_deque(clock, process_list, arrived);
        prioritise(arrived);
        if (!deque_null(arrived)) {
            curr = deque_pop(arrived);
            // Load job
            if (mem_strat != NULL) {
                load_time = (*mem_strat)(clock, memory, arrived, curr);
            }
            // Start current job
            print_execution(clock, curr, load_time, memory->mem_usage);
            clock = step(clock, curr->job_time+load_time, process_list, arrived);
            // Evict then finish
            if (mem_strat != NULL) {
                num_evicted = curr->pages_used;
                evicted_pages = evict_memory(memory, curr, num_evicted);
                print_evicted(clock, evicted_pages, num_evicted);
                free(evicted_pages);
            }
            update_stats(clock, stats, curr);
            print_finishing(clock, curr->pid, arrived->size);
            free(curr);
        } else {
            clock++;
        }
    }
    return clock;
}

void print_execution(int clock, Process* process, int load_time, int mem_usage)
{
    int i=0;
    if (mem_usage >= 0) {
        printf("%d, RUNNING, id=%d, remaining-time=%d, load-time=%d, mem-usage=%d\%, ",
                clock, process->pid, process->remaining_time, load_time, mem_usage);
        printf("mem-addresses=[");
        while (i < process->pages_used) {
            if (i == process->pages_used - 1) {
                printf("%d]\n", process->mem_index[i]);
            } else {
                printf("%d,", process->mem_index[i]);
            }
            i++;
        }
    } else {
        printf("%d, RUNNING, id=%d, remaining-time=%d\n",
                clock, process->pid, process->remaining_time);
    }

}

void print_finishing(int clock, int pid, int proc_remaining)
{
    printf("%d, FINISHED, id=%d, proc-remaining=%d\n", clock, pid, proc_remaining);
}

void print_performance(Statistics *stats)
{
    printf("Throughput %d, %d, %d\n",
            stats->throughput[TH_AVG], stats->throughput[TH_MIN],
            stats->throughput[TH_MAX]);
           printf("Turnaround time %d\n", stats->turnaround);
           printf("Time overhead %.2f %.2f\n",
                   stats->overhead[OVR_MAX], stats->overhead[OVR_AVG]);
           printf("Makespan %d\n", stats->makespan);
}

void print_evicted(int clock, int *evicted_pages, int num_evicted)
{
    int i=0;
    qsort(evicted_pages, num_evicted, sizeof(int), page_comp);
    printf("%d, EVICTED, mem-addresses=[", clock);
    while (i < num_evicted) {
        if (i == num_evicted - 1) {
            printf("%d]\n", evicted_pages[i]);
        } else {
            printf("%d,", evicted_pages[i]);
        }
        i++;
    }
}

// Updates clock, stats and structure of process_list/arrived as appropriate
int step(int clock, int step, Deque *process_list, Deque *arrived)
{
    int curr_time=clock+step;
    update_deque(curr_time, process_list, arrived);
    return curr_time;
}

// Handles memory management using Swapping-X and returns load time
int swapping(int clock, Memory *memory, Deque* arrived, Process *process)
{
    int load_time, index=0;
    int num_evicted = 0;
    int *evicted_pages = (int*)malloc((memory->total_pages)*sizeof(int));
    int *curr_evicted;
    Node *least_recent = get_least_recent(arrived);


    if (process->pages_used == process->mem_req) {
        return 0;
    }
    // No space? Evict memory from least recently executed process until there is
    while ((process->mem_req - process->pages_used) > memory->empty_pages) {
        if (least_recent == NULL) {
            fprintf(stderr, "Swapping least recent is null. May have gone past foot\n");
            break;
        }
        if (least_recent->process->pages_used > 0) {
            index += num_evicted;
            num_evicted = least_recent->process->pages_used;
            curr_evicted = evict_memory(memory, least_recent->process,
                                        least_recent->process->pages_used);
            memmove(evicted_pages+index, curr_evicted, num_evicted*sizeof(int));
            free(curr_evicted);
        }
        least_recent = least_recent->next;
    }
    if (num_evicted > 0) {
        print_evicted(clock, evicted_pages, num_evicted);
    }
    load_time = 2 * allocate_memory(memory, process,
                                    process->mem_req - process->pages_used);
    free(evicted_pages);
    return load_time;
}

// Handles memory management using Virtual Memory and returns load time
int virtual(int clock, Memory *memory, Deque *arrived, Process *process)
{
    int load_time, min_pages;
    int num_evicted = 0;
    int *evicted_pages = (int*)malloc((memory->total_pages)*sizeof(int));
    int *curr_evicted;
    Node *least_recent = get_least_recent(arrived);

    if (process->pages_used == process->mem_req) {
        return 0;
    }

    // Set bound for minimum pages to allocate
    min_pages = get_min_pages(process);

    // No need to evict, allocate as many as possible
    if (memory->empty_pages >= min_pages) {
        load_time = allocate_all(memory, process);
        return load_time;
    }

    // Otherwise have to evict
    while (memory->empty_pages < min_pages) {
        if (least_recent == NULL) {
            fprintf(stderr, "Virtual least recent is null. May have gone past foot\n");
            break;
        }
        while (least_recent->process->pages_used > 0) {
            if (memory->empty_pages >= min_pages) {
                break;
            }
            // Evict one by one
            curr_evicted = evict_memory(memory, least_recent->process, 1);
            evicted_pages[num_evicted] = *curr_evicted;
            num_evicted += 1;
            free(curr_evicted);
        }
        least_recent = least_recent->next;
    }
    print_evicted(clock, evicted_pages, num_evicted);
    load_time = 2 * allocate_memory(memory, process, min_pages);
    process->remaining_time += process->mem_req - process->pages_used;
    return load_time;
}

// Handles memory management by trying to evict equal pages from all arrived processes
int fair(int clock, Memory *memory, Deque *arrived, Process *process)
{
    int num_evicted=0;
    int old_evicted=0;
    int force=0;
    int load_time, min_pages, alloc_pages;
    int *evicted_pages = (int*)malloc((memory->total_pages)*sizeof(int));
    int *curr_evicted;
    Node *curr = arrived->head;


    if (process->pages_used == process->mem_req) {
        return 0;
    }

    min_pages = get_min_pages(process);
    alloc_pages = fair_alloc(memory, arrived, process, min_pages);

    if (memory->empty_pages >= alloc_pages) {
        load_time = 2 * allocate_memory(memory, process, alloc_pages);
        process->remaining_time += process->mem_req - process->pages_used;
        return load_time;
    }

    // Evict 1 page from each arrived process until sufficient space
    while (memory->empty_pages < alloc_pages) {
        if ((curr->process->pages_used > MIN_MEM_REQ) ||
            ((force) && (curr->process->pages_used > 0))) {
            curr_evicted = evict_memory(memory, curr->process, 1);
            evicted_pages[num_evicted] = *curr_evicted;
            num_evicted++;
            free(curr_evicted);
        }
        // Iterated through every arrived processes
        if (curr->next == NULL) {
            // Force evict past the MIN_MEM_REQ threshold if needed
            if (num_evicted == old_evicted) {
                force = 1;
            }
            old_evicted = num_evicted;
            curr = arrived->head;
        } else { // Otherwise keep going
            curr = curr->next;
        }
    }
    print_evicted(clock, evicted_pages, num_evicted);
    load_time = 2 * allocate_memory(memory, process, alloc_pages);
    process->remaining_time += process->mem_req - process->pages_used;
    return load_time;
}
