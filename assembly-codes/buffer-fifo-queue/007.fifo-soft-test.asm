.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "007.fifo-soft.inc"
; .equ	URXCaddr	= 0x0024	; USART Rx Complete
; .equ	UDREaddr	= 0x0026	; USART, Data Register Empty
; .equ	UTXCaddr	= 0x0028	; USART Tx Complete
.CSEG
;.ORG $0
;    RJMP INIT
;.ORG URXCaddr
;    RJMP ISR_URXC
;.ORG UDREaddr
;    RJMP ISR_UDRE
.ORG $100


INIT:
INIT_SP
;INIT_UART
INIT_FIFO 256, SRAM_START, R16, R17
;MAIN:

LOOP:
        LDI R21, 255
        DEC R21
        TST R21
        BRBS SREG_Z, DONE
        MOV R0, R21
        RCALL SR_FIFO_WRIN
        RJMP LOOP

DONE: RJMP DONE
;     LDI R16, 'Y'
;     RCALL SR_SEND_CHAR
;     LDI R16, 'E'
;     RCALL SR_SEND_CHAR
;     LDI R16, 'S'
;     RCALL SR_SEND_CHAR
; ;    LDI R16, $0D
;     ;RCALL SR_SEND_CHAR
; ;    LDI R16, $0A
;     ;RCALL SR_SEND_CHAR
;     RJMP MAIN
; 
; SR_SEND_CHAR:
;     STS UDR0, R16
;     RET
; SR_ECHO:
;     ;LDI R17, '>'
;     ;STS UDR0, R17
;     STS UDR0, R16
;     LDI R16, $0D
;     STS UDR0, R16
;     LDI R16, $0A
;     STS UDR0, R16
;     RET
;ISR_URXC:
    ;LDS R16, UDR0
    ;RCALL SR_ECHO
;    RETI

;ISR_UDRE:
;    LDI R26, '>'
;    STS UDR0, R16
;    RETI
