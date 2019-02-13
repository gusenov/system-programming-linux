#!/bin/bash

clear

gcc -Wall child.c -o child.out && gcc -Wall parent.c userfunc.h userfunc.c -o parent.out && ./parent.out
