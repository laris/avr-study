.CSEG
.ORG  0
RJMP MAIN
.DEF  LED = R16
.DEF  SWC = R17
.DEF  TMP = R18

.EQU PORTB  = 0x18
.EQU DDRB   = 0x17
.EQU PINB   = 0x16
; ***** PORTB ************************
; PORTB - Data Register, Port B
.equ	PORTB0	= 0	; 
.equ	PB0	= 0	; For compatibility
.equ	PORTB1	= 1	; 
.equ	PB1	= 1	; For compatibility
.equ	PORTB2	= 2	; 
.equ	PB2	= 2	; For compatibility
.equ	PORTB3	= 3	; 
.equ	PB3	= 3	; For compatibility
.equ	PORTB4	= 4	; 
.equ	PB4	= 4	; For compatibility
.equ	PORTB5	= 5	; 
.equ	PB5	= 5	; For compatibility

; DDRB - Data Direction Register, Port B
.equ	DDB0	= 0	; 
.equ	DDB1	= 1	; 
.equ	DDB2	= 2	; 
.equ	DDB3	= 3	; 
.equ	DDB4	= 4	; 
.equ	DDB5	= 5	; 

; PINB - Input Pins, Port B
.equ	PINB0	= 0	; 
.equ	PINB1	= 1	; 
.equ	PINB2	= 2	; 
.equ	PINB3	= 3	; 
.equ	PINB4	= 4	; 
.equ	PINB5	= 5	; 

; TN85
; PB0 -> switch -> GND
; PB1 -> LED -> R -> GND
MAIN:
        LDI TMP, ~(1<<DDB0)|(1<<DDB1)
        OUT DDRB,TMP
        LDI TMP,$FF
        OUT PORTB,TMP     ; pullup PINB0

;        RJMP PC


WAIT_PRESS:
        ;IN    SWC,PINB
        SBIC  PINB,PINB0
        RJMP  WAIT_PRESS

LED_TOGGLE:
        SBIS  PINB,PINB1
        RJMP  LED_ON
LED_OFF:
        CBI   PORTB,DDB1
        RJMP  WAIT_RELEASE
LED_ON:
        SBI   PORTB,DDB1

WAIT_RELEASE:
        ;IN    SWC,PINB
        SBIS  PINB,PINB0
        RJMP  WAIT_RELEASE
        ;CBI   PORTB,DDB1
        RJMP  WAIT_PRESS
