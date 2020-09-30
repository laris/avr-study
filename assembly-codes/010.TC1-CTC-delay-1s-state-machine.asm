.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
;.equ	OC2Aaddr	= 0x000e	; Timer/Counter2 Compare Match A
;.equ	OC2Baddr	= 0x0010	; Timer/Counter2 Compare Match A
;.equ	OVF2addr	= 0x0012	; Timer/Counter2 Overflow
;.equ	ICP1addr	= 0x0014	; Timer/Counter1 Capture Event
;.equ	OC1Aaddr	= 0x0016	; Timer/Counter1 Compare Match A
;.equ	OC1Baddr	= 0x0018	; Timer/Counter1 Compare Match B
;.equ	OVF1addr	= 0x001a	; Timer/Counter1 Overflow
;.equ	OC0Aaddr	= 0x001c	; TimerCounter0 Compare Match A
;.equ	OC0Baddr	= 0x001e	; TimerCounter0 Compare Match B
;.equ	OVF0addr	= 0x0020	; Timer/Couner0 Overflow

.CSEG
.ORG $0
    RJMP INIT
.ORG OC1Aaddr
    RJMP ISR_OC1A
;.ORG $40
INIT:
        INIT_SP
        SETB DDRB,DDB5        ; config PB5 as output
        CFG_SET_TCN_CTLRNAME_MODN 1,WGM,0b0100
        CFG_SET_TCN_CTLRNAME_MODN 1,CS,0b101
        CFG_SET_IOREGNAME_1_REGBIT TIMSK1,OCIE1A
        CFG_WR_IOREGNAME_TRH TIMSK1,R16
        SEI
        RAMWRAIR OCR1AH,HIGH($1E85),R16
        RAMWRAIR OCR1AL, LOW($1E85),R16
        CFG_WR_IOREGNAME_TRH TCCR1A,R16
        CFG_WR_IOREGNAME_TRH TCCR1B,R16
        .SET DELAYN=10
        LDI R17,DELAYN     ; set delay 5 sec to toggle
MAIN:
;        RCALL SR_CHK_T
;        RCALL SR_LEDBLINK
;        RCALL SR_CHK_T
;        RCALL SR_LEDBLINK
;        RCALL SR_CHK_T
;        RCALL SR_LEDBLINK
;        RCALL SR_CHK_T
;        RCALL SR_LEDBLINK
;        RCALL SR_CHK_T
;        RCALL SR_LEDBLINK
;        RCALL SR_CHK_T
;        RCALL SR_LEDBLINK
        RJMP MAIN
SR_LEDBLINK:
        BRBS SREG_T, JP_LEDOFF
        RAMBNOTABRH PORTB,PB5,R16
        RET
    JP_LEDOFF:
        CLRB PORTB,PB5
        RET
SR_CHK_T:
        TST R17
        BRBS SREG_Z, JP_SET
        DEC R17
        RET
    JP_SET:
        BNOTSREGTR R16
        LDI R17,DELAYN
        RET
ISR_OC1A:
        RCALL SR_CHK_T
        RCALL SR_LEDBLINK
        RETI
