#!/bin/sh

#this script will run all the tests in the sim-test.c file

gcc -g sim-mips.c -shared -o libMIPS.so
gcc -g -o sim-test sim-test.c libMIPS.so

./sim-test