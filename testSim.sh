#!/bin/sh

#this script will run all the tests in the sim-test.c file

gcc -g sim-test.c -shared -o libMIPS.so
gcc -g -o t sim-test.c libMIPS.so

./t