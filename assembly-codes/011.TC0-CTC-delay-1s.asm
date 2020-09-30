.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"

; example to use TC0 to generate 1s delay and toggle blink LED PB5 on/off
; TC0 use CTC mode
; max avalible T=0.01s, so 1s need 100 times x 0.01s = 1s
; using OCR0A INT as ISR to calculate loop and invert LED
; test pass
.CSEG
.ORG $0
    RJMP INIT
;.ORG $40

; TC ISR jmp address
;.ORG  OC2Aaddr
;      RJMP ISR_OC2A
;.ORG  OC2Baddr
;      RJMP ISR_OC2B
;.ORG  OVF2addr
;      RJMP ISR_OVF2
;.ORG  ICP1addr
;      RJMP ISR_ICP1
;.ORG  OC1Aaddr
;      RJMP ISR_OC1A
;.ORG  OC1Baddr
;      RJMP ISR_OC1B
;.ORG  OVF1addr
;      RJMP ISR_OVF1
.ORG  OC0Aaddr
      RJMP ISR_OC0A
;.ORG  OC0Baddr
;      RJMP ISR_OC0B
;.ORG  OVF0addr
;      RJMP ISR_OVF0

INIT:
        INIT_SP
        ; init IO PIN
        SETB DDRB,DDB5        ; config PB5 as output
        ; init TC config
        ; init TC0
        CFG_SET_TCN_CTLRNAME_MODN 0,WGM,0b010     ; config WGM, 010-CTC-OCR0A
        CFG_SET_TCN_CTLRNAME_MODN 0,CS,0b101      ; config CS, 101-1024
        ;CFG_SET_TCN_CTLRNAME_MODN 0,COMA,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_TCN_CTLRNAME_MODN 0,COMB,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR0B,FOC0A   ; config FOC0A
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR0B,FOC0B   ; config FOC0B
        CFG_SET_IOREGNAME_1_REGBIT TIMSK0,OCIE0A  ; config COMIE0A
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK0,OCIE0B  ; config COMIE0B
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK0,TOIE0   ; config TOIE0
        ;RAMWRAIR TCNT0,0,R16                      ; write TCNT0
        RAMWRAIR OCR0A,156,R16                    ; write OCR0A, 156 -> 0.01 s
        ;RAMWRAIR OCR0B,0,R16                      ; write OCR0B
        CFG_WR_IOREGNAME_TRH TIMSK0,R16           ; write TIMSK0
        SEI                                       ; global Interrupt
        CFG_WR_IOREGNAME_TRH TCCR0A,R16           ; write TCCR0A
        CFG_WR_IOREGNAME_TRH TCCR0B,R16           ; write TCCR0B, now start
        ; init TC1
        ;RAMWRAIR TCNT1H,HIGH($1E85),R16
        ;RAMWRAIR TCNT1L, LOW($1E85),R16
        ;RAMWRAIR OCR1AH,HIGH($1E85),R16
        ;RAMWRAIR OCR1AL, LOW($1E85),R16
        ;RAMWRAIR OCR1BH,HIGH($1E85),R16
        ;RAMWRAIR OCR1BL, LOW($1E85),R16
        ;CFG_WR_IOREGNAME_TRH TCCR1A,R16
        ;CFG_WR_IOREGNAME_TRH TCCR1B,R16
        ; init TC2
        ;RAMWRAIR TCNT2,0,R16       ; 
        ;RAMWRAIR OCR2B,0,R16       ;
        ; TC config finish
        .SET T10MSCNT=99      ; 1s = 100 x 0.01
        ;.SET T10MSCNT=5
        LDI R17,T10MSCNT
MAIN:
        RJMP MAIN
ISR_OC0A:
SR_CHK_T10MSCNT:
        TST R17
        BRBS SREG_Z, JP_RELOAD_T10MSCNT
        DEC R17
        RJMP JP_ISR_OC0A_EXIT
  JP_RELOAD_T10MSCNT:
        LDI R17,T10MSCNT
  JP_LEDINVERT:
        RAMBNOTABRH PORTB,PB5,R16
  JP_ISR_OC0A_EXIT:
        RETI
