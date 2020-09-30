;*******************************************************************************
;.INCLUDE "007.BUF_FIFO_V2.1_MAX255.INC"
;.INCLUDE "M328PDEF.INC"
;.INCLUDE "AVR_MACRO.INC"
;*******************************************************************************
;.EQU K16_FIFO_ADR_MX255V21   = SRAM_START
.EQU K16_FIFO_META_SIZE_MX255V21 = 5
.EQU AD_FIFO_SIZE       = 0 ; [1,255]
.EQU AD_FIFO_LEN        = 1 ; [0,255] ; LEN limit PTR 
.EQU AD_FIFO_PWRIN      = 2 ; [0,SIZE-1]
.EQU AD_FIFO_PRDOUT     = 3 ; [0,SIZE-1]
.EQU AD_FIFO_BUF0       = 4 ;
.EQU AD_FIFO_DATA       = 5 ; 0->255
;*******************************************************************************
;.DEF RHT1 = R16
;*******************************************************************************
SR_FIFO_INIT_0RHT1_SIZE_1PY_K16_FIFO_ADR_MX255V21:
        ;PUSH     RHT1 ; SIZE
        ;PUSHP_0P Y
        ;.EQU     K16_FIFO_SIZE = 255
        ;LDI      RHT1,K16_FIFO_SIZE
        ;MOV      RHT1,R0_SIZE
        ;.EQU     K16_FIFO_ADR_MX255V21 = SRAM_START
        ;PLD_0A1P K16_FIFO_ADR_MX255V21,Y
        ;RCALL    SR_FIFO_INIT_0RHT1_SIZE_1PY_K16_FIFO_ADR_MX255V21
        ;POPP_0P  Y 
        ;POP      RHT1
;-------------------------------------------------------------------------------
  JP_FIFO_INIT_SAVE_SIZE:
        STD     Y+AD_FIFO_SIZE,RHT1 ; save SIZE
SR_FIFO_INIT_RST_WR_PPRHT1_0PY_K16_FIFO_ADR_MX255V21:
  JP_FIFO_INIT_CLR_META:
        CLR     RHT1
        STD     Y+AD_FIFO_LEN,RHT1
        STD     Y+AD_FIFO_PWRIN,RHT1
        STD     Y+AD_FIFO_PRDOUT,RHT1
        RET
SR_FIFO_INIT_RST_RD_PPRHT1_0PY_K16_FIFO_ADR_MX255V21:
        LDD     RHT1,Y+AD_FIFO_SIZE
        STD     Y+AD_FIFO_LEN,RHT1
        RET
;*******************************************************************************
SR_FIFO_WR_CHK_FUL_0PY_K16_FIFO_ADR_MX255V21:
;        PUSHP_0P     X
;        PUSHP_0P     Y
;        PUSHSREG_0RT YL
;        PLD_0A1P     K16_FIFO_ADR_MX255V21,Y       ; ld FIFO ADR into Y
;        RCALL        SR_FIFO_WR_CHK_FUL_0PY_K16_FIFO_ADR_MX255V21
;        POPSREG_0RT  YL
;        POPP_0P      Y
;        POPP_0P      X
;-------------------------------------------------------------------------------
        LDD   XL,Y+AD_FIFO_LEN            ; load  LEN value into XL
        LDD   XH,Y+AD_FIFO_SIZE           ; load SIZE value into XH
        CP    XL,XH                       ; FIFO_LEN ?=FIFO_SIZE
        BRLO  JP_RCALL_SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21 ; write RHT2 into buffer
        ;BREQ  JP_FUL_EXIT
        RET
;*******************************************************************************
JP_RCALL_SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21:
;        PUSHP_0P     Y
;        PUSHSREG_0RT YL
;        PLD_0A1P     K16_FIFO_ADR_MX255V21,Y       ; ld FIFO ADR into Y
;        STD          Y+AD_FIFO_BUF0,RHT1
;        RCALL        SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21
;        POPSREG_0RT  YL
;        POPP_0P      Y
;-------------------------------------------------------------------------------
SR_FIFO_WR_BUF_0PY_K16_FIFO_ADR_MX255V21:
        PUSHP_0P      X
        PUSHSREG_0RT  XL
  JP_FIFO_WR_LD_LEN:
        LDD   XL,Y+AD_FIFO_LEN      ; ld LEN XL
  JP_FIFO_WR_INC_LEN:
        INC   XL                    ; inc LEN
  JP_FIFO_WR_UPDATE_LEN:            ; not full, update LEN 
        STD   Y+AD_FIFO_LEN,XL      ; save len ------------ LEN DONE
  JP_FIFO_WR_LOAD_WRIN_PTR:         ; ld PWRIN
        LDD   XL,Y+AD_FIFO_PWRIN  ; ld PWRIN XL
        LDD   XH,Y+AD_FIFO_SIZE     ; ld SIZE  XH
        DEC   XH                    ; SIZE-1 = END of PTR
        CP    XL,XH                 ; PTR ?== SIZE
        BREQ  JP_FIFO_WR_RESET_WRIN_PTR_0; if WRIN_P point to last Byte, next OV
  JP_FIFO_WR_INC_WRIN_PTR:          ; BRLO
        INC   XL
        RJMP  JP_FIFO_WR_UPDATE_WRIN_PTR
  JP_FIFO_WR_RESET_WRIN_PTR_0:                   ; rollback ptr to 0
        CLR   XL
  JP_FIFO_WR_UPDATE_WRIN_PTR:
        STD   Y+AD_FIFO_PWRIN,XL    ; save PWRIN
  JP_FIFO_WR_WRITE_DATA:
        TST   XL                    ; now XL = PWRIN +1 or rollover to 0
        BRNE  JP_FIFO_WR_PWRIN_NOT_END
  JP_FIFO_WR_PWRIN_END:
        LDD   XL,Y+AD_FIFO_SIZE       ; PWRIN = SIZE-1 = FIFO END
        ;RJMP  JP_FIFO_WR_PWRIN_NOT_END_ADD168
  JP_FIFO_WR_PWRIN_NOT_END:
        DEC   XL
  JP_FIFO_WR_LD_BUF:
        LDD   XH,Y+AD_FIFO_BUF0     ; ld WRBUF XH
  JP_FIFO_WR_PWRIN_NOT_END_ADD168:
        ADD   YL,XL                  ; move ptr = base + pwrin
        CLR   XL
        ADC   YH,XL
        ;RJMP  JP_FIFO_WR_BUF_INTO_FIFO
  JP_FIFO_WR_BUF_INTO_FIFO:
        STD   Y+AD_FIFO_DATA,XH     ; wr XL into FIFO
  JP_FIFO_WR_ERR_FUL_EXIT:
        POPSREG_0RT XL 
        POPP_0P     X
        RET
;*******************************************************************************
SR_FIFO_RD_CHK_NUL_0PY_K16_FIFO_ADR_MX255V21:
;        PUSH         RHT1
;        PUSHP_0P     Y
;        PUSHSREG_0RT YL
;        PLD_0A1P     K16_FIFO_ADR_MX255V21,Y       ; ld FIFO ADR into Y
;        RCALL        SR_FIFO_RD_CHK_NUL_0PY_K16_FIFO_ADR_MX255V21
;        POPSREG_0RT  YL
;        POPP_0P      Y
;        POP          RHT1
;-------------------------------------------------------------------------------
        LDD   RHT1,Y+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        TST   RHT1                        ; FIFO_LEN ?=K16_FIFO_SIZE
        BRNE  JP_RCALL_SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21 ;RD FIFO into BUF
        ;BREQ  JP_NUL_EXIT
        RET
;*******************************************************************************
JP_RCALL_SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21:
;        PUSHP_0P     Y
;        PUSHSREG_0RT YL
;        PLD_0A1P     K16_FIFO_ADR_MX255V21,Y       ; ld FIFO ADR into Y
;        RCALL        SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21
;        LDD          RHT1,Y+AD_FIFO_BUF0
;        POPSREG_0RT  YL
;        POPP_0P      Y
;-------------------------------------------------------------------------------
SR_FIFO_RD_BUF_0PY_K16_FIFO_ADR_MX255V21:
        PUSHP_0P      X
        PUSHSREG_0RT  XL
        LDD   XL,Y+AD_FIFO_LEN      ; ld LEN XL
  JP_FIFO_RD_DEC_LEN:
        DEC   XL                    ; dec LEN
  JP_FIFO_RD_UPDATE_LEN:            ; not null, update LEN 
        STD   Y+AD_FIFO_LEN,XL      ; save len ------------ LEN DONE
  JP_FIFO_RD_PRDOUT_NOT_END_ADD168:
        LDD   XL,Y+AD_FIFO_PRDOUT   ; ld PRDOUT XL
        ADD   YL,XL                  ; move ptr = base + pwrin
        CLR   XH
        ADC   YH,XH
        ;RJMP  JP_FIFO_RD_BUF_INTO_FIFO
  JP_FIFO_RD_BUF_INTO_FIFO:
        LDD   XH,Y+AD_FIFO_DATA     ; RD FIFO
  JP_FIFO_RD_RESET_Y:
        SUB   YL,XL
        CLR   XL
        SBC   YH,XL
        STD   Y+AD_FIFO_BUF0,XH     ; WR RDBUF
  JP_FIFO_RD_LOAD_RDOUT_PTR:        ; 
        LDD   XL,Y+AD_FIFO_PRDOUT   ; ld PRDOUT XL
        LDD   XH,Y+AD_FIFO_SIZE     ; ld SIZE  XH
        DEC   XH                    ; SIZE-1 = PTR END
        CP    XL,XH                 ; PTR ?== SIZE
        BREQ  JP_FIFO_RD_RESET_RDOUT_PTR_0; PRDOUT point to last Byte, next OV
  JP_FIFO_RD_INC_RDOUT_PTR:          ; BRLO
        INC   XL
        RJMP  JP_FIFO_RD_UPDATE_RDOUT_PTR
  JP_FIFO_RD_RESET_RDOUT_PTR_0:                   ; rollback ptr to 0
        CLR   XL
  JP_FIFO_RD_UPDATE_RDOUT_PTR:
        STD   Y+AD_FIFO_PRDOUT,XL    ; save PRDOUT
 
  JP_FIFO_RD_ERR_FUL_EXIT:
        POPSREG_0RT XL 
        POPP_0P     X
        RET
;*******************************************************************************
;SR_FIFO_SCAN_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ:
        ;LDD RHT1,Z+AD_FIFO_LEN          ; load FIFO_LEN value into RHT1
        ;CPI RHT1,1                    ; FIFO_LEN ?=K16_FIFO_SIZE
        ;BRSH JP_FIFO_SCAN_RHT2_RHT1_PZ ; read data into RHT2 from buffer
        ;RET
;SR_FIFO_SCAN_RD_LEN_OUT_RHT1:
;  JP_FIFO_SCAN_CHK_NUL_RT16:
;  JP_FIFO_SCAN_CHK_LEN_NUL:
;        LDD RHT1,Z+AD_FIFO_LEN
;        RET
;        TST RHT1
;        BREQ JP_FIFO_SCAN_ERR_NUL_EXIT
;SR_FIFO_RD_RHT2_RHT1_PZ:
;  JP_FIFO_SCAN_RHT2_RHT1_PZ:
;  JP_FIFO_SCAN_DEC_LEN:
;        ;DEC RHT1
;  JP_FIFO_SCAN_UPDATE_LEN:
        ;STD Z+AD_FIFO_LEN,RHT1

;SR_FIFO_SCAN_RD_IN_RHT1_DATA_INDEX_OUT_RHT2_PZ:
;  JP_FIFO_SCAN_LOAD_RDOUT_PTR:
;        LDD RHT2,Z+AD_FIFO_PRDOUT
;        CLC
;        ADD RHT2, RHT1 ; ptr = RDOUT_PTR+displacement
;        BRCC JP_FIFO_SCAN_READ_DATA
;        INC ZH
;  JP_FIFO_SCAN_READ_DATA:
;        CLC
;        ADD ZL, RHT2
;  JP_FIFO_SCAN_ZH_ADC0:
;        BRCC JP_FIFO_SCAN_ZH_NO_ADC0
;        INC ZH
;  JP_FIFO_SCAN_ZH_NO_ADC0:
;        LDD RHT2,Z+AD_FIFO_DATA       ; read data out to RHT2
;  JP_FIFO_SCAN_ERR_NUL_EXIT:
;        RET
;*******************************************************************************
;-------------------------------------------------------------------------------
; PASS, write 255 x [255,1] into buffer and read out
; 1 x RX [258]
; 1 x RX_read_out [255]
; 1 x TX [258]
; 1 x TX_read_out [255]
; test SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;JP_RX_WR:
;        LDI RHT2, 255
;JP_RX_WR_LOOP:
;        PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;        DEC RHT2
;        BRNE JP_RX_WR_LOOP
;        LDI RHT2, $FF
;        ;PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        ;RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;        RJMP JP_RX_RD
;-------------------------------------------------------------------------------
;JP_RX_RD:
;        LDI R18, 255
;        PLD_0A1P K16_FIFO_ADR_MX255V21_TST,Y
;JP_RX_RD_LOOP:
;        PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
;        ST Y+,RHT2
;        DEC R18
;        BRNE JP_RX_RD_LOOP
        ;LDI RHT2, $FF
        ;PLD_0A1P K16_FIFO_ADR_MX255V21,Z
        ;RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
        ;ST Y+,RHT2
        ;LDI RHT2, $FF
        ;PLD_0A1P K16_FIFO_ADR_MX255V21,Z
        ;RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
        ;ST Y+,RHT2
;        RJMP JP_TX_WR
;-------------------------------------------------------------------------------
;JP_TX_WR:
;        LDI RHT2, 255
;JP_TX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;        DEC RHT2
;        BRNE JP_TX_WR_LOOP
;        ;LDI RHT2, $FF
;        ;PLD_0A1P A_FIFO_BASE_TX,Z
;        ;RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;        RJMP JP_TX_RD
;-------------------------------------------------------------------------------
;JP_TX_RD:
;        LDI R19, 255
;        PLD_0A1P A_FIFO_BASE_TX_TST,Y
;JP_TX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
;        ST Y+,RHT2
;        DEC R19
;        BRNE JP_TX_RD_LOOP
        ;PLD_0A1P A_FIFO_BASE_TX,Z
        ;RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
        ;ST Y+,RHT2
;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------
; PASS, write 255 x [255,1] into buffer and read out
; write 255 and read 100 out, then write another 155 and read 155 out
; check if the in/out ptr in correct location
; 1 x RX [258]
; 1 x RX_read_out [255]
; 1 x TX [258]
; 1 x TX_read_out [255]
; test SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;JP3_RX_WR:
;        LDI RHT2, 255
;JP3_RX_WR_LOOP:
;        PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;        DEC RHT2
;        BRNE JP3_RX_WR_LOOP
;JP4_RX_RD:
;        LDI R18, 100
;        PLD_0A1P K16_FIFO_ADR_MX255V21_TST,Y
;JP4_RX_RD_LOOP:
;        PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
;        ST Y+,RHT2
;        DEC R18
;        BRNE JP4_RX_RD_LOOP
;JP5_RX_WR:
;        LDI RHT2, 100
;JP5_RX_WR_LOOP:
;        PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        RCALL SR_FIFO_WR_CHK_FUL_FALSE_WR_RHT2_RHT1_PZ
;        DEC RHT2
;        BRNE JP5_RX_WR_LOOP
;JP6_RX_RD:
;        LDI R18, 255
;        ;PLD_0A1P K16_FIFO_ADR_MX255V21_TST,Y
;JP6_RX_RD_LOOP:
;        PLD_0A1P K16_FIFO_ADR_MX255V21,Z
;        RCALL SR_FIFO_RD_CHK_NUL_FALSE_RD_RHT2_RHT1_PZ
;        ST Y+,RHT2
;        DEC R18
;        BRNE JP6_RX_RD_LOOP
;-------------------------------------------------------------------------------
;END:
;        RJMP END
;-------------------------------------------------------------------------------