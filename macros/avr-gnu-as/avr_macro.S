##define __AVR_ATtiny85__
##define __AVR_ATmega328P__
##include <avr/io.h>

#ifndef __AVR_MACRO__
#define __ABR_MACRO__
#endif

#include "avr_macro_common.S"
#include "avr_macro_dtrans.S"
##define __DEBUG_MCP_P__ 1
##define __DEBUG_PMCP__ 1
#include "avr_macro_arithlogic.S"
#include "avr_macro_control.S"
##define __DEBUG_GET_PRT_TYPE__ 1
##define __DEBUG_GET_AADDR__ 1
##define __DEBUG_IF_BIT__ 1
##define __DEBUG_IFB_TRUE_SKIP__ 1
#include "avr_macro_stack.S"
#include "avr_macro_misc.S"


