#!/bin/bash

clear

rm *.out
rm myfifo

gcc -Wall server.c -o server.out && ./server.out &
gcc -Wall client.c userfunc.h userfunc.c -o client.out && ./client.out
