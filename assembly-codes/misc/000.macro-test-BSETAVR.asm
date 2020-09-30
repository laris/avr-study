.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
; PASS
INIT_SP

MAIN:
        LDI R16, 7
        ;>3F
        ;.equ    UDR0    = 0xc6  ; MEMORY MAPPED
        BSETAVR UDR0,R16,R18
        NOP
        LDS  R0,UDR0
        PUSH R0
        NOP
        ;<=3F
        ;.equ    TCNT0   = 0x26
        LDI R16, 6
        BSETAVR TCNT0,R16,R18
        NOP
        IN R0,TCNT0
        PUSH R0
        NOP
        ; <1F
        ;.equ    DDRB    = 0x04
        LDI R16, 5
        BSETAVR DDRB,R16,R18
        NOP
        IN R0,DDRB
        PUSH R0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP

        LDI R16, 1
        ;>3F
        ;.equ    UDR0    = 0xc6  ; MEMORY MAPPED
        BSETAVR UDR0,R16,R18
        NOP
        LDS  R0,UDR0
        PUSH R0
        NOP
        ;<=3F
        ;.equ    TCNT0   = 0x26
        LDI R16, 1
        BSETAVR TCNT0,R16,R18
        NOP
        IN R0,TCNT0
        PUSH R0
        NOP
        ; <1F
        ;.equ    DDRB    = 0x04
        LDI R16, 1
        BSETAVR DDRB,R16,R18
        NOP
        IN R0,DDRB
        PUSH R0
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP
        NOP

        LDI R16, 1
        ;>3F
        ;.equ    UDR0    = 0xc6  ; MEMORY MAPPED
        BCLRAVR UDR0,R16,R18
        NOP
        LDS  R0,UDR0
        PUSH R0
        NOP
        ;<=3F
        ;.equ    TCNT0   = 0x26
        LDI R16, 1
        BCLRAVR TCNT0,R16,R18
        NOP
        IN R0,TCNT0
        PUSH R0
        NOP
        ; <1F
        ;.equ    DDRB    = 0x04
        LDI R16, 1
        BCLRAVR DDRB,R16,R18
        NOP
        IN R0,DDRB
        PUSH R0

        
        