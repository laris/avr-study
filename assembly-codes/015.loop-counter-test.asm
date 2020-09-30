.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"

        PLD_0A1P $100,Z
        LDI   R16, $FF
LOOP:
        ; do somthing
        ST    Z+,R16
        DEC   R16
        BRNE  LOOP

        LDI   R16,$FF
        ST    Z+,R16

JP_END:
        RJMP  JP_END


