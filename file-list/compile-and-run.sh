#!/bin/bash

clear

rm *.out
gcc -Wall program.c -o program.out && ./program.out
