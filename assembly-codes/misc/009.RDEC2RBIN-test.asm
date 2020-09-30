.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
; PASS
INIT_SP

MAIN:
        LDI R16, 7
        RDEC2RBIN R16,R17
        PUSH R17

        LDI R16, 6
        RDEC2RBIN R16,R17
        PUSH R17

        LDI R16, 5
        RDEC2RBIN R16,R17
        PUSH R17
 
        LDI R16, 4
        RDEC2RBIN R16,R17
        PUSH R17
 
        LDI R16, 3
        RDEC2RBIN R16,R17
        PUSH R17
 
        LDI R16, 2
        RDEC2RBIN R16,R17
        PUSH R17
 
        LDI R16, 1
        RDEC2RBIN R16,R17
        PUSH R17

        LDI R16, 0
        RDEC2RBIN R16,R17
        PUSH R17


        LDI R16, 7
LOOP:
        RDEC2RBIN R16,R17
        PUSH R17
        DEC R16
        BRNE LOOP
        RDEC2RBIN R16,R17
        PUSH R17

        RJMP MAIN
