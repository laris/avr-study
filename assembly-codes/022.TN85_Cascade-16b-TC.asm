.INCLUDE "TN85DEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
.INCLUDE "AVR_MACRO_MISC1.INC"
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
;*******************************************************************************
.DSEG                               ; allocate a tx buffer
.EQU FIFO_SIZE    = 64
;A_FIFO_BASE_RX:  .BYTE (FIFO_SIZE+3)
A_FIFO_BASE_TX:  .BYTE (FIFO_SIZE)
;*******************************************************************************
.CSEG
.ORG $0
      RJMP INIT
.ORG OC1Aaddr
      RJMP ISR_OC1A
.ORG OC1Baddr
      RJMP ISR_OC1B
.ORG OC0Aaddr
      RJMP ISR_OC0A
.ORG OC0Baddr
      RJMP ISR_OC0B
.ORG INT_VECTORS_SIZE
;-------------------------------------------------------------------------------
INIT:
        INIT_SP
;-------INIT PORT---------------------------------------------------------------
        SETB  DDRB,DDB0 ; PB0/OC0A wave out
        SETB  DDRB,DDB1 ; PB1/OC1A/LED out go into PB2/T0
        CLRB  DDRB,DDB2 ; PB2/set as input for T0
        SETB  DDRB,DDB4 ; PB4/OC1B
        .EQU  LEDPIN=PB1
;-------INIT_TXBF------------------------------------------------------------
        ;LDI   R16,0b0000_1111
        LDI   R16,0b0101_0101
        PLD_0A1P A_FIFO_BASE_TX,Z
        ST    Z,R16
;-------------------------------------------------------------------------------
;*******************************************************************************
;FPWM TC0 
        ;RAMWRAIR  OCR0A,127,R16 ; 50% duty, 8us(7.7)=128*0.0625
        ;RAMWRAIR  TCCR0A,(1<<COM0A1)|(1<<WGM01)|(1<<WGM00),R16 ; FPWM-TOP-$FF
        ;RAMWRAIR  TCCR0B,(1<<CS00),R16
        ;RAMWRAIR  TCCR0A,(1<<COM0A1)|(1<<WGM01)|(1<<WGM00),R16 ; FPWM-TOP-OCRA
        ;RAMWRAIR  TCCR0B,(1<<WGM02)|(1<<CS00),R16
;ctc    
        ;RAMWRAIR  OCR0A,255, R16  ; get 0.125*128=16us pulse width, why?
        ;RAMWRAIR  TCCR0A,(1<<COM0A0)|(1<<WGM01),R16 ; ctc+OC,
        ;RAMWRAIR  TCCR0B,(1<<CS00),R16
;normal  
        ;RAMWRAIR  OCR0A,255, R16 ; get 16us pulse width, 16/0.0625=256
        ;RAMWRAIR  TCCR0A,(1<<COM0A0),R16 ; normal+OC
        ;RAMWRAIR  TCCR0B,(1<<CS00),R16
;-------------------------------------------------------------------------------
;FPWMA TC1
        ;RAMWRAIR  OCR1C,  255, R16 ; TOP 
       ;RAMWRAIR  OCR1A,  127, R16 ; 50% duty = (1+OCRnA)/256,8us/7.7=128*0.0625
        ;RAMWRAIR  TCCR1,(1<<PWM1A)|(1<<COM1A0)|(1<<CS10),R16
;ctc    
        ;RAMWRAIR  OCR1A,255, R16 ; get 0.125*128=16us pulse width, why?
        ;RAMWRAIR  TCCR1,(1<<CTC1)|(1<<COM1A0)|(1<<CS10),R16;ctc+OC
;normal 
        ;RAMWRAIR  OCR1A,255, R16 ; get 0.0625*256=16us(15.5) pulse width
        ;RAMWRAIR  TCCR1,(1<<COM1A0)|(1<<CS10),R16 ; normal+OC
;*******************************************************************************
; cascade T0+T1 as 16 bit timer
; https://www.avrfreaks.net/forum/cascade-two-8-bit-timers-make-16bit-timer
; TC1 as low 8-bit timer, TC0 as high 8-bit timer
; PB1/PIN6/OC1A - OC1A output as TC0 input
; PB2/PIN7/T0   - T0 as input clock source
; connect PB1-PB2, TC1's OC1A as output and input TC0 T0
; PB0/PIN5/OC0A - OC0A output as waveform output
; TC0 OC0A toggle output as waveform, working in CTC
; TC1 work in normal + OC firstly, if OC ISR occurr, change to 
;-------INIT TC0----------------------------------------------------------------
;-------------------------------------------------------------------------------
; fix1=19, fix2=18
;.EQU  OCR_16BIT = 1 ; test low=1, get 17.94us=.0625*287
;.EQU  OCR_16BIT = 10 ; test low=10, get 3us=.0625*48
;.EQU  OCR_16BIT = 20 ; test low=20, get 3.625us=.0625*58 , 3.688us=58
;.EQU  OCR_16BIT = 30 ; test low=30, get 4.25us=.0625*68
;.EQU  OCR_16BIT = 40 ; get 4.812us=.0625*77, 4.875us=78
;.EQU  OCR_16BIT = 50 ; get 5.438us=.0625*87, 5.5us=88
;.EQU  OCR_16BIT = 100 ; get 8.438us=135, 8.5us=.0625*136
;.EQU  OCR_16BIT = 200 ; get 14.56us=233
;.EQU  OCR_16BIT = 255 ; get 17.88us=286
;.EQU  OCR_16BIT = 265 ; get 4.375us=70  33.44us=535
;.EQU  OCR_16BIT = 512 ; $0200 get 49.88us=799  49.81us=797
;.EQU  OCR_16BIT = 767 ; $02FF get 49.69us=795
;.EQU  OCR_16BIT = 65535 ; get 4.066ms=65056  4.096ms=65536
;-------------------------------------------------------------------------------
; TC0 as high byte, output, 
; work for 16-bit cascade TC
;.EQU  OCR_16BIT = 1666 ; $0682=0D1666, 105.8us=1693
;.EQU  OCR_16BIT = 30300
.EQU  OCR_16BIT = $FFFF/4 ; test for 8-bit mode
;        ;RAMWRAIR  TIMSK,  (1<<OCIE0A),R16        ; skip, enable OCIE0A in TC1
;        RAMWRAIR  OCR0A,HIGH(OCR_16BIT),R16
;        RAMWRAIR  TCCR0A,(1<<COM0A0)|(1<<WGM01),R16           ; COM0A0, M2-CTC
;        RAMWRAIR  TCCR0B,(1<<CS02)|(1<<CS01)|(1<<CS00),R16 ; T0 rising triger
; test for 16b-TC and diff man encoding
        ;RAMWRAIR  TIMSK,  (1<<OCIE0A),R16        ; skip, enable OCIE0A in TC1
        RAMWRAIR  OCR0A,HIGH(OCR_16BIT),R16
        RAMWRAIR  OCR0B,HIGH(OCR_16BIT/2),R16     ; set half FOC pulse
        RAMWRAIR  TCCR0A,(1<<COM0A0)|(1<<WGM01),R16           ; COM0A0, M2-CTC
        RAMWRAIR  TCCR0B,(1<<CS02)|(1<<CS01)|(1<<CS00),R16 ; T0 rising triger
;-------INIT TC1----------------------------------------------------------------
; TC1 as low byte, output to TC0 T0
; work for 16-bit cascade TC
;        RAMWRAIR  TIMSK,  (1<<OCIE1A)|(1<<OCIE0A),R16           ; enable OCIE
;        RAMWRAIR  OCR1A,  LOW(OCR_16BIT),R16                    ; initial OCR1A
;        RAMWRAIR  TCCR1,  (1<<CS10),R16   ; normal mode, disable COM1A0 toggle
; test TC1 and diff mancode encoding
;        RAMWRAIR  TIMSK,  (1<<OCIE1A)|(1<<OCIE1B),R16
;        RAMWRAIR  OCR1A,  255,R16
;        RAMWRAIR  OCR1B,  (127-6-6-12),R16 ; 6 ISR + 6 unknow cost + 12 rcall
;        RAMWRAIR  TCCR1,  (1<<COM1A0)|(1<<CS10),R16
; TC1 as low byte, output to TC0 T0
; test for 16b-TC and diff man encoding
        RAMWRAIR  TIMSK,(1<<OCIE0B)|(1<<OCIE0A)|(1<<OCIE1A),R16 ; enable OCIE
        RAMWRAIR  OCR1A,LOW(OCR_16BIT),R16                ; initial OCR1A
        RAMWRAIR  TCCR1,(1<<CS10),R16
;-------INIT INT----------------------------------------------------------------
        SEI
;-------------------------------------------------------------------------------
MAIN:
;  JP_LOOP1:
;        LDI R16,63
;        SETB PORTB,LEDPIN
;        DEC R16
;        BRNE JP_LOOP1
;  JP_LOOP2:
;        LDI R16,63
;        CLRB PORTB,LEDPIN
;        DEC R16
;        BRNE JP_LOOP2
; flip-flop to detect freq, result, 0.125us/pulse, 16MHz
;        SETB PORTB,LEDPIN   ; sbi 1c
;        CLRB PORTB,LEDPIN   ; cbi 1c
JP_ALWAYS_TX_ENCODE_CHK_LOOP:
        SBRC  SREGRh_TX,FLG_TXC; if 1=sending, skip exit=0=idle, set & do tx
        RJMP  JP_ALWAYS_TX_ENCODE_CHK_LOOP  ; if 0=idle, exit

  JP_SR_DIFFMANCODE_ENCODE_LD_TXB_1:
        PLD_0A1P A_FIFO_BASE_TX,Z
        LD    ENCODE_TXB,Z
        SBR   SREGRh_TX,(1<<FLG_TXC)        ; set TXC
        RJMP  JP_ALWAYS_TX_ENCODE_CHK_LOOP  ; if 0=idle, exit

        RJMP MAIN           ; 2c
;-------------------------------------------------------------------------------
SR_DIFFMANCODE_TX_SYNC_HEADER_CALL_BY_OC_ISR:
.EQU I_OCR_1T   = OCR_16BIT/257
.EQU I_OCR_2T   = 1; 2*OCR_16BIT
.EQU I_OCR_4T   = 1; 4*OCR_16BIT
.DEF REG_CNT    = R16
.DEF REG_TMP1   = R17
.EQU ADR_OCR    = TCCR0A
.EQU ADR_FOC    = TCCR0B
.EQU BIT_FOC    = FOC0A
;.DEF  SREGRh_TX     = R19
.EQU FLG_TX_SYNC_0x1T   = 0 ;
.EQU FLG_TX_SYNC_3x1T   = 1 ;
.EQU FLG_TX_SYNC_2x2T   = 2 ;
.EQU FLG_TX_SYNC_1x4T   = 3 ;
.EQU FLG_TX_SYNC_1x2T   = 4 ;
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CHK_TXS:
        SBRS  SREGRh_TX,FLG_TXS ; if 1=sending, skip exit, do tx
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_INIT_SET_1T:
        SBRC  SREGRh_TX,FLG_TX_SYNC_0x1T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CHK_nT_STS
;        SETB      ADR_FOC,BIT_FOC,REG_TMP1
        RAMWRAIR  ADR_OCR,I_OCR_1T,REG_TMP1
        SBR   SREGRh_TX,FLG_TX_SYNC_0x1T
        CLR   REG_CNT
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CHK_nT_STS:
        SBRS  SREGRh_TX,FLG_TX_SYNC_3x1T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_3x1T_SET_2T
        SBRS  SREGRh_TX,FLG_TX_SYNC_2x2T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_2x2T_SET_4T
        SBRS  SREGRh_TX,FLG_TX_SYNC_1x4T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_1x4T_SET_2T
        SBRS  SREGRh_TX,FLG_TX_SYNC_1x2T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_1x2T_SET_1T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_MARK_TXC_1_BUSY_CAN_TX
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_3x1T_SET_2T:
        INC   REG_CNT
        CPI   REG_CNT,4
        BRLO  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
        RAMWRAIR  ADR_OCR,I_OCR_2T,REG_TMP1
        CLR   REG_CNT
        SBR   SREGRh_TX,FLG_TX_SYNC_3x1T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_2x2T_SET_4T:
        INC   REG_CNT
        CPI   REG_CNT,2
        BRLO  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
        RAMWRAIR  ADR_OCR,I_OCR_4T,REG_TMP1
        CLR   REG_CNT
        SBR   SREGRh_TX,FLG_TX_SYNC_2x2T
        RJMP  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_1x4T_SET_2T:
        ;INC   REG_CNT
        ;CPI   REG_CNT,1
        ;BRLO  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
        RAMWRAIR  ADR_OCR,I_OCR_2T,REG_TMP1
        SBR   SREGRh_TX,FLG_TX_SYNC_1x4T
        ;CLR   REG_CNT
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_CNT_1x2T_SET_1T:
        ;INC   REG_CNT
        ;CPI   REG_CNT,1
        ;BRLO  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT
        RAMWRAIR  ADR_OCR,I_OCR_1T,REG_TMP1
        SBR   SREGRh_TX,FLG_TX_SYNC_1x2T
        ;CLR   REG_CNT
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_RCALL_ENCODE:
        ;RCALL SR_DIFFMANCODE_ENCODE_TC_OC_FOC_START
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_MARK_TXC_1_BUSY_CAN_TX:
        SBR   SREGRh_TX,FLG_TXC
  JP_SR_DIFFMANCODE_TX_SYNC_HEADER_EXIT:
        RET
;-------------------------------------------------------------------------------
SR_DIFFMANCODE_ENCODE_TC_OC_FOC_START:
.EQU  REG_FOC       = TCCR0B ; GTCCR ; TC1 ; TC0 TCCR0B
.EQU  REG_FOC_BIT   = FOC0A  ; FOC1A ; TC1 ; TC0 FOC0A
.DEF  ENCODE_TXB    = R17
.DEF  RhT1          = R18
.DEF  SREGRh_TX     = R19
;.EQU  FLG_HLF       = 7     ; 0=no send, 1=sent first half logic value
.EQU  FLG_TXS       = 7     ; TX Complete, 0=TXC, 1=tx/sending/busy
.EQU  FLG_TXC       = 6     ; TX Complete, 0=TXC, 1=tx/sending/busy
.EQU  FLG_TX_CNT0   = 0     ; 1
.EQU  FLG_TX_CNT1   = 1     ; 2
.EQU  FLG_TX_CNT2   = 2     ; 4
.EQU  FLG_TX_CNT3   = 3     ; 8
; before TX, must set FLG_TXC, then call TX
;       SBR   SREGRh_TX,FLG_TXC
  JP_SR_DIFFMANCODE_ENCODE_CHK_TXC:
        SBRS  SREGRh_TX,FLG_TXC; if 1=sending, skip exit=0=idle, do tx
        RJMP  JP_SR_DIFFMANCODE_ENCODE_EXIT ; if 0=idle, exit
;  JP_SR_DIFFMANCODE_ENCODE_CHK_HLF:
;        SBRC  SREGRh_TX,FLG_HLF ; if clr/TX-0, skip set/foc, do  
;        RJMP  JP_SR_DIFFMANCODE_ENCODE_CLR_HLF_FLG_DO_FOC ;if sent hlf, clr foc
;  JP_SR_DIFFMANCODE_ENCODE_SET_HLF_FLG:           ; if clr, set
;        SBR   SREGRh_TX,(1<<FLG_HLF)              ; set sent half flag
;  JP_SR_DIFFMANCODE_ENCODE_LD_TXB:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        LD    ENCODE_TXB,Z
        CLC
  JP_SR_DIFFMANCODE_ENCODE_ROL_TXB:
        ROL   ENCODE_TXB
  JP_SR_DIFFMANCODE_ENCODE_INC_TX_BITS_CNT:
        INC   SREGRh_TX                           ; +1/tx-1-bit
  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_BITS_CNT:
        SBRS  SREGRh_TX,FLG_TX_CNT3 ; if1,tx 8,clr TXC do encode,if0,do encode
        RJMP  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_1
  JP_SR_DIFFMANCODE_ENCODE_TXC_MARK_TXC_CLR_CNT:
        CBR   SREGRh_TX,(1<<FLG_TXC)              ; mark TXC
        CBR   SREGRh_TX,0b00001111                ; clr counter
  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_1:
        BRCS  JP_SR_DIFFMANCODE_ENCODE_EXIT       ; set/TX-1, skip foc, exit
  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_0:              ; clr/TX-0, do foc
;        RJMP  JP_SR_DIFFMANCODE_ENCODE_TX_FOC
;  JP_SR_DIFFMANCODE_ENCODE_CLR_HLF_FLG_DO_FOC:
;        CBR   SREGRh_TX,(1<<FLG_HLF)              ; if set, clr
  JP_SR_DIFFMANCODE_ENCODE_TX_FOC:                ; force output
;        LDS   RhT1,REG_FOC                        ; load TC REG
;        SBR   RhT1,(1<<REG_FOC_BIT)               ; set FOC bit
;        STS   REG_FOC,RhT1                        ; write TC REG
        ;RAMWRAIR  REG_FOC,(1<<REG_FOC_BIT),RhT1
        SETB   REG_FOC,REG_FOC_BIT,RhT1
  JP_SR_DIFFMANCODE_ENCODE_EXIT:
        RET
;-------------------------------------------------------------------------------
;SR_DIFFMANCODE_ENCODE_TOGGLE_PIN_START:
;.EQU  REG_FOC       = GTCCR ; TC1 ; TC0 TCCR0B
;.EQU  REG_FOC_BIT   = FOC1A ; TC1 ; TC0 FOC0A
;.DEF  ENCODE_TXB    = R17
;.DEF  RhT1          = R18
;.DEF  SREGRh_TX     = R19
;.EQU  FLG_HLF       = 7     ; 0=no send, 1=sent first half logic value
;.EQU  FLG_TXC       = 6     ; TX Complete, 0=TXC, 1=tx/sending/busy
;.EQU  FLG_TX_CNT0   = 0     ; 1
;.EQU  FLG_TX_CNT1   = 1     ; 2
;.EQU  FLG_TX_CNT2   = 2     ; 4
;.EQU  FLG_TX_CNT3   = 3     ; 8
;; before TX, must set FLG_TXC, then call TX
;;       SBR   SREGRh_TX,FLG_TXC
;  JP_SR_DIFFMANCODE_ENCODE_CHK_TXC:
;        SBRS  SREGRh_TX,FLG_TXC; if 1=sending, skip exit=0=idle, do tx
;        RJMP  JP_SR_DIFFMANCODE_ENCODE_EXIT ; if 0=idle, exit
;  JP_SR_DIFFMANCODE_ENCODE_CHK_HLF:
;        SBRC  SREGRh_TX,FLG_HLF ; if clr/TX-0, skip set/foc, do  
;        RJMP  JP_SR_DIFFMANCODE_ENCODE_CLR_HLF_FLG_DO_FOC ;if sent hlf, clr foc
;  JP_SR_DIFFMANCODE_ENCODE_SET_HLF_FLG:           ; if clr, set
;        SBR   SREGRh_TX,(1<<FLG_HLF)              ; set sent half flag
;  JP_SR_DIFFMANCODE_ENCODE_LD_TXB:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        LD    ENCODE_TXB,Z
;        CLC
;  JP_SR_DIFFMANCODE_ENCODE_ROL_TXB:
;        ROL   ENCODE_TXB
;  JP_SR_DIFFMANCODE_ENCODE_INC_TX_BITS_CNT:
;        INC   SREGRh_TX                           ; +1/tx-1-bit
;  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_BITS_CNT:
;        SBRC  SREGRh_TX,FLG_TX_CNT3 ; if 1,tx 8bits, clr TXC, if 0, skip next
;        CBR   SREGRh_TX,(1<<FLG_TXC)
;  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_1:
;        BRCS  JP_SR_DIFFMANCODE_ENCODE_EXIT       ; set/TX-1, skip foc, exit
;  JP_SR_DIFFMANCODE_ENCODE_CHK_TX_0:              ; clr/TX-0, do foc
;        RJMP  JP_SR_DIFFMANCODE_ENCODE_TX_FOC
;  JP_SR_DIFFMANCODE_ENCODE_CLR_HLF_FLG_DO_FOC:
;        CBR   SREGRh_TX,(1<<FLG_HLF)              ; if set, clr
;  JP_SR_DIFFMANCODE_ENCODE_TX_FOC:                ; force output
;        LDS   RhT1,REG_FOC                        ; load TC REG
;        SBR   RhT1,(1<<REG_FOC_BIT)               ; set FOC bit
;        STS   REG_FOC,RhT1                        ; write TC REG
;  JP_SR_DIFFMANCODE_ENCODE_EXIT:
;        RET
;-------------------------------------------------------------------------------
ISR_OC1A: ;2 ISR vector;  cost 1+2+2+1+3+3 = 12 cycle
.DEF  RhTMP = R16
;  JP_ISR_OCR1A_STOP_TC1:
;        LDI   RhTMP,(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10) ;1 ;
;        OUT   TCCR1,RhTMP         ;1 ; stop TC1, cost 1+2 cycle, low >=3
  JP_ISR_OC1A_CHK_COM1A0_SET:      
        IN    RhTMP,TCCR1         ;1 ; 
        SBRC  RhTMP,COM1A0        ;12; if COM1A0 clr, skip exit and go set
        RJMP  JP_ISR_OC1A_EXIT    ;2 ; COM1A set, skip set and exit
;        SBRC  RhTMP,CTC1          ;  ; if 8-bit mode, skip exit and set 8bit
;        RJMP  JP_ISR_OC1A_EXIT    ;  ; yes 8-bit, exit
  JP_ISR_OC1A_CHK_TC0_OCR0A_0_TRUE_SET_TC_8BIT:;  ; if only low byte need count?
        LDS   RhTMP,OCR0A         ;2 ;
        CLZ                       ;1 ;
        TST   RhTMP               ;1 ;
        BREQ  JP_ISR_OC1A_TC_8BIT_TCCR1_SET_CTC1_COM1A0  ;12; if high=0, force out and exit
  JP_ISR_OC1A_RELOAD_OCR1A_FF:
;        RAMWRAIR TCNT1,19,RhTMP   ;3 ; fix1 cycle cost,20-1
        RAMWRAIR OCR1A,$FF,RhTMP  ;3 ; if TCNT1 reach low, ISR reset to $FF
        RAMWRAIR TCCR1,(1<<COM1A0)|(1<<CS10),RhTMP ;3 ; on COM1A0, start TC1
        RJMP JP_ISR_OC0A_EXIT
  JP_ISR_OC1A_RELOAD_TCNT1_RESTART_TC1:
        RAMWRAIR  TCNT1,18,RhTMP          ;3 ; fix2 cycle cost
  JP_ISR_OC1A_TC_8BIT_TCCR1_SET_CTC1_COM1A0:
        RAMWRAIR  TCCR1,(1<<CTC1)|(1<<COM1A0)|(1<<CS10),RhTMP
        ;RAMWRAIR  TCCR1,(1<<CS10),RhTMP   ;3 ; restart TC1, cost 19c-1=18
  JP_ISR_OC1A_FOC1A: ; if only low byte work, set ctc and oc1a, start
        RAMWRAIR  GTCCR,(1<<FOC1A),RhTMP  ;3 ;
  JP_ISR_OC1A_EXIT:
        RETI                    ;4 ;
;-------------------------------------------------------------------------------
ISR_OC1A_2:
        ;LDI   R20,(1<<FOC1A)
;        LDI   R20,0b0000_0100
;        OUT   GTCCR,R20
        RETI
ISR_OC1B_2:   ; cost = fix 4c + LDI 1c + OUT 1c
;        RAMWRAIR GTCCR,(1<<FOC1A),R16   ; FOC1A @ middle
        RCALL SR_DIFFMANCODE_ENCODE_TC_OC_FOC_START
        RETI
ISR_OC1B:
        RETI
;-------------------------------------------------------------------------------
ISR_OC0A_2:
        RETI
ISR_OC0A:
  JP_ISR_OCR0A_STOP_TC1:
;        LDI   RhTMP,(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10) ;1 ;
;        OUT   TCCR1,RhTMP         ;1 ; stop TC1, cost 1+2 cycle, low >=3
        RAMWRAIR  OCR1A,LOW(OCR_16BIT),RhTMP  ;3 ;
        RAMWRAIR  TCCR1,(1<<CS10),RhTMP       ;3 ;
;        RAMWRAIR  TCNT1,15,RhTMP               ;3 ; fix3 cycle cost
  JP_ISR_OC0A_EXIT:
        RETI
;-------------------------------------------------------------------------------
ISR_OC0B:
        RCALL SR_DIFFMANCODE_ENCODE_TC_OC_FOC_START
  RETI
ISR_OC0B_0:
;SR_CHK_TCNT:
;        .SET TCNT=255
;        TST R18
;        BRBS FLG_Z, JP_RELOAD_TCNT
;        DEC R18
;        RJMP JP_ISR_OC0B_EXIT
;  JP_RELOAD_TCNT:
;        LDI R18,TCNT
;  JP_LEDINVERT:
;;        RAMBNOTABRH PORTB,LEDPIN,RhTMP
;  JP_ISR_OC0B_EXIT:
        RETI
;-------------------------------------------------------------------------------
