; PROGRAM 2.2 A Program to Illustrate PORT Output and Counting
; P56/PP87
; Counter - A simple AVR program to illustrate output to a port
; Designed to be executed in a simulator under debug control
; This program counts from 0 to 255 (and repeats)
; The current counter value is output to PORTB of an ATMega16A.
; Programmer: TM
; Date: 5/2010
; Platform: STK-500
; Device: ATMega16A

.cseg   ;select current segment as code
.org  0  ;begin assembling at address 0

;Define symbolic names for resources used
.def  count  = r16   ; Reg 16 will hold counter value
.def  temp   = r17   ; Reg 17 is used as a temporary register
.equ  PORTB  = 0x18  ; Port B's output register
.equ  DDRB   = 0x17  ; Port B's Data Direction Register

      ldi   temp,0xFF      ;configure PORTB as output
      out   DDRB,temp
      ldi   count,0x00      ;Initialize count at 0
lp:
      out PORTB,count       ;Put counter value on PORT B
      inc count             ;increment counter
      rjmp lp               ;repeat (forever)