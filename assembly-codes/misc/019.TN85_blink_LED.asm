.INCLUDE "TN85DEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
; ***** INTERRUPT VECTORS ************************************************
;.equ	INT0addr	= 0x0001	; External Interrupt 0
;.equ	PCI0addr	= 0x0002	; Pin change Interrupt Request 0
;.equ	OC1Aaddr	= 0x0003	; Timer/Counter1 Compare Match 1A
;.equ	OVF1addr	= 0x0004	; Timer/Counter1 Overflow
;.equ	OVF0addr	= 0x0005	; Timer/Counter0 Overflow
;.equ	ERDYaddr	= 0x0006	; EEPROM Ready
;.equ	ACIaddr	= 0x0007	; Analog comparator
;.equ	ADCCaddr	= 0x0008	; ADC Conversion ready
;.equ	OC1Baddr	= 0x0009	; Timer/Counter1 Compare Match B
;.equ	OC0Aaddr	= 0x000a	; Timer/Counter0 Compare Match A
;.equ	OC0Baddr	= 0x000b	; Timer/Counter0 Compare Match B
;.equ	WDTaddr	= 0x000c	; Watchdog Time-out
;.equ	USI_STARTaddr	= 0x000d	; USI START
;.equ	USI_OVFaddr	= 0x000e	; USI Overflow
;.equ	INT_VECTORS_SIZE	= 15	; size in words
; ***** INTERRUPT VECTORS ************************************************
;-------------------------------------------------------------------------------
.CSEG
.ORG $0
      RJMP INIT
.ORG OC0Aaddr
      RJMP ISR_OC0A
.ORG OC0Baddr
      RJMP ISR_OC0B
.ORG INT_VECTORS_SIZE
;-------------------------------------------------------------------------------
INIT:
        INIT_SP
;-------INIT PORT---------------------------------------------------------------
        SETB  DDRB,DDB0
        SETB  DDRB,DDB1
        SETB  DDRB,DDB2
        .EQU  LEDPIN=PB1
;-------INIT TC0----------------------------------------------------------------
; about 14MHz @ VCC=4.1v, 71.4ns/cycle, 255 cycle = 74576c/overflow
        RAMWRAIR  OCR0A,  255,R16
        RAMWRAIR  OCR0B,  255,R16
        RAMWRAIR  TIMSK,  (1<<OCIE0A) | (1<<OCIE0B) ,R16
        RAMWRAIR  TCCR0A, (1<<COM0A0) |(1<<COM0B0) | (1<<WGM01),R16 ; M2-CTC
        RAMWRAIR  TCCR0B, (1<<CS02)  | (1<<CS00) ,R16
;-------INIT INT----------------------------------------------------------------
        SEI
;-------------------------------------------------------------------------------
MAIN:
        LDI   R20, 0b0000_1111
        RJMP MAIN
;-------------------------------------------------------------------------------
ISR_OC0A:
        .SET CNT0=7 
        TST R17
        BREQ JP_RELOAD_CNT0
        DEC R17
        RJMP JP_ISR_OC0A_EXIT
  JP_RELOAD_CNT0:
        LDI R17, CNT0
  JP_OUTPUT:
        RAMBNOTABRH PORTB,PB2,R19
  JP_ISR_OC0A_EXIT:
        RETI
;-------------------------------------------------------------------------------
ISR_OC0B:
SR_CHK_TCNT:
        .SET TCNT=255
        TST R18
        BRBS SREG_Z, JP_RELOAD_TCNT
        DEC R18
        RJMP JP_ISR_OC0B_EXIT
  JP_RELOAD_TCNT:
        LDI R18,TCNT
  JP_LEDINVERT:
        RAMBNOTABRH PORTB,LEDPIN,R16
  JP_ISR_OC0B_EXIT:
        RETI
;-------------------------------------------------------------------------------
