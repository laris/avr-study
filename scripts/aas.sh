#!/bin/bash
fname=$1

avr-as -a $fname
avr-objdump -d a.out
