#!/bin/bash

AVR_BIN_PATH=$HOME/GitCodes/AVR/avr-tools

fn_ext=$1
fn=${fn_ext%.*}

#echo $fn_ext
#echo $fn

#wine avrasm2.exe -v9 -fI -o ${fn}.hex ${fn_ext}
#wine $AVR_BIN_PATH/avrasm2.exe -fI -l ${fn}.lst -o ${fname}.hex ${fn_ext}
#rm -rf *.cof *.eep.hex *.obj *.lst *.map
#rm -rf *.cof *.eep.hex *.obj *.map

rm -rf a.out 
avr-gcc $fn_ext -o ${fn}.elf
avr-objdump -Dzs ${fn}.elf
#avr-objcopy -j .text -j .data -O ihex ${fn}.elf ${fn}.hex
avr-objcopy -j .text -O ihex ${fn}.elf ${fn}.hex
sh.tn.up.sh ${fn}.hex
