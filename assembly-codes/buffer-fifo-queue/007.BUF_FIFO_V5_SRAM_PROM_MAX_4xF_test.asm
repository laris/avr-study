.INCLUDE "M328PDEF.INC"
;.INCLUDE "TN85DEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
.INCLUDE "AVR_MACRO_MISC1.INC"
;-------------------------------------------------------------------------------
.CSEG
;-------------------------------------------------------------------------------
.ORG  $0
      RJMP INIT
;-------------------------------------------------------------------------------
.ORG  INT_VECTORS_SIZE ; first usable CSEG
;-------------------------------------------------------------------------------
.INCLUDE "007.BUF_FIFO_V5_SRAM_PROM_MAX_4xF.INC"
INIT:
      ;INIT_SP
      LDI R16,HIGH(0x7FE)
      OUT SPH,R16
      LDI R16, LOW(0x7FE)
      OUT SPL,R16
;.EQU  SIM_DIFF = 2048 - 1791
;.EQU  FIFO_SIZE = SRAM_SIZE - FIFO_META_SIZE - SIM_DIFF
;      LDI   R_FIFO_SIZEL, LOW(FIFO_SIZE)
;      LDI   R_FIFO_SIZEH,HIGH(FIFO_SIZE)
;      PLD_0A1P ADR_FIFO,Z 
;      RCALL SR_FIFO_INIT_0RSIZE_1ADR_FIFO_2PZ

INIT_FIFO_RAOM:
ADR_FIFO_PROM:
.DB 10,0,0,1,2,3,4,5,6,7,8,9
.EQU  ADR_FIFO_SRAM = SRAM_START
        PLD_0A1P ADR_FIFO_SRAM,Y
        PLD_0A1P (ADR_FIFO_PROM<<1),Z
        RCALL SR_FIFO_RAOM_INIT_0PY_ADR_FIFO_SRAM_1PZ_ADR_FIFO_PROM
MAIN:
;        LDI   R_FIFO_BUF0,$FF
;        CLZ
  JP_LP:
        LDD   RHT1,Y+AD_FIFO_SREG   ; load SREG
        SBRC  RHT1,FLG_FIFO_SREG_NUL
        RJMP  END
        RCALL SR_FIFO_RAOM_RD_0RBUF0_1PY_ADR_FIFO_SRAM
        PUSH  RBUF0
        RJMP  JP_LP

  RJMP  MAIN

END:
        RJMP END