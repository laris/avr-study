#!/bin/sh

#picocom -c -b 115200 /dev/tty.SLAB_USBtoUART

picocom -c --imap 8bithex,spchex,nrmhex -b 115200 /dev/tty.SLAB_USBtoUART
