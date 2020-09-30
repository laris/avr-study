.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_MISC1.INC"
.CSEG
.ORG  $0
      RJMP INIT
;-------------------------------------------------------------------------------
.ORG  INT_VECTORS_SIZE ; first usable CSEG
.INCLUDE "007.BUF_FIFO_V2.1_MAX255.INC"

INIT:
      INIT_SP

MAIN:
.DEF RHT1 = R16

INIT_FIFO_1:
        PUSHP_0P X
        PUSHP_0P Y
        ;LDI/MOV  XL,K16_FIFO_SIZE
        .EQU      K16_FIFO_SIZE = 255; 32; 2 ;55
        LDI       RHT1,K16_FIFO_SIZE
        .EQU      K16_FIFO_ADR_MX255V21_1 = SRAM_START
        PLD_0A1P  K16_FIFO_ADR_MX255V21_1,Y
        RCALL     SR_FIFO_INIT_0RHT1_SIZE_1PY_K16_FIFO_ADR_MX255V21
        POPP_0P   Y 
        POPP_0P   X

        LDI       RHT1,0
INIT_FIFO_WITH_FF:
;        PUSHP_0P     X
;        PUSHP_0P     Y
;        PUSHSREG_0RT YL
;        PLD_0A1P     K16_FIFO_ADR_MX255V21_1,Y       ; ld FIFO ADR into Y
;        RCALL        SR_FIFO_WR_CHK_FUL_0PY_K16_FIFO_ADR_MX255V21
;        POPSREG_0RT  YL
;        POPP_0P      Y
;        POPP_0P      X
;-------------------------------------------------------------------------------
        PLD_0A1P K16_FIFO_ADR_MX255V21_1,Y       ; ld FIFO ADR into Y
        LDD   XL,Y+AD_FIFO_LEN            ; load  LEN value into XL
        LDD   XH,Y+AD_FIFO_SIZE           ; load SIZE value into XH
        CP    XL,XH                       ; FIFO_LEN ?=FIFO_SIZE
        BRLO  JP_WR_NEW_DATA              ; write RHT2 into buffer
        BREQ  JP_FILL_FIFO_1_EXIT

JP_WR_NEW_DATA:
        PLD_0A1P  K16_FIFO_ADR_MX255V21_1,Y       ; ld FIFO ADR into Y
        INC       RHT1
        STD       Y+AD_FIFO_BUF0,RHT1
        RCALL     SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21
        RJMP      INIT_FIFO_WITH_FF
JP_FILL_FIFO_1_EXIT:

;RJMP JP_RD_FIFO1_COPY_TO_FIFO2

INIT_FIFO_2:
        PUSHP_0P X
        PUSHP_0P Y
        ;LDI/MOV  XL,K16_FIFO_SIZE
        ;.EQU      K16_FIFO_SIZE = 255
        LDI       RHT1,K16_FIFO_SIZE
        .EQU      K16_FIFO_ADR_MX255V21_2 = \
                  K16_FIFO_ADR_MX255V21_1 + \
                  K16_FIFO_META_SIZE_MX255V21 + \
                  K16_FIFO_SIZE
        PLD_0A1P  K16_FIFO_ADR_MX255V21_2,Y
        RCALL     SR_FIFO_INIT_0RHT1_SIZE_1PY_K16_FIFO_ADR_MX255V21
        POPP_0P   Y 
        POPP_0P   X

JP_RD_FIFO1_COPY_TO_FIFO2:
        PLD_0A1P K16_FIFO_ADR_MX255V21_1,Y
        LDD   RHT1,Y+AD_FIFO_LEN
        TST   RHT1 
        BRNE  JP_RD_FIFO1
        BREQ  END

JP_RD_FIFO1:
        PLD_0A1P K16_FIFO_ADR_MX255V21_1,Y
        RCALL SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21
        LDD   RHT1,Y+AD_FIFO_BUF0
;RJMP JP_RD_FIFO1_COPY_TO_FIFO2

JP_WR_FIFO2:
        PLD_0A1P  K16_FIFO_ADR_MX255V21_2,Y
        STD       Y+AD_FIFO_BUF0,RHT1
        RCALL     SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21
        RJMP      JP_RD_FIFO1_COPY_TO_FIFO2
 
END:
        RJMP END