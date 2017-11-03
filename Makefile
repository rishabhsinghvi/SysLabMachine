CC = gcc

all: libMIPS.so sim-test sim-mips

libMIPS.so: sim-mips.c
	$(CC) -g sim-mips.c -shared -o libMIPS.so

sim-test: sim-test.c sim-mips.c libMIPS.so
	$(CC) -g -o sim-test sim-test.c libMIPS.so