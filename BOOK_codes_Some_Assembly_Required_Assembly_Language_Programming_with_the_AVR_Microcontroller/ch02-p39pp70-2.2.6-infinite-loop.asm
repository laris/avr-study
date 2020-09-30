.CSEG
.ORG  $0
        LDI   R16,  1
        ADD   R16,  R16
        ; method 1
        ;.dw  $CFFF
        ; method 2
        ;RJMP 0x02
HERE:   ; method 3
        RJMP HERE
        ; method 4
        ;RJMP PC
        ; method 5
        ;RJMP -1