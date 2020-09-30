;.INCLUDE "M328PDEF.INC"
;.INCLUDE "AVR_MACRO.INC"
;.INCLUDE "AVR_MACRO_IOREG.INC"
;-------------------------------------------------------------------------------
; Functional way to call, standard library
;-------------------------------------------------------------------------------
; Timer/Counter 0/2
; 1. init TC; 2. set run flag and CS; 3. start/stop;
; push param into stack TCCRnA/TCCRnB/TCNTn/OCRnA/OCRnB/TIMSKn/TIFRn
; pop  param then write to REG
;-------------------------------------------------------------------------------
        ;INIT_SP
;-------------------------------------------------------------------------------
; INIT TC0 CFG
;        .DEF  RHT   = R16
;        .DEF  RHT2  = R17
;-------------------------------------------------------------------------------
;TIMSK
        ;LDI  RHT, 0b0000_0000        ; TIMSKn 5x- OCIEnB/A/TOIEn
        ;PUSH RHT
;OCRnB
        ;LDI  RHT, 0b0000_0000        ; OCRnB
        ;PUSH RHT
;OCRnA
        ;LDI  RHT, 0b0000_0000        ; OCRnA
        ;PUSH RHT
;TCNTn
        ;LDI  RHT, 0b0000_0000        ; TCNTn
        ;PUSH RHT
;TCCRnB
        ;LDI  RHT, 0b0000_0000        ; TCCRnB FOCnA/B - - WGMn2 [CSn2/1/0]
        ;PUSH RHT
;TCCRnA ;WGM 000Normal 001PWM255 010CTCOCRnA 011FPWM255 101PWMOCRnA 111FPWMOCRnA
        ;COMn 00NONE 01Toggle 10Clear 11Set OA=D/PD6/P12 OB=D5/P11 T0=D4/P6
        ;COMnA1 COM0 COMnB1 COM0 - - WGMn1 WGMn0        
        ;LDI  RHT, 0b0000_0000        ; TCCRnA
        ;PUSH RHT  
;CS     
        ;LDI  RHT, 0b0000_0xxx        ; CSn2/1/0
        ;STS  RHT, CFG_CS_FOC
;TIFR0  ; not need LDI  RHT, 0b0000_0000        ; TIFR0  5x- OCFnB/A/TOVn
        ;PUSH RHT
        ;RCALL CALL_TC_INIT
;-------------------------------------------------------------------------------
; Reserve SRAM for start/stop and foc
;        .DSEG
;        .ORG        SRAM_START
;        CFG_CS_FOC:  .byte 1           ;
;        REG_TMP:     .byte 1           ; temp reg to save RHT
;-------------------------------------------------------------------------------
; CALL
;        .CSEG
        ;.ORG        0
;-------------------------------------------------------------------------------
;.ORG $0  ; RESET JP
;    RJMP INIT
;-------------------------------------------------------------------------------
; ***** INTERRUPT VECTORS ************************************************
;.equ	INT0addr	= 0x0002	; External Interrupt Request 0
;.equ	INT1addr	= 0x0004	; External Interrupt Request 1
;.equ	PCI0addr	= 0x0006	; Pin Change Interrupt Request 0
;.equ	PCI1addr	= 0x0008	; Pin Change Interrupt Request 0
;.equ	PCI2addr	= 0x000a	; Pin Change Interrupt Request 1
;.equ	WDTaddr	  = 0x000c	; Watchdog Time-out Interrupt
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
;.equ	SPIaddr	  = 0x0022	; SPI Serial Transfer Complete
;.equ	URXCaddr	= 0x0024	; USART Rx Complete
;.equ	UDREaddr	= 0x0026	; USART, Data Register Empty
;.equ	UTXCaddr	= 0x0028	; USART Tx Complete
;.equ	ADCCaddr	= 0x002a	; ADC Conversion Complete
;.equ	ERDYaddr	= 0x002c	; EEPROM Ready
;.equ	ACIaddr	  = 0x002e	; Analog Comparator
;.equ	TWIaddr	  = 0x0030	; Two-wire Serial Interface
;.equ	SPMRaddr	= 0x0032	; Store Program Memory Read
;.equ	INT_VECTORS_SIZE	= 52	; size in words
; ***** INTERRUPT VECTORS ************************************************
;        .ORG INT_VECTORS_SIZE
CALL_TC_INIT:
        ;STS   RHT, REG_TMP           ; save RHT
        ; clear reg
        LDI   RHT,  (1<<OCF0B) | (1<<OCF0A) | (1<<TOV0)
        OUT   TIFR0, RHT              ; clear TIFR0
        CLR   RHT
        OUT   TCCR0A, RHT
        OUT   TCCR0B, RHT
        OUT   TCNT0,  RHT
        OUT   OCR0A,  RHT
        OUT   OCR0B,  RHT
        STS   TIMSK0, RHT             ; clear TIMSK0
        ; init
        POP   RHT
        OUT   TCCR0A,  RHT
        POP   RHT
        OUT   TCCR0B,  RHT
        POP   RHT
        OUT   TCNT0,  RHT
        POP   RHT
        OUT   OCR0A,  RHT
        POP   RHT
        OUT   OCR0B,  RHT
        POP   RHT
        STS   TIMSK0, RHT
        RET
CALL_TC_START:
        PUSH  RHT
        PUSH  RHT2
        IN    RHT,  TCCR0B
        LDS   RHT2, CFG_CS_FOC
        CBR   RHT2, 0b0000_0111
        OR    RHT,  RHT2
        OUT   TCCR0B, RHT
        POP   RHT2
        POP   RHT
        RET
CALL_TC_STOP:
        PUSH  RHT
        IN    RHT,  TCCR0B
        CBR   RHT,  0B11111000
        OUT   TCCR0B, RHT
        POP   RHT
        RET
CALL_TC_FOC:
        PUSH  RHT
        PUSH  RHT2
        IN    RHT,  TCCR0B
        LDS   RHT2, CFG_CS_FOC
        CBR   RHT2, 0b1100_0000
        OR    RHT,  RHT2
        OUT   TCCR0B, RHT
        POP   RHT2
        POP   RHT
        RET

