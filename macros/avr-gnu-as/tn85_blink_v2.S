// ref 023.TN85-16b-TC-software.asm
// gas version
// not work now
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#include <avr/io.h>
##include <avr/interrupt.h>
#include "avr_macro.S"

.section .text
.org  0
      RJMP _initSP
.org  TIM0_COMPA_vect_num*2
      RJMP _isr_OC0A_16bTC
.org  _VECTORS_SIZE
#include "avr_lib_tnx5_tc16.S"
#include "avr_lib_blink_led.S"

.global _initSP
_initSP:
        _init_sp

#define rhtmp        R16

/*
#define pinLED       PB1
#define iLEDInit     0

#define rCntOnOff    R17
#define rCntChange   r18

#define iCntOnOff    3
#define iCntPulse    1
#define iCntChange   (2*iCntPulse)-1
*/

/*
#define TC16K_OCR16  15625
#define TC16R_TCNTH  R27      //
#define TC16R_TCNTL  TCNT0
#define TC16R_OCRAH  XL
#define TC16R_OCRAL  OCR0A
#define TC16R_OCRBH  R26      //XL
#define TC16R_OCRBL  OCR0B
#define TC16R_TCCRA  TCCR0A
#define TC16B_COMA0  COM0A0
#define TC16B_COMA1  COM0A1
#define TC16B_COMB0  COM0B0
#define TC16B_COMB1  COM0B1
#define TC16B_WGM0   WGM00
#define TC16B_WGM1   WGM01
#define TC16R_TCCRB  TCCR0B
#define TC16B_WGM2   WGM02
#define TC16B_CS2    CS02
#define TC16B_CS1    CS01
#define TC16B_CS0    CS00
#define TC16B_FOCA   FOC0A
#define TC16B_FOCB   FOC0B
#define TC16R_TIMSK  TIMSK
#define TC16B_OCIEA  OCIE0A
#define TC16B_OCIEB  OCIE0B
#define TC16B_TOIE   TOIE0
#define TC16R_TIFR   TIFR
#define TC16B_OCFA   OCF0A
#define TC16B_OCFB   OCF0B
#define TC16B_TOV    TOV0
#define TC16R_GTCCR  GTCCR
*/
/*
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
;.EQU  OCR_16BIT = $FFFF/4 ; test for 8-bit mode
;        ;RAMWRAIR  TIMSK,  (1<<OCIE0A),R16        ; skip, enable OCIE0A in TC1
;        RAMWRAIR  OCR0A,HIGH(OCR_16BIT),R16
;        RAMWRAIR  TCCR0A,(1<<COM0A0)|(1<<WGM01),R16           ; COM0A0, M2-CTC
;        RAMWRAIR  TCCR0B,(1<<CS02)|(1<<CS01)|(1<<CS00),R16 ; T0 rising triger
; test for 16b-TC and diff man encoding
        ;RAMWRAIR  TIMSK,  (1<<OCIE0A),R16        ; skip, enable OCIE0A in TC1
        ;RAMWRAIR  OCR0A,HIGH(OCR_16BIT),R16
        ;RAMWRAIR  OCR0B,HIGH(OCR_16BIT/2),R16     ; set half FOC pulse
        ;------------------------------
        ; 4.19424 65535
        ; 4s 62500
        ; 3s 46875
        ; 2s 31250
        ; 0.5Hz   15625*1024*62.5ns = 1s 0.5Hz
        ; SS  0.5Hz  1000ms = 15625     good for count 1s
        ; SLOW  1Hz   500ms =  7812     good for count 1s
        ; NORM  2Hz   250ms =  3906     no
        ; FAST  4Hz   125ms =  1953     Good for FAST blink
        ; FAST  5Hz   100ms =  1563     no
        ; FF   10Hz    50ms =   781     Good for FF
        ; FF   15Hz    33ms =   521     no
        ; FF   20Hz    25ms =   391     no
        ; SSB1 
*/
/*
_initLED:
        _bit_wr   DDRB, pinLED,1,rhtmp
        _bit_wr   PORTB,pinLED,1,rhtmp
_initConfig:
        _mwrki    iCntOnOff, rCntOnOff
        _mwrki    iCntChange,rCntChange
_initTC:
        _mwrki 0, TC16R_TCNTH, rhtmp
        _mwrki 0, TC16R_TCNTL, rhtmp
        _mwrk lo8(TC16K_OCR16),TC16R_OCRBL, rhtmp
        _mwrk hi8(TC16K_OCR16),TC16R_OCRBH, rhtmp
        _mwrki 0, TC16R_TCCRA, rhtmp
        _mwrk <1!<!<TC16B_OCIEB>,TC16R_TIMSK, rhtmp
        SEI
        _mwrk <1!<!<TC16B_CS2 | 1!<!<TC16B_CS0>,TC16R_TCCRB, rhtmp
        _bit_not PORTB, pinLED
*/
        RCALL funcBlinkLEDinit
        RCALL funcBlinkLEDinitConfig

        LDI   REG_TCNT0, 0
        LDI   TCNT0H,0
#define TOP16 15625
        LDI   REG_OCR0A,lo8(TOP16)
        LDI   OCR0AH,hi8(TOP16)
        _mwrk <1!<!<OCIE0A>,REG_TIMSK, rhtmp
        _mwrk <1!<!<CS00>,REG_TCCR0B, rhtmp
        RCALL  func_tnx5_tc0_init_reg
        SEI
        RCALL  func_tnx5_tc0_start_reg

.global _main
_main:
        RJMP _main

/*
.global _isrOC0B
_isrOC0B:
    _jpStopTC:
        _mcp  TC16R_TCCRA,rhtmp ; rd TCCRA to clr WGM, save other
        CBR   rhtmp,(1<<TC16B_WGM1)|(1<<TC16B_WGM0)   ; clr WGM 1/0
        _mcp  rhtmp,TC16R_TCCRA ; wr back, stop TC
    _jpOCIE:
        _if_bit TC16R_TIFR,TC16B_TOV,set,_jpNoOV
        INC     TC16R_TCNTH
        _bit_wr TC16R_TIFR,TC16B_TOV,1
    _jpNoOV:
        _mcp  TC16R_OCRBH,rhtmp
        dec   rhtmp
        _if   <CP TC16R_TCNTH,rhtmp>, eq, elif
    _jpStartTC:
              _mcp  TC16R_TCCRA,rhtmp ; rd TCCRA to clr WGM, save other
              SBR   rhtmp,(1<<TC16B_WGM1)|(1<<TC16B_WGM0)   ; clr WGM 1/0
              _mcp  rhtmp,TC16R_TCCRA ; wr back, stop TC
        _elif <CP TC16R_TCNTH,TC16R_OCRBH>, <sh>, Eif

    _jp_chk_1st_change:
              DEC rCntOnOff
              _if <TST rCntOnOff>, eq, elif
                  _bit_not  PORTB,pinLED,rhtmp
                  _mwrk     iCntOnOff,rCntOnOff
                  _mwrk     iCntChange,rCntChange
              _elif <TST rCntChange>, eq, else
                  CLR       TC16R_TCNTH
              _Eelif
              _else
                  DEC     rCntChange
                  _bit_not  PORTB,pinLED,rhtmp
                  CLR       TC16R_TCNTH
              _Eif

        _Eelif
      _Eif
RETI
*/