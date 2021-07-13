# COMP30023 Project 2 Makefile

CC = gcc
CFLAGS = -Wall
EXE = scheduler
OBJ = scheduler.o statistics.o process-deque.o memory.o
TESTARGS = ./scheduler -f processes.txt -a rr -s 100 -m v -q 10

# TESTARGS = scheduler -f processes.txt -a ff -m u


all: $(EXE)


$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ) -g

# Obj files

scheduler.o: scheduler.c scheduler.h
	$(CC) $(CFLAGS) -c scheduler.c -g

memory.o: memory.c memory.h
	$(CC) $(CFLAGS) -c memory.c -g

statistics.o: statistics.c statistics.h
	$(CC) $(CFLAGS) -c statistics.c -g

process-deque.o: process-deque.c process-deque.h
	$(CC) $(CFLAGS) -c process-deque.c -g


# Phony targets

.PHONY: all clean run debug

clean:
	rm -f *.o *.gch

run:
	#dash tests/cases/testcase1.in >output.txt && diff tests/cases/testcase1.out output.txt
	#dash tests/cases/testcase2.in >output.txt && diff tests/cases/testcase2.out output.txt
	#dash tests/cases/testcase3.in >output.txt && diff tests/cases/testcase3.out output.txt
	#dash tests/cases/testcase4.in >output.txt && diff tests/cases/testcase4.out output.txt
	#dash tests/cases/testcase5.in >output.txt && diff tests/cases/testcase5.out output.txt
	#dash tests/cases/testcase6.in >output.txt && diff tests/cases/testcase6.out output.txt
	#dash tests/cases/testcase7.in >output.txt && diff tests/cases/testcase7.out output.txt
	#dash tests/cases/testcase8.in >output.txt && diff tests/cases/testcase8.out output.txt
	#dash tests/cases/testcase9.in >output.txt && diff tests/cases/testcase9.out output.txt
	#dash tests/cases/testcase10.in >output.txt && diff tests/cases/testcase10.out output.txt
	#dash tests/cases/testcase11.in >output.txt && diff tests/cases/testcase11.out output.txt

	dash cs.in >output-cs.txt
	dash cs-rr.in >output-rr.txt
	dash cs-ff.in >output-ff.txt

	dash cm.in >output-cm.txt
	dash cm-p.in >output-p.txt
	dash cm-v.in >output-v.txt

debug:
	gdb -args $(EXE) $(TESTARGS)
	#valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes  $(TESTARGS)
