/* Double-ended Queue for Proccesses */
#include "process-deque.h"



// Create a new empty Deque and return a pointer to it
Deque *new_deque() {
	// Create space
	Deque* deque = (Deque*)malloc(sizeof(Deque));
	// Just to make sure everything is pointing to null
	reset_deque(deque);
	return deque;
}


// Free the memory associated with a Deque
void free_deque(Deque *deque) {
	Node* next_node;
	Node* curr_node = deque->head;
	// First free all nodes part of the list
	while (curr_node) {
		next_node = curr_node->next;
		free_process(curr_node->process);
		free(curr_node);
		curr_node = next_node;
	}
	// Now free deque itself
	free(deque);
	return;
}

// Add a process to the top of a Deque
void deque_push(Deque *deque, Process *process) {
	Node *push_node = new_node(process);
	// Checks if it's the first element in deque in order to initialise properly
	if (deque_null(deque)) {
		deque_initial(deque, push_node);
		return;
	}
	// Else rearrange pointers accordingly
	push_node->next = deque->head;
	deque->head->prev = push_node;
	deque->head = push_node;
	deque->size += 1;
	return;
}

// Add a process to the bottom of a Deque
void deque_append(Deque *deque, Process *process) {
	Node *insert_node = new_node(process);
	// Checks if it's the first element in deque in order to initialise properly
	if (deque_null(deque)) {
		deque_initial(deque, insert_node);
		return;
	}
	// Else rearrage pointers accordingly
	insert_node->prev = deque->foot;
	deque->foot->next = insert_node;
	deque->foot = insert_node;
	deque->size += 1;
	return;
}

// Remove and return the top process from a Deque
Process *deque_pop(Deque *deque) {
    Node* popped_node = deque->head;
	// Get process contained within head
    Process *process = popped_node->process;
	// Reallocate head to next element in list and free popped_node
    deque->head = popped_node->next;
	// Checks if head is null, if not then finish rearranging pointers

    if (deque->head != NULL) {
		deque->head->prev = NULL;
		deque->size -= 1;
	} else { // Popped the last element
		reset_deque(deque);
	}

	free(popped_node);

	return process;
}

// Remove and return the bottom process from a Deque
Process *deque_remove(Deque *deque) {
	Node *removed_node = deque->foot;
	// Get process contained within foot
	Process *process = removed_node->process;
	// Reallocate foot to point to prev element in list and free removed_node
	deque->foot = removed_node->prev;
	free(removed_node);
	// Checks if foot is null, if not then finish rearranging pointers
	if (deque->foot) {
		deque->foot->next = NULL;
	}
	deque->size -= 1;
    return process;
}

// Return the number of processes in a Deque
int deque_size(Deque* deque) {
	return deque->size;
}

// Checks whether deque's elements are empty
int deque_null(Deque *deque) {
	return (deque->head == NULL) ? 1 : 0;
}

// Creates a new empty node and returns a pointer to it
Node *new_node(Process *process) {
	Node *node = (Node*)malloc(sizeof(Node));
	node->prev = NULL;
	node->next = NULL;
	node->process = process;
	return node;
}

// Resets deque to null values
void reset_deque(Deque *deque) {
	deque->size = 0;
	deque->head = NULL;
	deque->foot = NULL;
}

// Deals with the initial case where list only has one node
void deque_initial(Deque *deque, Node *node) {
	deque->head = node;
	deque->foot = node;
	deque->size = 1;

	return;
}

// Creates and returns new process struct based on 4-tuple line passed
Process *new_process(char *process_line) {
    int i=0;
    int val;
    char *ele;
    Process *process = (Process*)malloc(sizeof(Process));

    // (time arrived, process id, memory size requirement, job time)
    while ((ele = strsep(&process_line, " ")) != NULL) {
        val = atoi(ele);
        if (i == ARRIVED) {
            process->arrival_time = val;
        }
        if (i == ID) {
            process->pid = val;
        }
        if (i == MEM_REQ) {
            process->mem_req = val/KB_PER_PAGE;
        }
        if (i == JOB_TIME) {
            process->job_time = val;
            process->remaining_time = val;
        }
        i++;
    }
	process->mem_index = (int*)malloc(process->mem_req*sizeof(int));
	// Initialise all values to NO_INDEX as 0 is the index of the first page
	for (int i=0; i<process->mem_req; i++) {
		process->mem_index[i] = NO_INDEX;
	}
	process->pages_used = 0;
	return process;
}

// Frees memory associated with process
void free_process(Process* process) {
	free(process->mem_index);
	free(process);
	return;
}

// Appends a process from process_list to arrived at the appropriate time
void update_deque(int clock, Deque *process_list, Deque *arrived) {
	//int prev_arrival_time=-1, multiple_arrivals=0;
	Process *arriving_process;
	//Node *start_from;

	while ((!deque_null(process_list)) && (process_list->head->process->arrival_time <= clock)) {
		arriving_process = deque_pop(process_list);
		//if (arriving_process->arrival_time == prev_arrival_time) {
			// First time triggered
		//	if (multiple_arrivals == 0) {
		//		start_from = arrived->foot;
		//	}
		//	multiple_arrivals = 1;
		//}
		deque_append(arrived, arriving_process);
		//prev_arrival_time = arriving_process->arrival_time;
	}

	//if (multiple_arrivals) {
	//	order_deque(arrived, start_from);
	//}

	return;
}

// Resolve order of same-time process arrivals using insertion sort on process id
void order_deque(Deque *deque) {
    int prev_arrival_time=-1;
    Node *curr_node, *insert_at;
    curr_node = deque->head;
    while (curr_node != NULL) {
        if (curr_node->process->arrival_time == prev_arrival_time) {
			insert_at = curr_node;
			while ((insert_at->prev != NULL) && (insert_at->prev->process->pid > curr_node->process->pid)) {
				if (insert_at->prev->process->arrival_time != prev_arrival_time) {
					break;
				}
				insert_at = insert_at->prev;
            }
			prev_arrival_time = curr_node->process->arrival_time;
			curr_node = insert_before(deque, curr_node, insert_at);
        } else {
            prev_arrival_time = curr_node->process->arrival_time;
            curr_node = curr_node->next;
        }
    }
}

// Inserts curr_node before insert_at and returns the original curr_node->next
Node *insert_before(Deque *deque, Node *curr_node, Node *insert_at) {
	Node* next = curr_node->next;

	if (curr_node == insert_at) {
		return next;
	}

	// First handle Nodes adjacent to curr_node's initial location
	// Consider refactoring
	curr_node->prev->next = next;
	if (curr_node == deque->foot) {
		deque->foot = curr_node->prev;
	} else {
		curr_node->next->prev = curr_node->prev;
	}

	// Insert curr_node into new location
	curr_node->prev = insert_at->prev;
	curr_node->next = insert_at;

	// Handle Nodes adjacent to curr_node's new location
	if (curr_node->prev != NULL) {
		curr_node->prev->next = curr_node;
	} else {
		deque->head = curr_node;
	}
	insert_at->prev = curr_node;

	return next;
}

Node *get_least_recent(Deque *deque) {
	Node *least_recent = deque->head;

	if (least_recent == NULL) {
		// fprintf(stderr, "get_least_recent called on empty deque\n");
		return least_recent;
	}

	while ((least_recent != NULL) && (least_recent->process->pages_used == 0)) {
		least_recent = least_recent->next;
	}

	return least_recent;
}

// Implements a priority queue using insertion sort based on job time
void prioritise(Deque *deque)
{
	Node *curr = deque->head;
	Node *insert_at;

	while (curr != NULL) {
		insert_at = curr;
		while ((insert_at->prev != NULL) && (curr->process->job_time < insert_at->prev->process->job_time)) {
			insert_at = insert_at->prev;
		}
		if (insert_at != curr) {
			curr = insert_before(deque, curr, insert_at);
		} else {
			curr = curr->next;
		}
	}

	return;
}
