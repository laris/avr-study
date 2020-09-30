#!/bin/sh

filename_ext=$1
filename=${filename_ext%.*}
./sh.avrasm2.sh ${filename}.asm
#./sh.avrasm2.sh ${filename}.inc
#./sh.avrasm2.sh ${filename}.INC
#./sh.avra.sh ${filename}.asm
sleep 2

#./sh.upload.sh ${filename}.hex
./sh.upload-cp2102.sh ${filename}.hex

