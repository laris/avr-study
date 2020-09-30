.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
;*******************************************************************************
;INIT:
;        INIT_SP
;        INIT_UART
;.INCLUDE "007.BUF_FIFO_V5_SRAM_PROM_MAX_4xF.INC"
.INCLUDE "007.BUF_FIFO_V2.1_MAX255.INC"
;*******************************************************************************
.DEF    RHT1      = R16
.DEF    RHT2      = R17
.EQU    SREG_UCSR = UCSR0B
.EQU    SREG_UDR  = UDR0
.EQU    BIT_RXCIE = RXCIE0
.EQU    BIT_UDRIE = UDRIE0
;-------------------------------------------------------------------------------
; Config SRAM Address
.EQU  K16_FIFO_SIZE_UART_RX = 64
.EQU  K16_FIFO_ADR_UART_RX = SRAM_START
;.EQU  K16_FIFO_ADR_UART_RX = \
;      K16_FIFO_ADR_FED + \
;      K16_FIFO_META_SIZE

.EQU  K16_FIFO_SIZE_UART_TX = 64
;.EQU  K16_FIFO_ADR_UART_TX = 
.EQU  K16_FIFO_ADR_UART_TX = \
      K16_FIFO_ADR_UART_RX + \
      K16_FIFO_SIZE_UART_RX + \
      K16_FIFO_META_SIZE_MX255V21
;-------------------------------------------------------------------------------
; init FIFO
      ;PUSH      RHT1
      ;PUSHP_0P  Y

      ;LDI       RHT1,K16_FIFO_SIZE_UART_RX
      ;PLD_0A1P  K16_FIFO_ADR_UART_RX,Y  ; ld RX ADR  into Y
      ;RCALL     SR_FIFO_INIT_0RHT1_SIZE_1PY_K16_FIFO_ADR_MX255V21

      ;LDI       RHT1,K16_FIFO_SIZE_UART_TX
      ;PLD_0A1P  K16_FIFO_ADR_UART_TX,Y  ; ld RX ADR  into Y
      ;RCALL     SR_FIFO_INIT_0RHT1_SIZE_1PY_K16_FIFO_ADR_MX255V21

      ;POPP_0P   Y
      ;POP       RHT1
;*******************************************************************************
        ;PLD_0A1P  K16_FIFO_ADR_UART_RX,Y
        ;RCALL     SR_UART_FIFO_RX_RD_0PY_FIFO_ADR
        ;LDD       RHT1,Y+AD_FIFO_BUF0
SR_UART_FIFO_RX_RD_0PY_FIFO_ADR:
        PUSH  RHT1
        PUSH  RHT2
        LDD   RHT1,Y+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        TST   RHT1                       ; check if null
        BREQ  JP_SR_UART_FIFO_RX_RD_NUL_EXIT ; nul exit
        RCALL SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21  ; not nul, read out into RHT2
  JP_SR_UART_FIFO_RX_RD_CHK_FUL_TRUE_SET_RXCIE0:
        LDD     RHT1,Y+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        LDD     RHT2,Y+AD_FIFO_SIZE
        ;CPI    RHT1,FIFO_SIZE-1             ; check full (size-1)
        CP      RHT1,RHT2
        BRNE    JP_SR_UART_FIFO_RX_RD_EXIT
        ;LDS    RHT1,SREG_UCSR                       ; enable RXCIE0
        RAMRDAR SREG_UCSR,RHT1
        SBR     RHT1,(1<<BIT_RXCIE)
        ;STS    SREG_UCSR,RHT1
        RAMWRRA RHT1,SREG_UCSR
  JP_SR_UART_FIFO_RX_RD_NUL_EXIT:
  JP_SR_UART_FIFO_RX_RD_EXIT:
        POP   RHT2
        POP   RHT1
        RET
;*******************************************************************************
        ;PLD_0A1P  K16_FIFO_ADR_UART_TX,Y
        ;STD       Y+AD_FIFO_BUF0,RHT1
        ;RCALL     SR_UART_FIFO_TX_WR_0PY_FIFO_ADR
SR_UART_FIFO_TX_WR_0PY_FIFO_ADR:
        PUSH  RHT1
        PUSH  RHT2
        LDD   RHT1,Y+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        LDD   RHT2,Y+AD_FIFO_SIZE
        CP    RHT1,RHT2
        BREQ  JP_SR_UART_FIFO_TX_WR_FUL_EXIT ; ful exit
        RCALL SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21
 JP_SR_UART_FIFO_TX_WR_CHK_NUL_TRUE_SET_UDRIE0:
        LDD     RHT1,Y+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        TST     RHT1                       ; check if null
        BRNE    JP_SR_UART_FIFO_TX_WR_EXIT
        ;LDS    RHT1,SREG_UCSR                       ; enable UDRIE0
        RAMRDAR SREG_UCSR,RHT1
        SBR     RHT1,(1<<BIT_UDRIE)
        ;STS    SREG_UCSR,RHT1
        RAMWRRA RHT1,SREG_UCSR
  JP_SR_UART_FIFO_TX_WR_FUL_EXIT:
  JP_SR_UART_FIFO_TX_WR_EXIT:
        POP   RHT2
        POP   RHT1
        RET
;*******************************************************************************
ISR_URXC: ; RX, save UDR into FIFO, auto dis when FIFO FULL
        PUSH          RHT2
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
        PUSHP_0P      Y
  JP_ISR_URXC_FIFO_CHK_FUL:
        PLD_0A1P K16_FIFO_ADR_UART_RX,Y
        LDD   RHT1,Y+AD_FIFO_LEN        ; LD FIFO_LEN value into RHT1
        LDD   RHT2,Y+AD_FIFO_SIZE       ; LD FIFO_SIZE value into RHT2
        CP    RHT1,RHT2                 ; check if full
        BREQ  JP_ISR_URXC_CLR_RXCIE0    ; if full, disable RXCIE0
  JP_ISR_URXC_FIFO_WR:
        RAMRDAR SREG_UDR,RHT1
        STD     Y+AD_FIFO_BUF0,RHT1
        RCALL   SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21
        RJMP    JP_ISR_URXC_EXIT
  JP_ISR_URXC_CLR_RXCIE0:
        ;LDS    RHT1,SREG_UCSR
        RAMRDAR SREG_UCSR,RHT1
        CBR     RHT1,BIT_RXCIE
        ;STS    SREG_UCSR,RHT1
        RAMWRRA RHT1,SREG_UCSR
  JP_ISR_URXC_EXIT:
        POPP_0P     Y
        POPSREG_0RT RHT1
        POP         RHT1
        POP         RHT2
        RETI
;*******************************************************************************
ISR_UDRE: ; TX, fetch new byte from FIFO to tx, auto dis when FIFO NUL
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
        PUSHP_0P      Y
  JP_ISR_UDRE_FIFO_CHK_NUL:
        PLD_0A1P K16_FIFO_ADR_UART_TX,Y
        LDD   RHT1,Y+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        TST   RHT1                        ; check if null
        BREQ  JP_ISR_UDRE_CLR_UDRIE       ; if null, disable UDRIE0
  JP_ISR_UDRE_FIFO_RD:
        RCALL   SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21
        LDD     RHT1,Y+AD_FIFO_BUF0
        ;STS    SREG_UDR,RHT1
        RAMWRRA RHT1,SREG_UDR
        RJMP    JP_ISR_URXC_EXIT
  JP_ISR_UDRE_CLR_UDRIE:
        ;LDS    RHT1,SREG_UCSR                 ; disable UDRIE0
        RAMRDAR SREG_UCSR,RHT1
        CBR     RHT1,(1<<BIT_UDRIE)
        ;STS    SREG_UCSR,RHT1
        RAMWRRA RHT1,SREG_UCSR
  JP_ISR_UDRE_EXIT:
        POPP_0P     Y
        POPSREG_0RT RHT1
        POP         RHT1
        RETI
;*******************************************************************************
