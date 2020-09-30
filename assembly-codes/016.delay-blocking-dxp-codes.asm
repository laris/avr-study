.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"

.CSEG
.ORG 0x0
      RJMP MAIN

MAIN:
SETB DDRB,DDB5,R16

.EQU DELAY_S = 10
.EQU DELAY_MS = 100
LOOP:
        RAMBNOTABRH PORTB,PB5,R16
        LDI R16, DELAY_S
        RCALL WaitSecLoop
        ;RCALL WaitMsLoop
        RJMP LOOP


.INCLUDE "DELAY_ASM_dxp.pl_2008_busy-delay-assembly-utilities.inc"