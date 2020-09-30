#!/bin/bash

hexfile=$(pwd)/$1
#echo $hexfile
#./micronucleus_v2.04_macos_10.14.3_exec --dump-progress --fast-mode --run $1
micronucleus_v2.04_macos_10.14.3_exec --fast-mode --run $1

