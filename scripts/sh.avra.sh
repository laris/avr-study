#!/bin/bash
PWD=$HOME/GitCodes/AVR/avr-tools

fname_ext=$1

if [ -z $fname_ext ]; then
	echo "No input assembly file!"
else
	fname=${fname_ext%.*}
	#wine avrasm2.exe -v9 -fI -o ${fname}.hex ${fname_ext}
	avra -l ${fname}.lst -o ${fname}.hex ${fname_ext}
	rm -rf *.cof *.eep.hex *.obj *.map
fi
