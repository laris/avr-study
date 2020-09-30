#INCLUDE "M328PDEF.INC"
;#DEFINE F_CPU 16000000
;#INCLUDE "WAIT.ASM"

      LDI R20, 0XFF   ; init R20=0xFF
      OUT DDRB, R20
      OUT PORTB, R20  ; set PORTB = 0xFF, PORTB as output
;      SBI PORTB, 5    ; turn on PORTB5
; LOOP:
;       SBI PORTB, 5    ; turn on PORTB5
;       RCALL DELAY     ; call delay
;       CBI PORTB, 5    ; turn off PORTB5
;       RCALL DELAY     ; delay
;       RCALL LOOP      ; loop
; DELAY:
;       LDI R16, 10000      ; load 1000 to R16, 1000ms = 1s
;       CALL WaitMiliseconds
