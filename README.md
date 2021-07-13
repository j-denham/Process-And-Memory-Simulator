## Process Scheduling and Memory Management Simulator

Program investigates performance of a variety of scheduling and memory management algorithms used within an Operating System's kernel. The simulation
outputs information about its state whenever a process starts, finishes, or pages are evicted from memory, along with performance statistics once 
all processes have been finished.


![Scheduler Output](./images/scheduler-output.png)

#### Output
* Each line of output (apart from the final statistics) begins with a timestamp of the event
* Whenever the scheduler switches processes, it outputs a line with the `RUNNING` field, along with:
  - Process id
  - Remaining time until the process is complete
  - Load time, simulating the time taken to load the required pages into memory
  - Current memory usage, and the memory addresses of the pages used by the process
* Whenever the scheduler finishes executing a process, it outputs a line with the `FINISHED` field, along with:
  - The process id of that which was finished
  - The number of remaining process
* Whenever memory is full (or if virtual memory is enabled, a processes memory requirement - 4 isn't available), a line is outputted with the `EVICTED`
field, along with the addresses of the pages that were evicted

#### Scheduling Algorithms Implemented:
* First come first served
* Round robin
* Custom Scheduler: Shortest first

#### Memory Management Algorithms Implemented:
* Swapping X
* Virtual Memory
* Custom Memory Management: Fair Memory Allocation

For a much more
detailed outline of the project, see ` project2.pdf`. See `report.txt` for a short explanation of the custom scheduling and memory management
algorithms and the situations where they perform better.
