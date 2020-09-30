;*******************************************************************************
;.INCLUDE "007.BUF_FIFO_V2_MAX255.INC"
;.DSEG                   ; allocate data space for FIFO + meta data
;.EQU FIFO_SIZE    = 255 ; [1,256] max=256
;A_FIFO_BASE_RX:  .BYTE (FIFO_SIZE+3)
;*******************************************************************************
;.INCLUDE "M328PDEF.INC"
;.INCLUDE "AVR_MACRO.INC"
;*******************************************************************************
; This DSEG can allocate [1,255] max=255 Bytes FIFO size
; space usage: RAM = FIFO bytes size + 4 (1 x LEN + 2 x PTR)
; A_FIFO_BASE must define in DSEG
; FIFO_SIZE=[1,255]
; .DSEG                   ; allocate data space for FIFO + meta data
; .EQU FIFO_SIZE    = 255 ; [1,256] max=256
; A_FIFO_BASE_RX:  .BYTE (FIFO_SIZE+3)
;A_FIFO_BASE_TX:  .BYTE (FIFO_SIZE+3)
;A_FIFO_BASE_RX_TST:  .BYTE (FIFO_SIZE)
;A_FIFO_BASE_TX_TST:  .BYTE (FIFO_SIZE)
.EQU D_FIFO_LEN     = 0          ; [0,255] max 255
.EQU D_FIFO_WRIN_P  = 1          ; [0,255]
.EQU D_FIFO_RDOUT_P = 2          ; [0,255]
.EQU D_FIFO_BASE    = 3          ; 0->255
;*******************************************************************************
;.CSEG
;.ORG $0
;      RJMP JP_RX_WR
      ;RJMP JP_TX_WR
;*******************************************************************************
;        PLD_0A1P  A_FIFO_BASE_RX,Z        ; load FIFO base ADR value into Z
;        LDD R16,Z+D_FIFO_LEN           ; load FIFO_LEN value into R16
;        CPI R16,FIFO_SIZE              ; FIFO_LEN ?=FIFO_SIZE
;        BRLO JP_FIFO_WR_R17_Rh16T_PZ_MAX255        ; write R17 into buffer
;*******************************************************************************
SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255:
;        PLD_0A1P  A_FIFO_BASE_RX,Z        ; load FIFO base ADR value into Z
        LDD R16,Z+D_FIFO_LEN           ; load FIFO_LEN value into R16
        CPI R16,FIFO_SIZE              ; FIFO_LEN ?=FIFO_SIZE
        BRLO JP_FIFO_WR_R17_Rh16T_PZ_MAX255        ; write R17 into buffer
        RET
;  JP_FIFO_WR_CHK_FUL_RT16:
;  JP_FIFO_WR_CHK_LEN_FUL:
;        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
;        CPI R16,FIFO_SIZE           ; FIFO_LEN ?=FIFO_SIZE
;        BREQ JP_FIFO_WR_ERR_FUL_EXIT          ; full, jp error
SR_FIFO_WR_R17_Rh16T_PZ_MAX255:
  JP_FIFO_WR_R17_Rh16T_PZ_MAX255:
  JP_FIFO_WR_INC_LEN:
        INC R16                       ; inc len
  JP_FIFO_WR_UPDATE_LEN:                          ; not full, write
        STD Z+D_FIFO_LEN,R16          ; save len
  JP_FIFO_WR_LOAD_WRIN_PTR:
        LDD R16,Z+D_FIFO_WRIN_P       ; load FIFO_WRIN_P var in R16
  JP_FIFO_WR_WRITE_DATA:
        CLC
        ADD ZL, R16                    ; move ptr = base+wrinp_var
  JP_FIFO_WR_ZH_ADC0:
        BRCC JP_FIFO_WR_ZH_NO_ADC0
        INC ZH
  JP_FIFO_WR_ZH_NO_ADC0:
        STD Z+D_FIFO_BASE,R17       ; write R17 into FIFO
  JP_FIFO_WR_ZH_SBIC0:
        BRCC JP_FIFO_WR_ZH_NO_SBIC0
        DEC ZH
  JP_FIFO_WR_ZH_NO_SBIC0:
        SUB ZL, R16
  JP_FIFO_WR_CHK_WRIN_PTR_END:
        CPI R16,FIFO_SIZE           ; R16+1 ?<= FIFO_SIZE
        BRNE JP_FIFO_WR_INC_WRIN_PTR  ; if WRIN_P point to last Byte, next OV
  JP_FIFO_WR_RESET_WRIN_PTR_0:                   ; rollback ptr to 0
        CLR R16
        RJMP JP_FIFO_WR_UPDATE_WRIN_PTR
  JP_FIFO_WR_INC_WRIN_PTR:
        INC R16
  JP_FIFO_WR_UPDATE_WRIN_PTR:
        STD Z+D_FIFO_WRIN_P,R16       ; save WRIN_P
  JP_FIFO_WR_ERR_FUL_EXIT:
        RET
;*******************************************************************************
;        PLD_0A1P  A_FIFO_BASE_RX,Z       ; load FIFO base ADR value into Z
;        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
;        CPI R16,1                    ; FIFO_LEN ?=FIFO_SIZE
;        BRSH JP_FIFO_RD_R17_Rh16T_PZ_MAX255 ; read data into R17 from buffer
;*******************************************************************************
SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255:
        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
        CPI R16,1                    ; FIFO_LEN ?=FIFO_SIZE
        BRSH JP_FIFO_RD_R17_Rh16T_PZ_MAX255 ; read data into R17 from buffer
        RET
;  JP_FIFO_RD_CHK_NUL_RT16:
;  JP_FIFO_RD_CHK_LEN_NUL:
;        LDD R16,Z+D_FIFO_LEN
;        TST R16
;        BREQ JP_FIFO_RD_ERR_NUL_EXIT
SR_FIFO_RD_R17_Rh16T_PZ_MAX255:
  JP_FIFO_RD_R17_Rh16T_PZ_MAX255:
  JP_FIFO_RD_DEC_LEN:
        DEC R16
  JP_FIFO_RD_UPDATE_LEN:
        STD Z+D_FIFO_LEN,R16
  JP_FIFO_RD_LOAD_RDOUT_PTR:
        LDD R16,Z+D_FIFO_RDOUT_P
  JP_FIFO_RD_READ_DATA:
        CLC
        ADD ZL, R16
  JP_FIFO_RD_ZH_ADC0:
        BRCC JP_FIFO_RD_ZH_NO_ADC0
        INC ZH
  JP_FIFO_RD_ZH_NO_ADC0:
        LDD R17,Z+D_FIFO_BASE       ; read data out to R17
  JP_FIFO_RD_ZH_SBIC0:
        BRCC JP_FIFO_RD_ZH_NO_SBIC0
        DEC ZH
  JP_FIFO_RD_ZH_NO_SBIC0:
        SUB ZL, R16
  JP_FIFO_RD_CHK_RDOUT_PTR_END:
        CPI R16,FIFO_SIZE
        BRNE JP_FIFO_RD_INC_ROUT_PTR
  JP_FIFO_RD_RESET_RDOUT_PTR_0:
        CLR R16
        RJMP JP_FIFO_RD_UPDATE_RDOUT_PTR
  JP_FIFO_RD_INC_ROUT_PTR:
        INC R16
  JP_FIFO_RD_UPDATE_RDOUT_PTR:
        STD Z+D_FIFO_RDOUT_P,R16
  JP_FIFO_RD_ERR_NUL_EXIT:
        RET
;*******************************************************************************
;SR_FIFO_SCAN_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255:
        ;LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
        ;CPI R16,1                    ; FIFO_LEN ?=FIFO_SIZE
        ;BRSH JP_FIFO_SCAN_R17_Rh16T_PZ_MAX255 ; read data into R17 from buffer
        ;RET
;SR_FIFO_SCAN_RD_LEN_OUT_R16:
;  JP_FIFO_SCAN_CHK_NUL_RT16:
;  JP_FIFO_SCAN_CHK_LEN_NUL:
;        LDD R16,Z+D_FIFO_LEN
;        RET
;        TST R16
;        BREQ JP_FIFO_SCAN_ERR_NUL_EXIT
;SR_FIFO_RD_R17_Rh16T_PZ_MAX255:
;  JP_FIFO_SCAN_R17_Rh16T_PZ_MAX255:
;  JP_FIFO_SCAN_DEC_LEN:
;        ;DEC R16
;  JP_FIFO_SCAN_UPDATE_LEN:
        ;STD Z+D_FIFO_LEN,R16

SR_FIFO_SCAN_RD_IN_R16_DATA_INDEX_OUT_R17_PZ:
  JP_FIFO_SCAN_LOAD_RDOUT_PTR:
        LDD R17,Z+D_FIFO_RDOUT_P
        CLC
        ADD R17, R16 ; ptr = RDOUT_PTR+displacement
        BRCC JP_FIFO_SCAN_READ_DATA
        INC ZH
  JP_FIFO_SCAN_READ_DATA:
        CLC
        ADD ZL, R17
  JP_FIFO_SCAN_ZH_ADC0:
        BRCC JP_FIFO_SCAN_ZH_NO_ADC0
        INC ZH
  JP_FIFO_SCAN_ZH_NO_ADC0:
        LDD R17,Z+D_FIFO_BASE       ; read data out to R17
  JP_FIFO_SCAN_ERR_NUL_EXIT:
        RET
;*******************************************************************************
;-------------------------------------------------------------------------------
; PASS, write 255 x [255,1] into buffer and read out
; 1 x RX [258]
; 1 x RX_read_out [255]
; 1 x TX [258]
; 1 x TX_read_out [255]
; test SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;JP_RX_WR:
;        LDI R17, 255
;JP_RX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;        DEC R17
;        BRNE JP_RX_WR_LOOP
;        LDI R17, $FF
;        ;PLD_0A1P A_FIFO_BASE_RX,Z
;        ;RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;        RJMP JP_RX_RD
;-------------------------------------------------------------------------------
;JP_RX_RD:
;        LDI R18, 255
;        PLD_0A1P A_FIFO_BASE_RX_TST,Y
;JP_RX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
;        ST Y+,R17
;        DEC R18
;        BRNE JP_RX_RD_LOOP
        ;LDI R17, $FF
        ;PLD_0A1P A_FIFO_BASE_RX,Z
        ;RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
        ;ST Y+,R17
        ;LDI R17, $FF
        ;PLD_0A1P A_FIFO_BASE_RX,Z
        ;RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
        ;ST Y+,R17
;        RJMP JP_TX_WR
;-------------------------------------------------------------------------------
;JP_TX_WR:
;        LDI R17, 255
;JP_TX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;        DEC R17
;        BRNE JP_TX_WR_LOOP
;        ;LDI R17, $FF
;        ;PLD_0A1P A_FIFO_BASE_TX,Z
;        ;RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;        RJMP JP_TX_RD
;-------------------------------------------------------------------------------
;JP_TX_RD:
;        LDI R19, 255
;        PLD_0A1P A_FIFO_BASE_TX_TST,Y
;JP_TX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
;        ST Y+,R17
;        DEC R19
;        BRNE JP_TX_RD_LOOP
        ;PLD_0A1P A_FIFO_BASE_TX,Z
        ;RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
        ;ST Y+,R17
;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------
; PASS, write 255 x [255,1] into buffer and read out
; write 255 and read 100 out, then write another 155 and read 155 out
; check if the in/out ptr in correct location
; 1 x RX [258]
; 1 x RX_read_out [255]
; 1 x TX [258]
; 1 x TX_read_out [255]
; test SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;JP3_RX_WR:
;        LDI R17, 255
;JP3_RX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;        DEC R17
;        BRNE JP3_RX_WR_LOOP
;JP4_RX_RD:
;        LDI R18, 100
;        PLD_0A1P A_FIFO_BASE_RX_TST,Y
;JP4_RX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
;        ST Y+,R17
;        DEC R18
;        BRNE JP4_RX_RD_LOOP
;JP5_RX_WR:
;        LDI R17, 100
;JP5_RX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_R17_Rh16T_PZ_MAX255
;        DEC R17
;        BRNE JP5_RX_WR_LOOP
;JP6_RX_RD:
;        LDI R18, 255
;        ;PLD_0A1P A_FIFO_BASE_RX_TST,Y
;JP6_RX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_R17_Rh16T_PZ_MAX255
;        ST Y+,R17
;        DEC R18
;        BRNE JP6_RX_RD_LOOP
;-------------------------------------------------------------------------------
;END:
;        RJMP END
;-------------------------------------------------------------------------------