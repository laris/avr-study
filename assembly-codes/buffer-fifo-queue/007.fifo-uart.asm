.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
; .equ	URXCaddr	= 0x0024	; USART Rx Complete
; .equ	UDREaddr	= 0x0026	; USART, Data Register Empty
; .equ	UTXCaddr	= 0x0028	; USART Tx Complete
.CSEG
.ORG $0
    RJMP INIT
.ORG URXCaddr
    RJMP ISR_URXC
.ORG UDREaddr
    RJMP ISR_UDRE
.ORG $100

.MACRO INIT_FIFO
.EQU  FIFO_RX_CAPACITY  = 256
.EQU  FIFO_RX_ADR_START     = SRAM_START            ; SRAM_START = 0x0100
.EQU  FIFO_RX_ADR           = FIFO_RX_ADR_START
.EQU  FIFO_RX_ADR_END       = SRAM_START + FIFO_RX_CAPACITY - 1
.EQU  FIFO_TX_CAPACITY  = 256
.EQU  FIFO_TX_ADR_START     = FIFO_RX_ADR_END + 1
.EQU  FIFO_TX_ADR           = FIFO_TX_ADR_START
.EQU  FIFO_TX_ADR_END       = FIFO_TX_ADR_START + FIFO_TX_CAPACITY - 1
.EQU  FIFO_REG_CAPACITY = 7
.EQU  FIFO_REG_ADR_START    = FIFO_TX_ADR_END + 1
.EQU  FIFO_REG_ADR          = FIFO_REG_ADR_START
.EQU  FIFO_REG_ADR_END      = FIFO_REG_ADR_START+FIFO_REG_CAPACITY-1
.EQU  FIFO_RX_WRIN_PTR  = 0                    ; head pointer
.EQU  FIFO_RX_RDOUT_PTR = 1                    ; tail pointer
.EQU  FIFO_RX_LEN       = 2                    ; length = head/in - tail/out
.EQU  FIFO_TX_WRIN_PTR  = 3                    ; head pointer
.EQU  FIFO_TX_RDOUT_PTR = 4                    ; tail pointer
.EQU  FIFO_TX_LEN       = 5                    ; length = head/in - tail/out
.EQU  FIFO_SREG         = 6                    ; FIFO Status Register
.EQU  FIFO_RX_FLG_EMPTY = 0
.EQU  FIFO_RX_FLG_FULL  = 1
.EQU  FIFO_TX_FLG_EMPTY = 3
.EQU  FIFO_TX_FLG_FULL  = 4
.DEF  FIFO_REG_TMP1     = R16
.DEF  FIFO_REG_TMP2     = R17
.ENDM
.MACRO LDZPTR ; @0 = MEM ADR
        LDI   ZL, LOW(@0)
        LDI   ZH, HIGH(@0)
.ENDM
.MACRO RDMEMPD   ; @0=REG @1=MEM ADR, @2=K6D[0,63],
        LDZPTR @1
        LDD   @0, Z+@2
.ENDM
.MACRO WRMEMPD   ; @0=MEM ADR, @1=K6D[0,63], @2=REG
        LDZPTR @0
        STD  Z+@1, @2
.ENDM
.MACRO RAMBSETPZAD ; @0=ADR, @1=DISP @2=BIT index, @3=REGTMP
    LDZPTR @0
    LDD  @3, Z+@1
    SBR  @3, (1<<@2)
    STD  Z+@1, @3
.ENDM
.MACRO RAMBCLRPZAD ; @0=ADR, @1=DISP @2=BIT index, @3=REGTMP
    LDZPTR @0
    LDD  @3, Z+@1
    CBR  @3, (1<<@2)
    STD  Z+@1, @3
.ENDM
SR_FIFO_RX_GET_LEN:
        ; hardcode way
        ;MOV   FIFO_RX_LEN, FIFO_RX_WRIN_PTR
        ;SUB   FIFO_RX_LEN, FIFO_RX_RDOUT_PTR        ; LEN = head/in - tail/out
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_RX_WRIN_PTR  ; RT1=RX IN PTR
        RDMEMPD FIFO_REG_TMP2,FIFO_REG_ADR_START,FIFO_RX_RDOUT_PTR ; RT2=RX OUT PTR
        ; call way, cost more cycle, drop
        ;CALL SR_FIFO_CAL_LEN
        SUB   FIFO_REG_TMP1, FIFO_REG_TMP2     ; LEN = (head/in) =- tail/out
        WRMEMPD FIFO_REG_ADR_START,FIFO_RX_LEN,FIFO_REG_TMP1       ; save LEN
        RET
SR_FIFO_RX_CHK_FULL:
        RCALL SR_FIFO_RX_GET_LEN
        ; hardcode way
        ;CPI   FIFO_RX_LEN, FIFO_RX_CAPACITY
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_RX_LEN       ; RT1=LEN
        CPI   FIFO_REG_TMP1,FIFO_RX_CAPACITY-1
        BRGE JP_FIFO_RX_FULL_SET
JP_FIFO_RX_FULL_CLR:
        ; GPR as SREG way
        ;CBR   FIFO_SREG, FIFO_RX_FLG_FULL
        ; MEM as SREG way
        RAMBSETPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_RX_FLG_FULL,FIFO_REG_TMP1
        RET
JP_FIFO_RX_FULL_SET:
        ;SBR   FIFO_SREG, FIFO_RX_FLG_FULL
        RAMBCLRPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_RX_FLG_FULL,FIFO_REG_TMP1
        RET
SR_FIFO_RX_CHK_EMPTY:
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_RX_RDOUT_PTR ; RT1=RX OUT PTR
        RDMEMPD FIFO_REG_TMP2,FIFO_REG_ADR_START,FIFO_RX_WRIN_PTR  ; RT1=RX IN PTR
        ;CP   FIFO_RX_RDOUT_PTR, FIFO_RX_WRIN_PTR ; cp tail/out ?>= head/in
        CP    FIFO_REG_TMP1,FIFO_REG_TMP2         ; cp tail/out ?>= head/in
        BRLT JP_FIFO_RX_EMPTY_CLR            ; if tail<head, no empty
        BREQ JP_FIFO_RX_EMPTY_SET            ; if tail==head, empty
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_RX_LEN
        CPI   FIFO_REG_TMP1,FIFO_RX_CAPACITY-1; if tail>head, cp LEN?>=FIFO size
        BRGE JP_FIFO_RX_EMPTY_SET            ; if tail>head && LEN>=size, empty
JP_FIFO_RX_EMPTY_CLR:
        ;CBR   FIFO_SREG, FIFO_RX_FLG_EMPTY
        RAMBCLRPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_RX_FLG_FULL,FIFO_REG_TMP1
        RET
JP_FIFO_RX_EMPTY_SET:
        ;SBR   FIFO_SREG, FIFO_RX_FLG_EMPTY
        RAMBSETPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_RX_FLG_FULL,FIFO_REG_TMP1
        RET

SR_FIFO_TX_GET_LEN:
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_TX_WRIN_PTR
        RDMEMPD FIFO_REG_TMP2,FIFO_REG_ADR_START,FIFO_TX_RDOUT_PTR
        SUB   FIFO_REG_TMP1, FIFO_REG_TMP2
        WRMEMPD FIFO_REG_ADR_START,FIFO_TX_LEN,FIFO_REG_TMP1
        RET
SR_FIFO_TX_CHK_FULL:
        RCALL SR_FIFO_TX_GET_LEN
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_TX_LEN
        CPI   FIFO_REG_TMP1,FIFO_TX_CAPACITY-1
        BRGE JP_FIFO_TX_FULL_SET
JP_FIFO_TX_FULL_CLR:
        RAMBSETPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_TX_FLG_FULL,FIFO_REG_TMP1
        RET
JP_FIFO_TX_FULL_SET:
        RAMBCLRPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_TX_FLG_FULL,FIFO_REG_TMP1
        RET
SR_FIFO_TX_CHK_EMPTY:
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_TX_RDOUT_PTR
        RDMEMPD FIFO_REG_TMP2,FIFO_REG_ADR_START,FIFO_TX_WRIN_PTR
        CP    FIFO_REG_TMP1,FIFO_REG_TMP2
        BRLT JP_FIFO_TX_EMPTY_CLR
        BREQ JP_FIFO_TX_EMPTY_SET
        RDMEMPD FIFO_REG_TMP1,FIFO_REG_ADR_START,FIFO_RX_LEN
        CPI   FIFO_REG_TMP1,FIFO_RX_CAPACITY-1
        BRGE JP_FIFO_TX_EMPTY_SET
JP_FIFO_TX_EMPTY_CLR:
        RAMBCLRPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_TX_FLG_FULL,FIFO_REG_TMP1
        RET
JP_FIFO_TX_EMPTY_SET:
        RAMBSETPZAD FIFO_REG_ADR,FIFO_SREG,FIFO_TX_FLG_FULL,FIFO_REG_TMP1
        RET

FIFO_RX_WRIN:
        NOP
        ; cal full
        ; if full, error, wait
        ; not full, write data into FIFO
        RET
FIFO_RX_RDOUT:
        NOP
        ; cal empty
        ; if empty, error
        ; not empty, read out
        RET

INIT:
INIT_SP
INIT_UART
INIT_FIFO
MAIN:
    LDI R16, 'Y'
    RCALL SR_SEND_CHAR
    LDI R16, 'E'
    RCALL SR_SEND_CHAR
    LDI R16, 'S'
    RCALL SR_SEND_CHAR
;    LDI R16, $0D
    ;RCALL SR_SEND_CHAR
;    LDI R16, $0A
    ;RCALL SR_SEND_CHAR
    RJMP MAIN

SR_SEND_CHAR:
    STS UDR0, R16
    RET
SR_ECHO:
    ;LDI R17, '>'
    ;STS UDR0, R17
    STS UDR0, R16
    LDI R16, $0D
    STS UDR0, R16
    LDI R16, $0A
    STS UDR0, R16
    RET
ISR_URXC:
    ;LDS R16, UDR0
    ;RCALL SR_ECHO
    RETI

ISR_UDRE:
;    LDI R26, '>'
;    STS UDR0, R16
    RETI