;#define __AVR_ATtiny85__
#define __AVR_ATmega328P__
// default offset 0x20 
##define __SFR_OFFSET  0x20
#include <avr/io.h>
#include "avr_macro.S"

#_bit_not_gpr r10, 0
#_bit_not_gpr r31, 7
#_bit_not r17, 1
#_bit_not 0x1f, 2
#_bit_not 0x23, 2
#_bit_not 0x5F, 2
#_bit_not 0x60, 3
#_bit_not PORTB, 1
#_bit_not SREG, 1

.macro _bit_wr_sfrl_tb
_bit_wr_sfrl DDRB, 1, 0
_bit_wr_sfrl DDRB, 7, 1
##define __SFR_OFFSET  0
_bit_wr_sfrl DDRB, 1, 0
_bit_wr_sfrl DDRB, 7, 1
.endm
#_bit_wr_sfrl

.macro _bit_wr_gprh_tb
_bit_wr_gprh r16, 0, 1
LDI r16,0xFF
_bit_wr_gprh r16, 0, 0
LDI r16,0xFF
_bit_wr_gprh r16, 7, 0
LDI r16,0x00
_bit_wr_gprh r16, 7, 1
.endm
#_bit_wr_sfrl

.macro _bit_wr_tb
_bit_wr r0,  0, 0
_bit_wr r10, 1, 1
_bit_wr r15, 7, 1, r18
#SREG = 0x3F/0x5F
_bit_wr SREG, 0, 0
_bit_wr SREG, 1, 1 
_bit_wr SREG, 7, 0, r19
.endm
#_bit_wr_tb

.macro _get_bit_tb
ldi r16,1
_get_bit r16,0
_get_bit r16,1
_mcp r16, SREG
_get_bit SREG,0
_get_bit SREG,1
_mcp r16, 0x60
_get_bit 0x60,0
_get_bit 0x60,1
.endm
#_get_bit_tb

