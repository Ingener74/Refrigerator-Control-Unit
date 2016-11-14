#!/bin/sh
avr-gcc -mmcu=attiny13 -o main.o main.c
avr-objcopy -O ihex main.o main.hex
rm -f main.o *.*~