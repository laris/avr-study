#!/bin/bash

vavrdisasm --assembly -c atmega328p -t ihex $1
