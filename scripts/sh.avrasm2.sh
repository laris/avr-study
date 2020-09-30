#!/bin/bash
AVR_BIN_PATH=$HOME/GitCodes/AVR/avr-tools
fname_ext=$1
fname=${fname_ext%.*}
#echo $fname_ext
#echo $fname

#wine avrasm2.exe -v9 -fI -o ${fname}.hex ${fname_ext}
wine $AVR_BIN_PATH/avrasm2.exe -fI -l ${fname}.lst -o ${fname}.hex ${fname_ext}
#rm -rf *.cof *.eep.hex *.obj *.lst *.map
rm -rf *.cof *.eep.hex *.obj *.map
