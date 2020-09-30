#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"

.section .text
.org  0
      RJMP _jp_init_sp
.org  TIM0_OVF_vect_num*2    // 5
      RJMP _isr_OVF0
#.org  TIM0_COMPA_vect_num*2  // 10
#      RJMP _isr_OC0A_16bTC
#.org  TIM0_COMPB_vect_num*2  // 11
#      RJMP
.org  _VECTORS_SIZE
_SW_INT_VECTOR_JP1:
      RJMP _icall_loop_blink
#include "avr_lib_tnx5_tc.S"

_jp_init_sp:
        _init_sp
_jp_init_led:
        _bit_wr   DDRB, PB1,1,rhtmp1
_jp_init_cnt:
#define loop_cnt_15us9 R28
#define loop_cnt_4ms08 R29
        _mwrk 0, loop_cnt_15us9   // 256 x 15.9375us = 4.08ms
        _mwrk 0, loop_cnt_4ms08   // 1000/4.08ms = 245
_jp_init_tc0:
/* TOIE0
*/
        _mwrk <1!<!<CS00>,  REG_TCCR0B
        _mwrk <1!<!<TOIE0>, REG_TIMSK
/* OCIE0A
        _mwrk <1!<!<CS00>,  REG_TCCR0B
        _mwrk 0xff,         REG_OCR0A
        _mwrk <1!<!<OCIE0A>, REG_TIMSK
*/

        RCALL func_tnx5_tc0_init_reg
        SEI
        RCALL func_tnx5_tc0_start_reg
.global main
main:
// 8bit
#        _mwrk 0xff, delayCNT_reg  // 255 x 62.5ns = 15.9375us
#        _mwrk 0xff, loop_cnt_15us9   // 256 x 15.9375us = 4.08ms
#        _mwrk 245, loop_cnt_4ms08   // 1000/4.08ms = 245
// 16bit
##define loop_cnt_4ms08 R17
#        _mwrk 0xff, delayCNTL_reg  // 
#        _mwrk 0xff, delayCNTH_reg  // 0xFFFF x 62.5ns = 4.0959375 ms
#        _mwrk 244, loop_cnt_4ms08   // 1000/4.0959375 ms = 244
#        _mwrk 244, loop_cnt_4ms08   // 500/4.0959375 ms = 122

_jp_end:
        RJMP _jp_end

_icall_loop_blink:
        _if <CPI loop_cnt_15us9,0xff>, eq, else
                INC   loop_cnt_15us9 // 0xff -> 0
                INC   loop_cnt_4ms08 // ++
            #_if <CPI loop_cnt_4ms08,244>, eq, Eif
            _if <CPI loop_cnt_4ms08,122>, eq, Eif
                _mwrk 0,loop_cnt_4ms08      // reset
                _mwrk 0,loop_cnt_15us9
                _bit_not PORTB,PB1
            _Eif
        _else
            INC   loop_cnt_15us9
        _Eif
RET
