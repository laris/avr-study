#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
/* default offset 0x20 */
##define __SFR_OFFSET  0x20
#include <avr/io.h>
##define __DEBUG_GET_PRT_TYPE__ 1
##define __DEBUG_GET_AADDR__ 1
##include "avr_macro_common.S"
##include "avr_macro_dtrans.S"
##include "avr_macro_control.S"

/* http://uzebox.org/wiki/index.php?title=Assembler_Tips&oldid=5226
Timing
    nop        ; 1 cycle
    rjmp  .    ; 2 cycles
    lpm        ; 3 cycles - but destroys a register (r0 if plain "lpm")
To kill off 3N cycles: (N > 0)
.macro delay3N value
    ldi   r19, \value
    dec   r19
    brne  .-4
.endm
You can use any register r16 or above for the counter. 
The branch costs one less cycle on the last iteration, 
but that is "paid for" by the LDI instruction up front.

To kill off variable number of cycles
You may also write a routine which waits an arbitrary amount of cycles as follows:

This produces a delay of 12 cycles 
(excluding the CALL or RCALL used to call it), when r24 is 4. 
By incrementing r24, you can increment the delay cycle by cycle, 
up to 267 (r24 = 3, after wrapping around).
 */

.section .text
.org 0
rjmp _jp_delay_tst

_jp_delay_tst:
    ldi r24, 0
    rcall delay_cycles
    nop
    ldi r24, 1
    rcall delay_cycles
    nop
    ldi r24, 2
    rcall delay_cycles
    nop
    ldi r24, 255
    rcall delay_cycles
    nop

delay_cycles:
    lsr   r24
    brcs  .    ; +1 if bit0 was set
    lsr   r24
    brcs  .    ; +1 if bit1 was set
    brcs  .    ; +1 if bit1 was set
    dec   r24
    nop
    brne  .-6  ; 4 cycle loop
    ret

