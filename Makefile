CC = gcc

all: sim-test sim-mips

sim-test: sim-test.c sim-mips.c
	$(CC) -g sim-mips.c -shared -o libMIPS.so
	$(CC) -g -o sim-test.o sim-test.c libMIPS.so

sim-mips:
	$(CC) -g -o sim-mips.o sim-mips.c 
