#!/bin/sh

#this script will run all the tests in the sim-test.c file

#gcc -g sim-mips.c -shared -o libMIPS.so
#gcc -g -o sim-test.o sim-test.c libMIPS.so


gcc -g -o sim-mips.o sim-mips.c #gcc -g -o sim-mips.o -fpic sim-mips.c
gcc -shared -o libMIPS.so sim-mips.o
gcc -g -o sim-test.o sim-test.c libMIPS.so


./sim-test.o
