CC = gcc

sim-test: sim-test.c sim-mips.c
	$(CC) -g sim-mips.c -shared -o libMIPS.so
	$(CC) -g -L$$PWD -Wl,-rpath=$$PWD -o sim-test.out sim-test.c libMIPS.so

sim-mips:
	$(CC) -g -o sim-mips.out sim-mips.c 
