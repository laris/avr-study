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
.INCLUDE "007.BUF_FIFO_V4_MAX_4xF.INC"
INIT:
.EQU SIM_DIFF = 2048 - 1791
.EQU  FIFO_SIZE = SRAM_SIZE - FIFO_META_SIZE - SIM_DIFF
      LDI   R_FIFO_SIZEL, LOW(FIFO_SIZE)
      LDI   R_FIFO_SIZEH,HIGH(FIFO_SIZE)
      PLD_0A1P ADR_FIFO,Z 
      RCALL SR_FIFO_INIT_0RSIZE_1ADR_FIFO_2PZ

MAIN:
        LDI   R_FIFO_BUF0,$FF
        CLZ
  JP_LP:
        RCALL SR_FIFO_WR_CHK_FIFO_FUL_SET_T
        BRTS  END
        RCALL SR_FIFO_WR_0RBUF_1ADR_FIFO_2PZ
        RJMP  JP_LP

  RJMP  MAIN

END:
        RJMP END