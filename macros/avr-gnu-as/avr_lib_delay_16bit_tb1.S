#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"

.section .text
.org  0
      RJMP _jp_init_sp
#.org  TIM0_COMPA_vect_num*2
#      RJMP _isr_OC0A_16bTC
#.org  _VECTORS_SIZE
#include "avr_lib_delay_16bit.S"

_jp_init_sp:
        _init_sp
_jp_init_led:
        _bit_wr   DDRB, PB1,1,rhtmp

.global main
main:
// 8bit
##define loop_cnt_us R16
##define loop_cnt_ms R17
#        _mwrk 0xff, delayCNT_reg  // 255 x 62.5ns = 15.9375us
#        _mwrk 0xff, loop_cnt_us   // 256 x 15.9375us = 4.08ms
#        _mwrk 245, loop_cnt_ms   // 1000/4.08ms = 245
// 16bit
#define loop_cnt_ms R17
#        _mwrk 0xff, delayCNTL_reg  // 
#        _mwrk 0xff, delayCNTH_reg  // 0xFFFF x 62.5ns = 4.0959375 ms
#        _mwrk 244, loop_cnt_ms   // 1000/4.0959375 ms = 244
#        _mwrk 244, loop_cnt_ms   // 500/4.0959375 ms = 122

jp_loop_infinite:
        #_mwrk 244, loop_cnt_ms
        _mwrk 244/2, loop_cnt_ms
        _do
                _mwrk 0xff, delayCNTL_reg  // 
                _mwrk 0xff, delayCNTH_reg  // 0xFFFF x 62.5ns = 4.0959375 ms
                RCALL funcDelayNCycle16_reg
        DEC loop_cnt_ms
        _while nz

        _bit_not PORTB,PB1
        RJMP jp_loop_infinite
        
_jp_end:
        RJMP _jp_end


