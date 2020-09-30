/*
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"
*/

#define TX_BUF0   R0
#define TX_BUF1   R1
#define TX_CNT    R2

#define TX_PORT    PORTB
#define TX_DDR     DDRB
#define TX_PIN     PB1

func_mod_output_init:
    _bit_wr   TX_DDR, TX_PIN, 1
    RET
func_mod_set_idle_0:
    _bit_wr   TX_PORT, TX_PIN, 0
    RET
func_mod_set_idle_1:
    _bit_wr   TX_PORT, TX_PIN, 1
    RET
func_mod_gen_bit_1xBUF:
    LSR   TX_BUF1
    RET
func_mod_gen_bit_2xBUF:
    LSR   TX_BUF1
    ROR   TX_BUF0
    RET
func_mod_output:
    _if <>, cs, else
        _bit_wr  TX_PORT, TX_PIN, 1
    _else
        _bit_wr  TX_PORT, TX_PIN, 0
        NOP
    _Eif
    RET


