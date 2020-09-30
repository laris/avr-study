#!/bin/sh

./sh.batch2.sh $1
picocom -b 115200 /dev/tty.SLAB_USBtoUART
