#!/bin/sh
app=$1
#serial_dev=$2
#serial_dev=$(ls /dev/tty.wchusbserial*)
#serial_dev="/dev/tty.wchusbserial1420"
#serial_dev=$(ls /dev/tty.SLAB_USBtoUART*)
serial_dev=$(ls /dev/cu.usbmodem142101)

picocom  -qrx 100 -b 1200 ${serial_dev}
sleep 1
avrdude -v -patmega32u4 -cavr109 -P${serial_dev} -b57600 -D -Uflash:w:${app}:i

# Leonardo m32u4
#Forcing reset using 1200bps open/close on port /dev/cu.usbmodem142101
#PORTS {/dev/cu.Bluetooth-Incoming-Port, /dev/cu.DSM-DevB, /dev/cu.LarisXMan-WirelessiAP, /dev/cu.POGO-DevB, /dev/cu.POGOPLUG-DevB, /dev/cu.TheGenius-SPPDev-2, /dev/cu.TheGenius-SPPDev-3, /dev/cu.usbmodem142101, /dev/tty.Bluetooth-Incoming-Port, /dev/tty.DSM-DevB, /dev/tty.LarisXMan-WirelessiAP, /dev/tty.POGO-DevB, /dev/tty.POGOPLUG-DevB, /dev/tty.TheGenius-SPPDev-2, /dev/tty.TheGenius-SPPDev-3, /dev/tty.usbmodem142101, } / {/dev/cu.Bluetooth-Incoming-Port, /dev/cu.DSM-DevB, /dev/cu.LarisXMan-WirelessiAP, /dev/cu.POGO-DevB, /dev/cu.POGOPLUG-DevB, /dev/cu.TheGenius-SPPDev-2, /dev/cu.TheGenius-SPPDev-3, /dev/cu.usbmodem142101, /dev/tty.Bluetooth-Incoming-Port, /dev/tty.DSM-DevB, /dev/tty.LarisXMan-WirelessiAP, /dev/tty.POGO-DevB, /dev/tty.POGOPLUG-DevB, /dev/tty.TheGenius-SPPDev-2, /dev/tty.TheGenius-SPPDev-3, /dev/tty.usbmodem142101, } => {}
#PORTS {/dev/cu.Bluetooth-Incoming-Port, /dev/cu.DSM-DevB, /dev/cu.LarisXMan-WirelessiAP, /dev/cu.POGO-DevB, /dev/cu.POGOPLUG-DevB, /dev/cu.TheGenius-SPPDev-2, /dev/cu.TheGenius-SPPDev-3, /dev/cu.usbmodem142101, /dev/tty.Bluetooth-Incoming-Port, /dev/tty.DSM-DevB, /dev/tty.LarisXMan-WirelessiAP, /dev/tty.POGO-DevB, /dev/tty.POGOPLUG-DevB, /dev/tty.TheGenius-SPPDev-2, /dev/tty.TheGenius-SPPDev-3, /dev/tty.usbmodem142101, } / {/dev/cu.Bluetooth-Incoming-Port, /dev/cu.DSM-DevB, /dev/cu.LarisXMan-WirelessiAP, /dev/cu.POGO-DevB, /dev/cu.POGOPLUG-DevB, /dev/cu.TheGenius-SPPDev-2, /dev/cu.TheGenius-SPPDev-3, /dev/tty.Bluetooth-Incoming-Port, /dev/tty.DSM-DevB, /dev/tty.LarisXMan-WirelessiAP, /dev/tty.POGO-DevB, /dev/tty.POGOPLUG-DevB, /dev/tty.TheGenius-SPPDev-2, /dev/tty.TheGenius-SPPDev-3, } => {}
#PORTS {/dev/cu.Bluetooth-Incoming-Port, /dev/cu.DSM-DevB, /dev/cu.LarisXMan-WirelessiAP, /dev/cu.POGO-DevB, /dev/cu.POGOPLUG-DevB, /dev/cu.TheGenius-SPPDev-2, /dev/cu.TheGenius-SPPDev-3, /dev/tty.Bluetooth-Incoming-Port, /dev/tty.DSM-DevB, /dev/tty.LarisXMan-WirelessiAP, /dev/tty.POGO-DevB, /dev/tty.POGOPLUG-DevB, /dev/tty.TheGenius-SPPDev-2, /dev/tty.TheGenius-SPPDev-3, } / {/dev/cu.Bluetooth-Incoming-Port, /dev/cu.DSM-DevB, /dev/cu.LarisXMan-WirelessiAP, /dev/cu.POGO-DevB, /dev/cu.POGOPLUG-DevB, /dev/cu.TheGenius-SPPDev-2, /dev/cu.TheGenius-SPPDev-3, /dev/cu.usbmodem142101, /dev/tty.Bluetooth-Incoming-Port, /dev/tty.DSM-DevB, /dev/tty.LarisXMan-WirelessiAP, /dev/tty.POGO-DevB, /dev/tty.POGOPLUG-DevB, /dev/tty.TheGenius-SPPDev-2, /dev/tty.TheGenius-SPPDev-3, /dev/tty.usbmodem142101, } => {/dev/cu.usbmodem142101, /dev/tty.usbmodem142101, }
#Found upload port: /dev/cu.usbmodem142101
#/Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega32u4 -cavr109 -P/dev/cu.usbmodem142101 -b57600 -D -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_225263/Blink.ino.hex:i 

# Arduino upload bootloader via arduino/serial/stk500
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -carduino -P/dev/cu.wchusbserial1420 -b115200 -D -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_215269/Blink.ino.hex:i 
# Arduino upload bootloader via usbasp
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbasp -Pusb -Uflash:w:/Applications/Arduino.app/Contents/Java/hardware/arduino/avr/bootloaders/optiboot/optiboot_atmega328.hex:i -Ulock:w:0x0F:m
# Arduino upload app via usbasp
# /Applications/Arduino.app/Contents/Java/hardware/tools/avr/bin/avrdude -C/Applications/Arduino.app/Contents/Java/hardware/tools/avr/etc/avrdude.conf -v -patmega328p -cusbasp -Pusb -Uflash:w:/var/folders/l1/4zq595012d5484z50z_1bytw0000gn/T/arduino_build_215269/Blink.ino.hex:i 


