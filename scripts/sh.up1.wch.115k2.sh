#!/bin/sh
app=$1
#serial_dev=$2
#serial_dev="/dev/tty.wchusbserial1420"
serial_dev=$(ls /dev/tty.wchusbserial*)
#serial_dev=$(ls /dev/tty.SLAB_USBtoUART*)
avrdude -v -patmega328p -carduino -P${serial_dev} -b115200 -D -Uflash:w:${app}:i

# Arduino upload bootloader via arduino/serial/stk500
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/cu.wchusbserial1420 -b115200 -D -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_215269/Blink.ino.hex:i 
# Arduino upload bootloader via usbasp
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbasp -Pusb -Uflash:w:/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0x0F:m
# Arduino upload app via usbasp
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbasp -Pusb -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_215269/Blink.ino.hex:i 


