#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"

#.section .data
#include "avr_lib_mem_pool_m.S"
_mem_pool_data_init 0x60, 248, 2
.section .text
.org  0
      RJMP _jp_init_sp
#.org  TIM0_COMPA_vect_num*2
#      RJMP _isr_OC0A_16bTC
.org  _VECTORS_SIZE

_jp_init_sp:
      _init_sp

main:
      RJMP main
NOP