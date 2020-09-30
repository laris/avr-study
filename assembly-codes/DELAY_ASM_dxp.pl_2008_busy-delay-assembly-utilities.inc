;.INCLUDE "M328PDEF.INC"
;.INCLUDE "AVR_MACRO.INC"
;*************************************************************************
; README
; http://en.radzio.dxp.pl/avr-mcu/delay-assembly-utilities.html
; Simple delay utilities. Simple busy-wait loops assembly library.
; Should be used with AVR Assembler v.2
; This library can be used with any AVR device,
; because not use any specific peripheral.
;*************************************************************************
; # How use this file?
; In file with entry code place include directive :
;   #include "wait.asm" ; AVR Assembler v.2
;   // or
;   .include "wait.asm" ; AVR Assembler v. 1
; # DVUS(x) macro
; // Define ?V? U(micro)seconds macro, MACRO (us) = DVUS(x us/number of cycles)
; This macro is used to compute number of cycles
; for specified time in microseconds (us).
; # Wait4xCycles
; This routine generate delay in CPU cycles multiply by 4.
; For delay equal 10000 CPU cycles load to XH:XL 2500.
; Example of use :
;   ldi	   XH, HIGH(DVUS(500))
;   ldi	   XL, LOW(DVUS(500))
;   rcall	 Wait4xCycles ; wait 500 microseconds
; # WaitMiliseconds
; This routine generate delay in miliseconds (ms).
; Number of miliseconds must be loaded into r16 register before call this routine.
; Example of use :
;   ldi	   r16, 50
;   rcall	 WaitMiliseconds ; wait 50 miliseconds
;*************************************************************************
; Source code of wait.asm file
;*************************************************************************
; Busy-wait loops utilities module
; For F_CPU >= 4MHz
; http://avr-mcu.dxp.pl
; (c) Radoslaw Kwiecien, 2008
;*************************************************************************
#ifndef F_CPU
#define F_CPU 16000000
;  #error "F_CPU must be defined!"
#endif

#if F_CPU < 4000000
  #warning "F_CPU too low than 4MHz, possible wrong delay"
#endif

#define CYCLES_PER_US (F_CPU/1000000)
;#define C4PUS (CYCLES_PER_US/4)
#define C4PUS (CYCLES_PER_US/1)
#define DVUS(x) (C4PUS*x)

;------------------------------------------------------------------------------
; Input : XH:XL - number of CPU cycles to wait (divided by four)
;------------------------------------------------------------------------------
Wait4xCycles:
  SBIW XH:XL, 1     ; XH:XL = 1 x word, range D[0,65535]=H[0,0xFFFF]
  BRNE  Wait4xCycles ; if ZeroFlag != 0, return
  RET
;------------------------------------------------------------------------------
; Input : r16 - number of miliseconds to wait
;------------------------------------------------------------------------------
WaitMiliseconds:
;  PUSH  R16
WaitMsLoop:
  LDI   XH,HIGH(DVUS(500))  ; Define 500 us high byte, 0x01
  LDI   XL,LOW(DVUS(500))   ; Define 500 us low byte, 0xF4
  RCALL Wait4xCycles        ; cost 500us
  LDI   XH,HIGH(DVUS(500))
  LDI   XL,LOW(DVUS(500))
  RCALL Wait4xCycles        ; cost 500us
  DEC   R16
  BRNE  WaitMsLoop
;  POP   R16
  RET
;------------------------------------------------------------------------------
WaitSecLoop:
  PUSH  R17
Wait1SLoop:
  LDI   R17, 255
Wait3Ms9Loop:
  LDI   XH, HIGH(DVUS(62500))
  LDI   XL,  LOW(DVUS(62500))
  RCALL Wait4xCycles          ; cost 3.9ms 
  DEC   R17
  BRNE  Wait3Ms9Loop         ; 255->0, 256*3.9xxxxx ms=1s 
  LDI   XH, HIGH(DVUS(62500)) ; cost last 3.9ms
  LDI   XL,  LOW(DVUS(62500))
  RCALL Wait4xCycles          ; cost 3.9ms
  CLZ 
  DEC   R16
  BRNE  Wait1SLoop
  POP   R17
  RET
; End of file
;------------------------------------------------------------------------------
