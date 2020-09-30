;*************************************************************************
; Example: 6-1
; Page: 200 / PDF Page: 214
; Chapter: 6.1 Shift Operators
; KEYWORD: <<; >>
; Question: Write codes to PB2 and PB4 of PORTB to 1 and clear the other pins
; a) without the directives
; b) using the directives
;*************************************************************************
; Solution
; a) LDI R20, 0x14  ; R20 = 0x14
;    LDI R20, 0b00010100  ; readable with binary
;    OUT PORTB, R20 ; PORTB = R20 = 0x14
; b) LDI R20, (1<<4) | (1<<2) ; R20 = (0b1000 | 0b00100) = 0b10100
;    LDI R20, (1<<PB4) | (1<<PB2) ; PB2=2, PB4=4, set the PB4/2 bits
;    OUT PORTB, R20
;*************************************************************************
#INCLUDE "M328PDEF.INC"
