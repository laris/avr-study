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
.CSEG
.ORG $0
     RJMP INIT
.ORG OC1Aaddr
     RJMP ISR_OC1A
.ORG OC0Aaddr
     RJMP ISR_OC0A
;.ORG OC0Baddr
     ;RJMP ISR_OC0B
;-------------------------------------------------------------------------------
.ORG INT_VECTORS_SIZE
;-------------------------------------------------------------------------------
INIT:
        INIT_SP
;-------INIT PORT---------------------------------------------------------------
        SETB  DDRB,DDB0 ; PB0/OC0A wave out
        SETB  DDRB,DDB1 ; PB1/OC1A out go into PB2/T0
;        SETB  DDRB,DDB2 ; PB2/set as input for T0
;        CLRB  DDRB,DDB2 ; PB2/set as input for T0
        .EQU  LEDPIN1=PB1
;-------------------------------------------------------------------------------
;*******************************************************************************
;CTC-OCRA
        RAMWRAIR  OCR0A,255,R16 ;us=111*0.0625
        RAMWRAIR  TCCR0A,(1<<COM0A0)|(1<<WGM01),R16 ; CTC-OCRA
        RAMWRAIR  TCCR0B,(1<<CS01),R16
;FPWM-TOP-$FF TC0 
        ;RAMWRAIR  OCR0A,127,R16 ; 50% duty, 8us(7.7)=128*0.0625
        ;RAMWRAIR  TCCR0A,(1<<COM0A1)|(1<<WGM01)|(1<<WGM00),R16 ; FPWM-TOP-$FF
        ;RAMWRAIR  TCCR0B,(1<<CS00),R16
;FPWM-OCRA
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
;normal 
        RAMWRAIR  OCR1A,255, R16 ; get 0.0625*160=10us pulse width
        RAMWRAIR  TIMSK,(1<<OCIE1A)|(1<<OCIE0A),R16 ; enable OCIE
        ;RAMWRAIR  TCCR1,(1<<COM1A0)|(1<<CS10),R16 ; normal+OC
        RAMWRAIR  TCCR1,(1<<CS10),R16 ; normal+OC
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
;-------------------------------------------------------------------------------
        ;RAMWRAIR  TIMSK,  (1<<OCIE1A)|(1<<OCIE0A),R16 ; enable OCIE
;-------------------------------------------------------------------------------
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
;.EQU  OCR_16BIT = 1666 ; $0682=0D1666, 105.8us=1693
;.EQU  OCR_16BIT = 220
;        ;RAMWRAIR  TIMSK,  (1<<OCIE0A),R16  ; skip, enable OCIE0A in TC1 
;        RAMWRAIR  OCR0A,HIGH(OCR_16BIT),R16  
;        RAMWRAIR  TCCR0A,(1<<COM0A0)|(1<<WGM01),R16 ; COM0A0, M2-CTC
;        RAMWRAIR  TCCR0B,(1<<CS02)|(1<<CS01)|(1<<CS00),R16 ; T0 rising triger
;-------INIT TC1----------------------------------------------------------------
;        RAMWRAIR  TIMSK,  (1<<OCIE1A)|(1<<OCIE0A),R16 ; enable OCIE
;        RAMWRAIR  OCR1A,  LOW(OCR_16BIT),R16 ; initial OCR1A
;        RAMWRAIR  TCCR1,  (1<<CS10),R16 ; normal mode, disable COM1A0 toggle
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
        RJMP MAIN           ; 2c
;-------------------------------------------------------------------------------
;ISR_OC1A: ;2 ISR vector;  cost 1+2+2+1+3+3 = 12 cycle
;.DEF  RhTMP = R16
;  JP_ISR_OCR1A_STOP_TC1:
;        LDI   RhTMP,(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10) ;1 ;
;        OUT   TCCR1,RhTMP         ;1 ; stop TC1, cost 1+2 cycle, low >=3
;  JP_ISR_OC1A_CHK_TC0_OCR0A_0:
;        LDS   RhTMP,OCR0A         ;2 ;
;        CLZ                       ;1 ;
;        TST   RhTMP               ;1 ;
;        BREQ  JP_ISR_OC1A_FOC1A   ;12; if high=0, force out and exit
;  JP_ISR_OC1A_CHK_COM1A0_SET:      
;        IN    RhTMP,TCCR1         ;1 ; 
;        SBRC  RhTMP,COM1A0        ;12; if COM1A0 clr, skip exit and go set
;        RJMP  JP_ISR_OC1A_EXIT    ;2 ; COM1A set, skip set and exit
;  JP_ISR_OC1A_RELOAD_OCR1A_FF:
;        RAMWRAIR TCNT1,19,RhTMP   ;3 ; fix1 cycle cost,20-1
;        RAMWRAIR OCR1A,$FF,RhTMP  ;3 ; if TCNT1 reach low, ISR reset to $FF
;        RAMWRAIR TCCR1,(1<<COM1A0)|(1<<CS10),RhTMP ;3 ; on COM1A0, start TC1
;        RJMP JP_ISR_OC0A_EXIT
;  JP_ISR_OC1A_FOC1A:
;        RAMWRAIR  GTCCR,(1<<FOC1A),RhTMP  ;3 ;
;  JP_ISR_OC1A_RELOAD_TCNT1_RESTART_TC1:
;        RAMWRAIR  TCNT1,18,RhTMP          ;3 ; fix2 cycle cost
;        RAMWRAIR  TCCR1,(1<<CS10),RhTMP   ;3 ; restart TC1, cost 19c-1=18
;  JP_ISR_OC1A_EXIT:
;        RETI                    ;4 ;
;-------------------------------------------------------------------------------
;ISR_OC0A:
;  JP_ISR_OCR0A_STOP_TC1:
;        LDI   RhTMP,(1<<CS13)|(1<<CS12)|(1<<CS11)|(1<<CS10) ;1 ;
;        OUT   TCCR1,RhTMP         ;1 ; stop TC1, cost 1+2 cycle, low >=3
;        RAMWRAIR  OCR1A,LOW(OCR_16BIT),RhTMP  ;3 ;
;        RAMWRAIR  TCNT1,8,RhTMP               ;3 ; fix3 cycle cost
;        RAMWRAIR  TCCR1,(1<<CS10),RhTMP       ;3 ;
;  JP_ISR_OC0A_EXIT:
;        RETI
;-------------------------------------------------------------------------------
;ISR_OC0B:
;SR_CHK_TCNT:
;        .SET TCNT=255
;        TST R18
;        BRBS SREG_Z, JP_RELOAD_TCNT
;        DEC R18
;        RJMP JP_ISR_OC0B_EXIT
;  JP_RELOAD_TCNT:
;        LDI R18,TCNT
;  JP_LEDINVERT:
;        RAMBNOTABRH PORTB,LEDPIN,RhTMP
;  JP_ISR_OC0B_EXIT:
;        RETI
;-------------------------------------------------------------------------------
ISR_OC0A:
        RETI
ISR_OC1A:
#define TMSCNT(x) (x*625/10) ;(x*1000/0.0625*256)
#define T1SCNT    62500    ;(x*1000000/0.0625*256) ; max 1.04856s
        CPI16_01RHL2KI3RhT XH,XL,0,R16
        BRNE JP_ISR_OC1A_EXIT
  JP_RELOAD_CNT0:
        .SET CNT0=TMSCNT(10)
        LDI XH,HIGH(CNT0)
        LDI XL,LOW(CNT0)
  JP_OUTPUT:
        RAMBNOTABRH PORTB,LEDPIN1,R16
  JP_ISR_OC1A_EXIT:
        SBIW XH:XL,1
        RETI
;-------------------------------------------------------------------------------
