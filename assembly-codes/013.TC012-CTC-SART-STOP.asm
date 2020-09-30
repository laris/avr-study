.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"

; example to use TC0 to generate 1s delay and toggle blink LED PB5 on/off
; TC0/1/2 use CTC mode
; TC0/2 CS=1024, max avalible T=0.01s/OCR=156, so 1s need 100 times x 0.01s = 1s
; TC1   CS=1024, max avalible T=1s/$3D09/15625,so 1s need 1 times x 1s = 1s
; test pass
.CSEG
.ORG $0
    RJMP INIT
;.ORG $40

; TC ISR jmp address
.ORG  OC2Aaddr
      RJMP ISR_OC2A
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
        ;********************************************************************** 
        INIT_SP
        ;********************************************************************** 
        ; init TC config
        ;********************************************************************** 
        ; init TC0 MODE and function
        CFG_SET_TCN_CTLRNAME_MODN 0,WGM,0b010     ; config WGM, 010-CTC-OCR0A
        CFG_SET_TCN_CTLRNAME_MODN 0,CS,0b101      ; config CS, 101-1024
        ;CFG_SET_TCN_CTLRNAME_MODN 0,COMA,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_TCN_CTLRNAME_MODN 0,COMB,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR0B,FOC0A   ; config FOC0A
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR0B,FOC0B   ; config FOC0B
        ; init TC0 INT
        CFG_SET_IOREGNAME_1_REGBIT TIMSK0,OCIE0A  ; config COMIE0A
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK0,OCIE0B  ; config COMIE0B
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK0,TOIE0   ; config TOIE0
        ; init TC0 write counters IOREG
        ;RAMWRAIR TCNT0,0,R16                      ; write TCNT0
        ;RAMWRAIR OCR0A,0,R16                      ; write OCR0A
        RAMWRAIR OCR0A,156,R16                    ; write OCR0A, 156=10ms
        ;RAMWRAIR OCR0B,0,R16                      ; write OCR0B
        ; init TC0 config write to IOREG
        CFG_WR_IOREGNAME_TRH TIMSK0,R16           ; write TIMSK0
        SEI                                       ; global Interrupt
        CFG_WR_IOREGNAME_TRH TCCR0A,R16           ; write TCCR0A
        CFG_WR_IOREGNAME_TRH TCCR0B,R16           ; write TCCR0B, now start
        ;********************************************************************** 
        ; init TC1 MODE and function
        ;CFG_SET_TCN_CTLRNAME_MODN 1,WGM,0b0100    ; config WGM, 0100-CTC-OCR1A
        ;CFG_SET_TCN_CTLRNAME_MODN 1,CS,0b101      ; config CS, 101-1024
        ;CFG_SET_TCN_CTLRNAME_MODN 1,COMA,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_TCN_CTLRNAME_MODN 1,COMB,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR1B,ICNC1   ; config ICNC1
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR1B,ICES1   ; config ICES1
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR1C,FOC1A   ; config FOC1A
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR1C,FOC1B   ; config FOC1B
        ; init TC1 INT
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK1,ICIE1   ; config ICIE1
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK1,OCIE1A  ; config COMIE1A
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK1,OCIE1B  ; config COMIE1B
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK1,TOIE1   ; config TOIE1
        ; init TC1 write counters IOREG
        ;RAMWRAIR TCNT1H,0,R16                     ; write TCNT1H
        ;RAMWRAIR TCNT1L,0,R16                     ; write TCNT1L
        ;RAMWRAIR OCR1AH,0,R16                     ; write OCR1AH
        ;RAMWRAIR OCR1AL,0,R16                     ; write OCR1AL
        ;RAMWRAIR OCR1BH,0,R16                     ; write OCR1BH
        ;RAMWRAIR OCR1BL,0,R16                     ; write OCR1BL
        ; init TC1 config write to IOREG
        ;CFG_WR_IOREGNAME_TRH TIMSK1,R16           ; write TIMSK1
        ;SEI                                       ; global Interrupt
        ;CFG_WR_IOREGNAME_TRH TCCR1A,R16           ; write TCCR1A
        ;CFG_WR_IOREGNAME_TRH TCCR1C,R16           ; write TCCR1C
        ;CFG_WR_IOREGNAME_TRH TCCR1B,R16           ; write TCCR1B, now start
        ;********************************************************************** 
        ; init TC2 MODE and function
        CFG_SET_TCN_CTLRNAME_MODN 2,WGM,0b010     ; config WGM, 010-CTC-OCR2A
        CFG_SET_TCN_CTLRNAME_MODN 2,CS,0b111      ; config CS, 101-1024
        ;CFG_SET_TCN_CTLRNAME_MODN 2,COMA,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_TCN_CTLRNAME_MODN 2,COMB,0b01    ; config COMA/B, 01-toggle
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR2B,FOC2A   ; config FOC2A
        ;CFG_SET_IOREGNAME_1_REGBIT TCCR2B,FOC2B   ; config FOC2B
        ; init TC2 INT
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK2,TOIE2   ; config TOIE2
        CFG_SET_IOREGNAME_1_REGBIT TIMSK2,OCIE2A  ; config COMIE2A
        ;CFG_SET_IOREGNAME_1_REGBIT TIMSK2,OCIE2B  ; config COMIE2B
        ; init TC2 write counters IOREG
        ;RAMWRAIR TCNT2,0,R16                      ; write TCNT2
        ;RAMWRAIR OCR2A,0,R16                      ; write OCR2A
        RAMWRAIR OCR2A,156,R16                    ; write OCR2A, 156 -> 0.01 s
        ;RAMWRAIR OCR2B,0,R16                      ; write OCR2B
        ; init TC2 config write to IOREG
        CFG_WR_IOREGNAME_TRH TIMSK2,R16           ; write TIMSK2
        SEI                                       ; global Interrupt
        CFG_WR_IOREGNAME_TRH TCCR2A,R16           ; write TCCR2A
;STOP2  
        ;CFG_WR_IOREGNAME_TRH TCCR2B,R16           ; write TCCR2B, now start
        ;********************************************************************** 
        ; Variable for TC
        .SET T0_10MS_CNT=99                       ; 1s = 100 x 0.01
        ;LDI R17,T0_10MS_CNT
        ;.SET T1_1S_CNT=0                          ; no need counter
        .SET T2_10MS_CNT=99                       ; 1s = 100 x 0.01
        ;LDI R18,T2_10MS_CNT
        .SET INVERT_CNT=10                        ; invert TC0/2 every 10 times
        ;LDI R19,INVERT_CNT
        ;********************************************************************** 
        ; init IO PIN
        SETB DDRB,DDB5                            ; config PB5 as output
        ;********************************************************************** 

MAIN:
        RJMP MAIN

        .SET TC0_OCR0A_SAVE_ADR=0x100
        .SET TC1_OCR1AH_SAVE_ADR=0x200
        .SET TC1_OCR1AL_SAVE_ADR=0x201
        .SET TC2_OCR2A_SAVE_ADR=0x300
        .SET MSG_ADR=0x400


ISR_OC0A:
SR_CHK_T0_10MS_CNT:
        TST R17
        BRBS SREG_Z, JP_RELOAD_T0_10MS_CNT
        DEC R17
        RJMP JP_ISR_OC0A_EXIT
  JP_RELOAD_T0_10MS_CNT:
        LDI R17,T0_10MS_CNT
  JP_LEDINVERT_0:
        RAMBNOTABRH PORTB,PB5,R16
  JP_ISR_OC0A_EXIT:
        RETI
ISR_OC2A:
SR_CHK_T2_10MS_CNT:
        TST R18
        BRBS SREG_Z, JP_RELOAD_T2_10MS_CNT
        DEC R18
        RJMP JP_ISR_OC2A_EXIT
  JP_RELOAD_T2_10MS_CNT:
        LDI R18,T2_10MS_CNT
  JP_LEDINVERT_2:
        RAMBNOTABRH PORTB,PB5,R16
  JP_ISR_OC2A_EXIT:
        RETI

