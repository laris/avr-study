;.INCLUDE "M328PDEF.INC"
;.INCLUDE "TN85DEF.INC"
;.INCLUDE "AVR_MACRO.INC"
;.INCLUDE "AVR_MACRO_IOREG.INC"
;.INCLUDE "AVR_MACRO_MISC1.INC"
;*******************************************************************************
; This DSEG can allocate [1,256] max=256 Bytes FIFO size
; space usage: RAM = FIFO bytes size + 4 (1 x LEN + 2 x PTR + 1 x 256 byte FLAG)
; A_FIFO_BASE must define in DSEG
; FIFO_SIZE=[1,256]
;.DSEG                   ; allocate data space for FIFO + meta data
;.EQU  FIFO_SIZE    = 256 ; [1,256] max=256
;A_FIFO_BASE_RX:  .BYTE (FIFO_SIZE+4)
;A_FIFO_BASE_TX:  .BYTE (FIFO_SIZE+4)
;A_FIFO_BASE_RX_TST:  .BYTE (FIFO_SIZE)
;A_FIFO_BASE_TX_TST:  .BYTE (FIFO_SIZE)
;*******************************************************************************
.EQU  ADR_FIFO        = SRAM_START
.EQU  FIFO_META_SIZE  = 11
.EQU  AD_FIFO_SIZEL   = 0   ; [1,SRAM_SIZE] AVR max addressable [0,$FFFF]
.EQU  AD_FIFO_SIZEH   = 1   ; SRAM_SIZE: M328P 2KB, TN85 512B
.EQU  AD_FIFO_LENL    = 2   ; [1,FIFO_SIZE]
.EQU  AD_FIFO_LENH    = 3
.EQU  AD_FIFO_WRINPL  = 4   ; [0,FIFO_SIZE-1]
.EQU  AD_FIFO_WRINPH  = 5
.EQU  AD_FIFO_RDOUTPL = 6   ; [0,FIFO_SIZE-1]
.EQU  AD_FIFO_RDOUTPH = 7
.EQU  AD_FIFO_BUF0    = 8
.EQU  AD_FIFO_BUF1    = 9
.EQU  AD_FIFO_SREG    = 10
.EQU  AD_FIFO_DATA    = 11
;-------------------------------------------------------------------------------
.EQU  FLG_FIFO_SREG_NUL= 7
.EQU  FLG_FIFO_SREG_FUL= 6
;-------------------------------------------------------------------------------
;.CSEG
.DEF  R_FIFO_SIZEL    = R18
.DEF  R_FIFO_SIZEH    = R19
;.EQU  R_FIFO_LENL     = R2
;.EQU  R_FIFO_LENH     = R3
;.EQU  R_FIFO_WRINPL   = R4
;.EQU  R_FIFO_WRINPH   = R5
;.EQU  R_FIFO_RDOUTPL  = R6
;.EQU  R_FIFO_RDOUTPH  = R7
.DEF  R_FIFO_BUF0     = R20
;.EQU  R_FIFO_BUFW     = R9
;.EQU  R_FIFO_SREG     = R10
.DEF  RHT1            = R16
;.DEF  RHT2            = R17
;.EQU  RHT3            = R18
;.EQU  RHT4            = R19
;.ORG $0
;      RJMP JP_RX_WR
      ;RJMP JP_TX_WR
;*******************************************************************************
SR_FIFO_INIT_0RSIZE_1ADR_FIFO_2PZ: 
; in PZ, SIZEHL, SRAM_ADR, via X_CNT, RHT1, SREG_Z
  JP_FIFO_INIT_PUSH:
        ;PUSHP_0P  Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
  JP_FIFO_INIT_MOV_PZ_2_FIFO_END:
        ;PLD_0A1P ADR_FIFO,Z
        ;MOVW  XH:XL,R_FIFO_SIZEH:R_FIFO_SIZEL
        ;ADDI16  XH,XL,FIFO_META_SIZE
        CLC
        ADDI16  ZH,ZL,(FIFO_META_SIZE-1)
        ;ADD16   ZH,ZL,R_FIFO_SIZEH,R_FIFO_SIZEL
        LDI     RHT1,0
        ST      Z,RHT1
        CLZ
  JP_FIFO_INIT_CLR_FIFO_LOOP:
        CPI16_01RHL2KI_R16PP ZH,ZL,ADR_FIFO
        BREQ  JP_FIFO_INIT_META
        ST    -Z,RHT1
        ;SBIW  XH:XL,1
        ;SBIW  ZH:ZL,1
        RJMP  JP_FIFO_INIT_CLR_FIFO_LOOP
  JP_FIFO_INIT_META: ; write FIFO_SIZE, set NUL
        ;SBIW  ZH:ZL,1+FIFO_META_SIZE
        ;SUB16 ZH,ZL,R_FIFO_SIZEH,R_FIFO_SIZEL
        STD16_0P1DL23RHL Z,AD_FIFO_SIZEL,R_FIFO_SIZEH,R_FIFO_SIZEL
        RAMBSET_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_INIT_POP:
        POPSREG_0RT RHT1
        POP         RHT1 
        ;POPP_0P Z
  JP_FIFO_INIT_EXIT:
        RET
;-------------------------------------------------------------------------------
SR_FIFO_INIT_FILL0_0RSIZE_1ADR_FIFO_2PZ: 
; in PZ, SIZEHL, SRAM_ADR, via X_CNT, RHT1, SREG_Z
  JP_FIFO_INIT_FILL0_PUSH:
        ;PUSHP_0P  Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
  JP_FIFO_INIT_FILL0_MOV_PZ_2_FIFO_END:
        ;PLD_0A1P ADR_FIFO,Z
        ;MOVW  XH:XL,R_FIFO_SIZEH:R_FIFO_SIZEL
        ;ADDI16  XH,XL,FIFO_META_SIZE
        CLC
        ADDI16  ZH,ZL,(FIFO_META_SIZE-1)
        ADD16   ZH,ZL,R_FIFO_SIZEH,R_FIFO_SIZEL
        LDI     RHT1,0
        ST      Z,RHT1
        CLZ
  JP_FIFO_INIT_FILL0_CLR_FIFO_LOOP:
        CPI16_01RHL2KI_R16PP ZH,ZL,ADR_FIFO
        BREQ  JP_FIFO_INIT_FILL0_META
        ST    -Z,RHT1
        ;SBIW  XH:XL,1
        ;SBIW  ZH:ZL,1
        RJMP  JP_FIFO_INIT_FILL0_CLR_FIFO_LOOP
  JP_FIFO_INIT_FILL0_META: ; write FIFO_SIZE, set NUL
        ;SBIW  ZH:ZL,1+FIFO_META_SIZE
        ;SUB16 ZH,ZL,R_FIFO_SIZEH,R_FIFO_SIZEL
        STD16_0P1DL23RHL Z,AD_FIFO_SIZEL,R_FIFO_SIZEH,R_FIFO_SIZEL
        RAMBSET_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_INIT_FILL0_POP:
        POPSREG_0RT RHT1
        POP         RHT1 
        ;POPP_0P Z
  JP_FIFO_INIT_FILL0_EXIT:
        RET
;-------------------------------------------------------------------------------
SR_FIFO_WR_CHK_FIFO_FUL_SET_T: ; in PZ, SRAM_ADR, out SREG_T, via  RHT1
        ;PLD_0A1P  ADR_FIFO,Z          ; load FIFO ADR value into Z
        LDD   RHT1,Z+AD_FIFO_SREG
        CLT                           ; clr SREG_T
        SBRC  RHT1,FLG_FIFO_SREG_FUL
        SET                           ; if ful, set SREG_T
        RET
SR_FIFO_RD_CHK_FIFO_NUL_SET_T:
        LDD   RHT1,Z+AD_FIFO_SREG
        CLT
        SBRC  RHT1,FLG_FIFO_SREG_NUL
        SET
        RET
;-------------------------------------------------------------------------------
SR_FIFO_WR_R_FIFO_BUF0_2_FIFO_BUF0:
        STD   Z+AD_FIFO_BUF0,R_FIFO_BUF0
        RET
SR_FIFO_RD_FIFO_BUF0_2_R_FIFO_BUF0:
        LDD   R_FIFO_BUF0,Z+AD_FIFO_BUF0
        RET
;-------------------------------------------------------------------------------
;SR_FIFO_WR_R17_Rh16T_PZ_MAX256:
SR_FIFO_WR_0RBUF_1ADR_FIFO_2PZ: ; in PZ, SRAM_ADR, via X/Y, RHT1, SREG_Z
; assume no full
  JP_FIFO_WR_PUSH_ENTRY:
        PUSHP_0P      X
        PUSHP_0P      Y
        ;PUSHP_0P      Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
        ;PLD_0A1P ADR_FIFO,Z
   JP_FIFO_WR_INC_LEN:
        LDD16_0P1DL2P Z,AD_FIFO_LENL,X
        ADDI16 XH,XL,1
        STD16_0P1DL2P Z,AD_FIFO_LENL,X
  JP_FIFO_WR_CHK_LEN_FUL:
        LDD16_0P1DL2P Z,AD_FIFO_SIZEL,Y
        CLZ
        CP16_01RHL23RHL XH,XL,YH,YL       ; FIFO_LEN ?=FIFO_SIZE
        BRNE  JP_FIFO_WR_WRITE_DATA       ; not ful
  JP_FIFO_WR_SET_FLG_FIFO_SREG_FUL:       ; full, set flg
        RAMBSET_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
  JP_FIFO_WR_WRITE_DATA:
  JP_FIFO_WR_CLR_FLG_FIFO_SREG_NUL:
        LDD   RHT1,Z+AD_FIFO_SREG
        SBRC  RHT1,FLG_FIFO_SREG_NUL
        RAMBCLR_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_WR_LOAD_WRIN_PTR:
        LDD16_0P1DL2P Z,AD_FIFO_WRINPL,X  ; load WRINP into 
        ;LDD   RHT1,Z+AD_FIFO_BUF0
        ADD16 ZH,ZL,XH,XL                       ; move PZ to PZ+PX
        ;STD   Z+AD_FIFO_DATA,RHT1       ; write buf into FIFO
        STD   Z+AD_FIFO_DATA,R_FIFO_BUF0       ; write buf into FIFO
        SUB16 ZH,ZL,XH,XL                       ; recover PZ
  JP_FIFO_WR_CHK_WRIN_PTR_END:
        SBIW  YH:YL,1                     ; FIFO_SIZE-1 = PTR_END
        CLZ
        CP16_01RHL23RHL XH,XL,YH,YL       ; WRP ?= FIFO_SIZE-1
        BRNE  JP_FIFO_WR_INC_WRIN_PTR  ; if WRIN_P point to last Byte, next OV
  JP_FIFO_WR_RESET_WRIN_PTR_0:                   ; rollback ptr to 0
        CLR   XH
        CLR   XL
        RJMP  JP_FIFO_WR_UPDATE_WRIN_PTR
  JP_FIFO_WR_INC_WRIN_PTR:
        ADDI16 XH,XL,1
  JP_FIFO_WR_UPDATE_WRIN_PTR:
        STD16_0P1DL2P Z,AD_FIFO_WRINPL,X
  JP_FIFO_WR_POP_EXIT:
        POPSREG_0RT RHT1 
        POP         RHT1 
        ;POPP_0P     Z
        POPP_0P     Y
        POPP_0P     X
  JP_FIFO_WR_ERR_FUL_EXIT:
        RET
;-------------------------------------------------------------------------------
;SR_FIFO_RD_R17_Rh16T_PZ_MAX256:
SR_FIFO_RD_0RBUF_1ADR_FIFO_2PZ:
; assume no null
  JP_FIFO_RD_PUSH_ENTRY:
        PUSHP_0P      X
        PUSHP_0P      Y
        ;PUSHP_0P      Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
        ;PLD_0A1P ADR_FIFO,Z
  JP_FIFO_RD_DEC_LEN:
        LDD16_0P1DL2P Z,AD_FIFO_LENL,X
        SBIW  XH:XL,1
        STD16_0P1DL2P Z,AD_FIFO_LENL,X
  JP_FIFO_RD_CHK_LEN_NUL:
        CLZ
        TST   XL
        BRNE  JP_FIFO_RD_READ_DATA
        CLZ
        TST   XH
        BRNE  JP_FIFO_RD_READ_DATA
  JP_FIFO_RD_SET_FLG_FIFO_SREG_NUL:
        RAMBSET_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_RD_READ_DATA:
  JP_FIFO_RD_CLR_FLG_FIFO_SREG_FUL:
        LDD   RHT1,Z+AD_FIFO_SREG
        SBRC  RHT1,FLG_FIFO_SREG_FUL
        RAMBCLR_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
  JP_FIFO_RD_LOAD_RDOUT_PTR:
        LDD16_0P1DL2P Z,AD_FIFO_RDOUTPL,X
        ADD16 ZH,ZL,XH,XL                       ; move PZ to PZ+PX
        LDD   RHT1,Z+AD_FIFO_DATA       ; copy byte into RTH1
        STD   Z+AD_FIFO_BUF0,RHT1
        SUB16 ZH,ZL,XH,XL                       ; recover PZ
  JP_FIFO_RD_CHK_RDOUT_PTR_END:
        LDD16_0P1DL2P Z,AD_FIFO_SIZEL,Y
        SBIW  YH:YL,1                     ; FIFO_SIZE-1 = PTR_END
        CLZ
        CP16_01RHL23RHL XH,XL,YH,YL       ; WRP ?= FIFO_SIZE-1
        BRNE  JP_FIFO_RD_INC_ROUT_PTR
  JP_FIFO_RD_RESET_RDOUT_PTR_0:
        CLR   XH
        CLR   XL
        RJMP JP_FIFO_RD_UPDATE_RDOUT_PTR
  JP_FIFO_RD_INC_ROUT_PTR:
        ADDI16 XH,XL,1
  JP_FIFO_RD_UPDATE_RDOUT_PTR:
        STD16_0P1DL2P Z,AD_FIFO_WRINPL,X
  JP_FIFO_RD_POP_EXIT:
        POPSREG_0RT RHT1 
        POP         RHT1 
        ;POPP_0P     Z
        POPP_0P     Y
        POPP_0P     X
  JP_FIFO_RD_ERR_NUL_EXIT:
        RET
;*******************************************************************************
;-------------------------------------------------------------------------------
; PASS, write 256 x [255,1]+[255] into buffer and read out
; 1 x RX [260]
; 1 x RX_read_out [256]
; 1 x TX [260]
; 1 x TX_read_out [256]
; test SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;JP_RX_WR:
;        LDI R17, 255
;JP_RX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;        DEC R17
;        BRNE JP_RX_WR_LOOP
;        LDI R17, $FF
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;        RJMP JP_RX_RD
;-------------------------------------------------------------------------------
;JP_RX_RD:
;        LDI R18, 255
;        PLD_0A1P A_FIFO_BASE_RX_TST,Y
;JP_RX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX256
;        ST Y+,R17
;        DEC R18
;        BRNE JP_RX_RD_LOOP
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX256
;        ST Y+,R17
;        RJMP JP_TX_WR
;-------------------------------------------------------------------------------
;JP_TX_WR:
;        LDI R17, 255
;JP_TX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;        DEC R17
;        BRNE JP_TX_WR_LOOP
;        LDI R17, $FF
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;        RJMP JP_TX_RD
;-------------------------------------------------------------------------------
;JP_TX_RD:
;        LDI R19, 255
;        PLD_0A1P A_FIFO_BASE_TX_TST,Y
;JP_TX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX256
;        ST Y+,R17
;        DEC R19
;        BRNE JP_TX_RD_LOOP
;        PLD_0A1P A_FIFO_BASE_TX,Z
;        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX256
;        ST Y+,R17
;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------
; PASS, write 256 x [255,1]+[255] into buffer and read out
; write 256 and read 128 out, then write another 128 and read 128 out
; check if the in/out ptr in correct location
; 1 x RX [260]
; 1 x RX_read_out [256]
; 1 x TX [260]
; 1 x TX_read_out [256]
; test SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;JP_RX_WR:
;        LDI R17, 255
;JP_RX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;        DEC R17
;        BRNE JP_RX_WR_LOOP
;        ; write 256
;        LDI R17, $FF
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;JP_RX_RD:
;        LDI R18, 128
;        PLD_0A1P A_FIFO_BASE_RX_TST,Y
;JP_RX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX256
;        ST Y+,R17
;        DEC R18
;        BRNE JP_RX_RD_LOOP
;
;JP2_RX_WR:
;        LDI R17, 128
;JP2_RX_WR_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX256
;        DEC R17
;        BRNE JP2_RX_WR_LOOP
;JP2_RX_RD:
;        LDI R18, 128
;        ;PLD_0A1P A_FIFO_BASE_RX_TST,Y
;JP2_RX_RD_LOOP:
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX256
;        ST Y+,R17
;        DEC R18
;        BRNE JP2_RX_RD_LOOP
;-------------------------------------------------------------------------------
;END:
;        RJMP END
;-------------------------------------------------------------------------------

