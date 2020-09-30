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
.org  _VECTORS_SIZE
#include "avr_lib_delay_8bit_inc.S"
_init_delay_8bit_cycle 24, 25
#include "avr_lib_delay_8bit.S"
#include "avr_gpio.S"

_jp_init_sp:
        _init_sp
_jp_init_led:
//        _bit_wr   DDRB, PB1,1,rhtmp
        _gpio_ddrx_xpin_set_out DDRB, PB1

.global main
main:

#define loop_cnt_us R16
#define loop_cnt_ms R17
#        _mwrk 0xff, delayCNT_reg  // 255 x 62.5ns = 15.9375us
#        _mwrk 0xff, loop_cnt_us   // 256 x 15.9375us = 4.08ms
#        _mwrk 245, loop_cnt_ms   // 1000/4.08ms = 245

jp_loop_infinite:
        _mwrk 245, loop_cnt_ms
        _do
            _mwrk 0xff, loop_cnt_us
            _do
                _mwrk 0xff, delayCNT_reg  // 255 x 62.5ns = 15.9375us
                RCALL funcDelayNCycle8_reg
            DEC   loop_cnt_us
            _while nz
        DEC loop_cnt_ms
        _while nz

        //_bit_not PORTB,PB1
        _gpio_portx_xpin_toggle PORTB, PB1

        RJMP jp_loop_infinite
        
_jp_end:
        RJMP _jp_end


