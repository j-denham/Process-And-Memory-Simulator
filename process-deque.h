#ifndef PROCESS_DEQUE
#define PROCESS_DEQUE
#define ARRIVED 0
#define ID 1
#define MEM_REQ 2
#define JOB_TIME 3
#define NO_INDEX -1

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct process Process;
typedef struct node Node;
typedef struct deque Deque;

struct process {
    int arrival_time;
    int pid;
    int mem_req; // Given in pages required
    int job_time;
    int remaining_time;
    int *mem_index;
    int pages_used;
};

struct node {
    Process *process;
    Node *prev;
    Node *next;
};


struct deque {
    int size;
    Node *head;
    Node *foot;
};

#include "memory.h"

// Create a new empty Deque and return a pointer to it
Deque *new_deque();

// Free the memory associated with a Deque
void free_deque(Deque *deque);

// Add a process to the top of a Deque
void deque_push(Deque *deque, Process* process);

// Add a process to the bottom of a Deque
void deque_append(Deque *deque, Process* process);

// Remove and return the top process from a Deque
Process* deque_pop(Deque *deque);

// Remove and return the bottom process from a Deque
Process* deque_remove(Deque *deque);

// Return the number of process in a Deque
int deque_size(Deque *deque);

// Checks whether deque's elements are empty
int deque_null(Deque* deque);

// Creates a new empty node and returns a pointer to it
Node* new_node(Process* process);

// Resets deque to null values
void reset_deque(Deque* deque);

// Deals with the initial case where list only has one node
void deque_initial(Deque* deque, Node* node);

// Creates and returns new process derived from process_line
Process* new_process(char* process_line);

// Frees memory associated with process
void free_process(Process* process);

// Appends a process from process_list to arrived at the appropriate time
void update_deque(int clock, Deque *process_list, Deque *arrived);

// Resolve order of same-time process arrivals by process id
void order_deque(Deque *deque);

// Inserts Node curr before insert_at and returns the Node after curr
Node *insert_before(Deque *deque, Node *curr_node, Node *insert_at);

// Returns a pointer to the least recently executed process
Node *get_least_recent(Deque *deque);

// Implements a priority queue using insertion sort based on job time
void prioritise(Deque *deque);

#endif
