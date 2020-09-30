#!/bin/bash

raw_file=$1
#list=$(cat $raw_file | grep _SFR_IO8 | tr -s '\t' ' ' | cut -d ' ' -f 2  | tr -s '\n' ' ')
list=$(cat $raw_file | grep _SFR_IO8 | tr -s '\t' ' ' | cut -d ' ' -f 2  | tr -s '\n' ',' | rev | cut -c 2- | rev)
echo $list
