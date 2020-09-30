.CSEG
.ORG  0
        CLR R0
        LDI R16,1
        LDI R17,1
        LDI R18,1

        ADD R0,R16
        ADD R0,R17
        ADD R0,R18

        ;RJMP PC
        ;RJMP PC-1      ; PE 2.4
