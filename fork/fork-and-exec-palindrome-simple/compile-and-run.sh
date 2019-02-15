#!/bin/bash

clear

rm *.out
gcc -Wall child.c userfunc.h userfunc.c -o child.out && gcc -Wall parent.c -o parent.out && ./parent.out
