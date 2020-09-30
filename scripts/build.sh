#!/bin/bash

rm -rf a.out 
avr-gcc  tn85_blink.S 
avr-objdump -dz a.out
avr-objcopy -j .text -O ihex a.out out.hex
