#INCLUDE "M328PDEF.INC"
#DEFINE F_CPU 16000000
;#INCLUDE "WAIT.ASM"
#INCLUDE "delay_ms.inc.asm"

      LDI R20, 0xFF   ; init R20=0xFF
;     OUT DDRB, R20
      OUT PORTB, R20  ; set PORTB = 0xFF, PORTB as output
LOOP:
      SBI PORTB, 5    ; turn on PORTB5
      CALL DELAY     ; call delay
      CBI PORTB, 5    ; turn off PORTB5
      CALL DELAY     ; delay
      CALL LOOP      ; loop
DELAY:
      LDI ZH, 0
      LDI ZL, 0x1
      CALL DELAY_MS
