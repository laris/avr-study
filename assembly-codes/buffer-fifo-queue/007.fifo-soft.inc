;.INCLUDE "AVR_MACRO.INC"
; FIFO (First-In First-OUt) BUFFER software implementation
.MACRO INIT_FIFO_INDEX  ; @0=RAM_A, @1=INDEX_CAP, @2=REGTMP[GPR]
; FIFO INSTANCE INDEX
; INST_INDEX_N=[1,2,3], INST_N_A=HIGH(2N),LOW(2N-1)
.EQU  FIFO_INDEX_AS       = @0                  ; index RAM start ADR
.EQU  FIFO_INDEX_CAP      = @1                  ; max instance = 256
.EQU  FIFO_INDEX_A        = FIFO_INDEX_AS +1    ; index RAM start ADR
.EQU  FIFO_INDEX_AE       = FIFO_INDEX_A + FIFO_INDEX_CAP ; MEM MAP=REG+CAP
.EQU  FIFO_INDEX_A_INSTN  = FIFO_INDEX_AS       ; FIFO instance num [0,255]
      RAMFILLPZAAKR  FIFO_INDEX_AS,FIFO_INDEX_AE,0x0,@2
.ENDMACRO
SR_FIFO_INDEX_INST_ADD:
      
SR_FIFO_INDEX_INST_DEL:

 .MACRO INIT_FIFO ; @0=FIFO_CAP[1,256], @1=RAM_AS, @2=REGT1[16,31], @3=REGT2[16,31]
.EQU  FIFO_CAPACITY     = @0                    ; buffer max capacity=256
.EQU  FIFO_AS    = @1                    ; SRAM_START = 0x0100
.EQU  FIFO_A          = FIFO_AS
.EQU  FIFO_AE      = FIFO_AS + FIFO_CAPACITY - 1
.EQU  FIFO_REG_CAPACITY = 4
.EQU  FIFO_REG_AS= FIFO_AE + 1
.EQU  FIFO_REG_A      = FIFO_REG_AS
.EQU  FIFO_REG_AE  = FIFO_REG_AS + FIFO_REG_CAPACITY - 1
.DEF  FIFO_REG_TMP1     = @2
.DEF  FIFO_REG_TMP2     = @3
.ENDMACRO

.EQU  FIFO_SREG_A     = FIFO_REG_A          ; FIFO SREG ADR
.EQU  FIFO_SREG_EMPTY   = 0                     ; buffer empty flag index
.EQU  FIFO_SREG_FULL    = 1                     ; buffer full flag index
.EQU  FIFO_SREG         = 0                     ; FIFO Status Register
.EQU  FIFO_WRIN_PTR     = 1                     ; head/write/in pointer
.EQU  FIFO_RDOUT_PTR    = 2                     ; tail/read/out pointer
.EQU  FIFO_LEN          = 3                     ; item/queue length = head - tail

SR_FIFO_GET_LEN:
        ; hardcode way
        ;MOV   FIFO_LEN, FIFO_WRIN_PTR
        ;SUB   FIFO_LEN, FIFO_RDOUT_PTR         ; LEN = head/in - tail/out
        RAMRDPZRAD FIFO_REG_TMP1,FIFO_REG_A,FIFO_WRIN_PTR  ; RT1=RX IN PTR
        RAMRDPZRAD FIFO_REG_TMP2,FIFO_REG_A,FIFO_RDOUT_PTR ; RT2=RX OUT PTR
        ; call way, cost more cycle, drop
        ;CALL SR_FIFO_CAL_LEN
        SUB   FIFO_REG_TMP1, FIFO_REG_TMP2      ; LEN = (head/in) =- tail/out
        RAMWRPZADR FIFO_REG_A,FIFO_LEN,FIFO_REG_TMP1       ; save LEN
        RET
SR_FIFO_CHK_FULL:
        RCALL SR_FIFO_GET_LEN
        ; hardcode way
        ;CPI   FIFO_LEN, FIFO_CAPACITY
        RAMRDPZRAD FIFO_REG_TMP1,FIFO_REG_A,FIFO_LEN       ; RT1=LEN
        CPI   FIFO_REG_TMP1,FIFO_CAPACITY-1
        BRGE JP_FIFO_FULL_SET
JP_FIFO_FULL_CLR:
        ; GPR as SREG way
        ;CBR   FIFO_SREG, FIFO_SREG_FULL
        ; MEM as SREG way
        RAMBSETPZAD FIFO_REG_A,FIFO_SREG,FIFO_SREG_FULL,FIFO_REG_TMP1
        RET
JP_FIFO_FULL_SET:
        ;SBR   FIFO_SREG, FIFO_SREG_FULL
        RAMBCLRPZAD FIFO_REG_A,FIFO_SREG,FIFO_SREG_FULL,FIFO_REG_TMP1
        RET
SR_FIFO_CHK_EMPTY:
        RAMRDPZRAD FIFO_REG_TMP1,FIFO_REG_A,FIFO_RDOUT_PTR ; RT1=RX OUT PTR
        RAMRDPZRAD FIFO_REG_TMP2,FIFO_REG_A,FIFO_WRIN_PTR  ; RT1=RX IN PTR
        ;CP   FIFO_RDOUT_PTR, FIFO_WRIN_PTR ; cp tail/out ?>= head/in
        CP    FIFO_REG_TMP1,FIFO_REG_TMP2         ; cp tail/out ?>= head/in
        BRLT JP_FIFO_EMPTY_CLR            ; if tail<head, no empty
        BREQ JP_FIFO_EMPTY_SET            ; if tail==head, empty
        RAMRDPZRAD FIFO_REG_TMP1,FIFO_REG_A,FIFO_LEN
        CPI   FIFO_REG_TMP1,FIFO_CAPACITY-1; if tail>head, cp LEN?>=FIFO size
        BRGE JP_FIFO_EMPTY_SET            ; if tail>head && LEN>=size, empty
JP_FIFO_EMPTY_CLR:
        ;CBR   FIFO_SREG, FIFO_SREG_EMPTY
        RAMBCLRPZAD FIFO_REG_A,FIFO_SREG,FIFO_SREG_EMPTY,FIFO_REG_TMP1
        RET
JP_FIFO_EMPTY_SET:
        ;SBR   FIFO_SREG, FIFO_SREG_EMPTY
        RAMBSETPZAD FIFO_REG_A,FIFO_SREG,FIFO_SREG_EMPTY,FIFO_REG_TMP1
        RET
SR_UPDATE_REG:
        CALL SR_FIFO_CHK_EMPTY
        CALL SR_FIFO_CHK_FULL
        RET

SR_FIFO_WRIN:  ; write REGTMP=R0 into FIFO
        ;CALL SR_UPDATE_REG
        SKBS  FIFO_SREG_A,FIFO_SREG_FULL,FIFO_REG_TMP1 ; check ?= 1, 1=full skip next
        RJMP JP_FIFO_FULL_ERR            ; if full, error, wait
        RJMP JP_FIFO_WRIN                ; not full, write REGTMP into FIFO
JP_FIFO_WRIN:
        ;MOV   FIFO_REG_TMP1,R0
        ;RAMWRPZADR FIFO_A,FIFO_WRIN_PTR,FIFO_REG_TMP1
        RAMWRPZADR FIFO_A,FIFO_WRIN_PTR,R0
        RAMINCPZAD FIFO_REG_A,FIFO_WRIN_PTR,FIFO_REG_TMP1
        CALL SR_FIFO_CHK_EMPTY
        CALL SR_FIFO_CHK_FULL
        RET
JP_FIFO_FULL_ERR:
        NOP
        RET
SR_FIFO_RDOUT:  ; read FIFO out to REGTMP=R0
        SKBS  FIFO_SREG_A,FIFO_SREG_EMPTY,FIFO_REG_TMP1 ; check ?= 1, 1=empty skip next
        RJMP JP_FIFO_EMPTY_ERR            ; if empty, error, wait
        RJMP JP_FIFO_RDOUT                ; not empty, read FIFO out to REGTMP
JP_FIFO_RDOUT:
        RAMRDPZRAD R0,FIFO_A,FIFO_RDOUT_PTR
        RAMINCPZAD FIFO_REG_A,FIFO_RDOUT_PTR,FIFO_REG_TMP1
        CALL SR_FIFO_CHK_EMPTY
        CALL SR_FIFO_CHK_FULL
        RET
JP_FIFO_EMPTY_ERR:
        NOP
        RET
