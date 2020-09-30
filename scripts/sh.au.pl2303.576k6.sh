#!/bin/bash

filename_with_ext=$1
filename=${filename_with_ext%.*}

./sh.asm.sh ${filename_with_ext}
sleep 2

./sh.up4.pl2303.57k6.sh ${filename}.hex
