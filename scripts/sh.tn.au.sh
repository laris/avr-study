#!/bin/bash

filename_with_ext=$1
filename=${filename_with_ext%.*}

./sh.asm.sh ${filename_with_ext}
sleep 2

./sh.tn.up.sh ${filename}.hex
