#!/bin/bash

AVR_BIN_PATH=$HOME/GitCodes/AVR/avr-tools

serial_dev=$(ls /dev/tty.wchusbserial*)

fn_ext=$1
fn=${fn_ext%.*}

#echo $fn_ext
#echo $fn

#wine avrasm2.exe -v9 -fI -o ${fn}.hex ${fn_ext}
#wine $AVR_BIN_PATH/avrasm2.exe -fI -l ${fn}.lst -o ${fname}.hex ${fn_ext}
#rm -rf *.cof *.eep.hex *.obj *.lst *.map
#rm -rf *.cof *.eep.hex *.obj *.map

#rm -rf a.out 
#avr-gcc $fn_ext -o ${fn}.elf
#avr-objdump -Dzs ${fn}.elf
##avr-objcopy -j .text -j .data -O ihex ${fn}.elf ${fn}.hex
#avr-objcopy -j .text -O ihex ${fn}.elf ${fn}.hex
#sh.tn.up.sh ${fn}.hex


#picocom -c --imap 8bithex,spchex,nrmhex -b 115200 /dev/tty.usbserial
picocom -b 115200 ${serial_dev}
#app=$1
#serial_dev=$2
#serial_dev="/dev/tty.wchusbserial1420"

#serial_dev=$(ls /dev/tty.SLAB_USBtoUART*)
#avrdude -v -patmega328p -carduino -P${serial_dev} -b115200 -D -Uflash:w:${app}:i

# Arduino upload bootloader via arduino/serial/stk500
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/cu.wchusbserial1420 -b115200 -D -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_215269/Blink.ino.hex:i 
# Arduino upload bootloader via usbasp
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbasp -Pusb -Uflash:w:/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0x0F:m
# Arduino upload app via usbasp
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbasp -Pusb -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_215269/Blink.ino.hex:i 


