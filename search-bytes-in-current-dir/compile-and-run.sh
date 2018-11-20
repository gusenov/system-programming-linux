#!/bin/bash

clear

gcc -Wall 1.c -o 1.exe -lpthread && ./1.exe --dir=/usr/include/ --string="stdio.h" --file="search-results.txt"

