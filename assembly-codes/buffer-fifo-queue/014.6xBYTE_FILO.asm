.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
;.INCLUDE "AVR_MACRO_IOREG.INC"
;
.CSEG
.ORG $0
    RJMP INIT
;*******************************************************************************
; USART ISR jmp address
;.ORG URXCaddr
;    RJMP ISR_URXC
.ORG UDREaddr
    RJMP ISR_UDRE
;.ORG UTXCaddr
;    RJMP ISR_UTX
;*******************************************************************************

INIT:
        ;***********************************************************************
        INIT_SP
        ;***********************************************************************
        ;********************************************************************** 
        ; UART
        INIT_UART
        ;********************************************************************** 
RJMP MAIN
;*******************************************************************************
; UART_TX_BUF
.DEF  UART_TX_BUF0_R      = R3   ; input buf
.DEF  UART_TX_BUF1_R      = R4
.DEF  UART_TX_BUF2_R      = R5
.DEF  UART_TX_BUF3_R      = R6
.DEF  UART_TX_BUF4_R      = R7
.DEF  UART_TX_BUF5_R      = R8
.DEF  UART_TX_BUF6_R      = R9
.DEF  UART_TX_BUF_LEN_RH  = R20
;-------------------------------------------------------------------------------
SR_UART_TX_WR_BUF: 
; 1stly check UART_TX_BUF_LEN_RH ?==6,then put data into UART_TX_BUF0_R and exec
        CPI   UART_TX_BUF_LEN_RH,5
        BREQ  _JP_WR_BUF6_INC_LEN
        CPI   UART_TX_BUF_LEN_RH,4
        BREQ  _JP_WR_BUF5_INC_LEN
        CPI   UART_TX_BUF_LEN_RH,3
        BREQ  _JP_WR_BUF4_INC_LEN
        CPI   UART_TX_BUF_LEN_RH,2
        BREQ  _JP_WR_BUF3_INC_LEN
        CPI   UART_TX_BUF_LEN_RH,1
        BREQ  _JP_WR_BUF2_INC_LEN
        CPI   UART_TX_BUF_LEN_RH,0
        BREQ  _JP_WR_BUF1_INC_LEN
    _JP_WR_BUF6_INC_LEN:
        MOV   UART_TX_BUF6_R,UART_TX_BUF0_R
        INC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_WR_BUF_EXIT
    _JP_WR_BUF5_INC_LEN:
        MOV   UART_TX_BUF5_R,UART_TX_BUF0_R
        INC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_WR_BUF_EXIT
    _JP_WR_BUF4_INC_LEN:
        MOV   UART_TX_BUF4_R,UART_TX_BUF0_R
        INC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_WR_BUF_EXIT
    _JP_WR_BUF3_INC_LEN:
        MOV   UART_TX_BUF3_R,UART_TX_BUF0_R
        INC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_WR_BUF_EXIT
    _JP_WR_BUF2_INC_LEN:
        MOV   UART_TX_BUF2_R,UART_TX_BUF0_R
        INC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_WR_BUF_EXIT
    _JP_WR_BUF1_INC_LEN:
        MOV   UART_TX_BUF1_R,UART_TX_BUF0_R
        INC   UART_TX_BUF_LEN_RH
    _JP_SR_UART_TX_WR_BUF_EXIT:
        RET
;-------------------------------------------------------------------------------
; PASSS test for call SR_UART_TX_WR_BUF
.MACRO TM_SR_UART_TX_WR_BUF_0IN ; @0=BUF content
        ;LDI   R31, $FF ; fill 4 x buff with $FF 
        LDI   R31, @0 ; 'A'=D41
        LDI   R30, 4
    JP_TM_SR_UART_TX_WR_BUF_0IN_LOOP:
    JP_TM_SR_UART_TX_WR_BUF_0IN_CHK_FULL:
        CPI   UART_TX_BUF_LEN_RH,6
        BREQ  JP_TM_SR_UART_TX_WR_BUF_0IN_FULL_EXIT
        MOV   UART_TX_BUF0_R,R31
        RCALL SR_UART_TX_WR_BUF
        ; loop
        INC   R31  ; fill ABCD
        DEC   R30
        BRNE  JP_TM_SR_UART_TX_WR_BUF_0IN_LOOP
    JP_TM_SR_UART_TX_WR_BUF_0IN_ADD_CRLF:
        LDI   R30, $0d ;\r CR
        MOV   UART_TX_BUF0_R,R31
        RCALL SR_UART_TX_WR_BUF
        LDI   R30, $0a ;\n LF
        MOV   UART_TX_BUF0_R,R31
        RCALL SR_UART_TX_WR_BUF
    JP_TM_SR_UART_TX_WR_BUF_0IN_FULL_EXIT:
.ENDMACRO
;-------------------------------------------------------------------------------
SR_UART_TX_RD_BUF: ; read first buf into UART_TX_BUF0_R
; 1stly check UART_TX_BUF_LEN_RH ?==0, exec SR, get data into UART_TX_BUF0_R
        CPI   UART_TX_BUF_LEN_RH,6
        BREQ  _JP_RD_BUF6_DEC_LEN
        CPI   UART_TX_BUF_LEN_RH,5
        BREQ  _JP_RD_BUF5_DEC_LEN
        CPI   UART_TX_BUF_LEN_RH,4
        BREQ  _JP_RD_BUF4_DEC_LEN
        CPI   UART_TX_BUF_LEN_RH,3
        BREQ  _JP_RD_BUF3_DEC_LEN
        CPI   UART_TX_BUF_LEN_RH,2
        BREQ  _JP_RD_BUF2_DEC_LEN
        CPI   UART_TX_BUF_LEN_RH,1
        BREQ  _JP_RD_BUF1_DEC_LEN
    _JP_RD_BUF6_DEC_LEN:
        MOV   UART_TX_BUF0_R,UART_TX_BUF6_R
        DEC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_RD_BUF_EXIT
    _JP_RD_BUF5_DEC_LEN:
        MOV   UART_TX_BUF0_R,UART_TX_BUF5_R
        DEC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_RD_BUF_EXIT
    _JP_RD_BUF4_DEC_LEN:
        MOV   UART_TX_BUF0_R,UART_TX_BUF4_R
        DEC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_RD_BUF_EXIT
    _JP_RD_BUF3_DEC_LEN:
        MOV   UART_TX_BUF0_R,UART_TX_BUF3_R
        DEC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_RD_BUF_EXIT
    _JP_RD_BUF2_DEC_LEN:
        MOV   UART_TX_BUF0_R,UART_TX_BUF2_R
        DEC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_RD_BUF_EXIT
    _JP_RD_BUF1_DEC_LEN:
        MOV   UART_TX_BUF0_R,UART_TX_BUF1_R
        DEC   UART_TX_BUF_LEN_RH
        RJMP  _JP_SR_UART_TX_RD_BUF_EXIT
    _JP_SR_UART_TX_RD_BUF_EXIT:
        RET
;-------------------------------------------------------------------------------
; PASS test for call SR_UART_TX_RD_BUF
.MACRO TM_SR_UART_TX_RD_BUF
    JP_TM_SR_UART_TX_RD_BUF:
        ;LDI   R31,  $FF
        LDI   R30,  5
    JP_TM_SR_UART_TX_RD_BUF_CHK_NULL:
        CPI   UART_TX_BUF_LEN_RH,0
        BREQ  JP_TM_SR_UART_TX_RD_BUF_EXIT
        RCALL SR_UART_TX_RD_BUF
        MOV   R31,UART_TX_BUF0_R
        ; loop
        DEC   R30
        BRNE  JP_TM_SR_UART_TX_RD_BUF_CHK_NULL
    JP_TM_SR_UART_TX_RD_BUF_EXIT:
.ENDMACRO
;-------------------------------------------------------------------------------
; SR send buff
SR_UART_TX_BUF_1BYTE_POLL:
    JP_UART_TX_BUF_1BYTE_POLL_CHK_NULL:
        CPI   UART_TX_BUF_LEN_RH,0
        BREQ  JP_UART_TX_BUF_1BYTE_POLL_EXIT
        RCALL SR_UART_TX_RD_BUF
        STS   UDR0,UART_TX_BUF0_R
    JP_UART_TX_BUF_1BYTE_POLL_EXIT:
        RET
SR_UART_TX_BUF_NBYTE_POLL:
    JP_UART_TX_BUF_NBYTE_POLL_CHK_NULL:
        CPI   UART_TX_BUF_LEN_RH,0
        BREQ  JP_UART_TX_BUF_NBYTE_POLL_EXIT
    JP_UART_TX_BUF_NBYTE_POLL_LOOP:
        RCALL SR_UART_TX_RD_BUF
        STS   UDR0,UART_TX_BUF0_R
        TST   UART_TX_BUF_LEN_RH
        BRNE  JP_UART_TX_BUF_NBYTE_POLL_LOOP
    JP_UART_TX_BUF_NBYTE_POLL_EXIT:
        RET
;-------------------------------------------------------------------------------
; test SR_UART_TX_BUF_1BYTE_POLL
; test pass
.MACRO TM_SR_UART_TX_BUF_1BYTE_POLL
        TM_SR_UART_TX_WR_BUF_0IN 41 ; write 4 x buff with $FF
        RCALL SR_UART_TX_BUF_1BYTE_POLL
.ENDMACRO
.MACRO TM_SR_UART_TX_BUF_NBYTE_POLL
;        TM_SR_UART_TX_WR_BUF_0IN ; write 4 x buff with $FF
        RCALL SR_UART_TX_BUF_NBYTE_POLL
.ENDMACRO
;-------------------------------------------------------------------------------
ISR_UDRE:
ISR_UART_TX_BUF:
    RCALL SR_UART_TX_BUF_1BYTE_POLL
    RETI

.MACRO _DEBUG ; @0='symbol', @1=TR
    LDI @1, @0
    STS UDR0, @1
.ENDMACRO
MAIN:
    RCALL SR_DELAY_BLOCK
    ;CLI
    ; test polling
    _DEBUG '0',R16
    TM_SR_UART_TX_WR_BUF_0IN $41   ; ABCD
    _DEBUG '1',R16
    RCALL SR_UART_TX_BUF_1BYTE_POLL
    _DEBUG '2',R16
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL ; extra one 
    ; NBYTE
    ;RCALL SR_UART_TX_BUF_NBYTE_POLL
    ; test ISR
    ;TM_SR_UART_TX_WR_BUF_0IN $43
    ;RCALL SR_UART_TX_BUF_1BYTE_POLL
    ;NOP
    ;RCALL SR_UART_TX_BUF_NBYTE_POLL
    ;SEI
    ; add delay for transfer with ISR
    RJMP MAIN

_END:
    RJMP _END

SR_DELAY_BLOCK:
L1:    
    LDI R18, 255
L2:    
    LDI R19, 255

    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP
    NOP

    DEC R11
    BRNE L2   
    DEC R10
    BRNE L1
    RET