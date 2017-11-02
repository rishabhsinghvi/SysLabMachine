#!/bin/sh

gcc -g sim-test.c -shared -o libMIPS.so
gcc -g -o t sim-test.c libMIPS.so

./t