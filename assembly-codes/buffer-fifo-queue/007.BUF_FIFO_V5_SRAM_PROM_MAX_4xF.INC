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
.EQU  K16_FIFO_ADR    = SRAM_START
;.EQU  ADR_FIFO_RAOM_META   = SRAM_START + K16_FIFO_META_SIZE + FIFO_SIZE
.EQU  K16_FIFO_META_SIZE  = 11
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
;*******************************************************************************
;.DEF  RHT1            = R16
;.DEF  RHT2            = R17
;*******************************************************************************
;.DEF  XL    = R26  ; R_FIFO_SIZEL
;.DEF  XH    = R27  ; R_FIFO_SIZEH
;.EQU  R_FIFO_LENL     = R2
;.EQU  R_FIFO_LENH     = R3
;.EQU  R_FIFO_WRINPL   = R4
;.EQU  R_FIFO_WRINPH   = R5
;.EQU  R_FIFO_RDOUTPL  = R6
;.EQU  R_FIFO_RDOUTPH  = R7
;.DEF  RBUF0           = R24
;.EQU  R_FIFO_BUFW     = R9
;.EQU  R_FIFO_SREG     = R10

;.EQU  RHT3            = R18
;.EQU  RHT4            = R19
;.ORG $0
;      RJMP JP_RX_WR
      ;RJMP JP_TX_WR
;*******************************************************************************
SR_FIFO_INIT_0PX_SIZE_1PY_ADR_FIFO: 
; in PZ, SIZEHL, SRAM_ADR, via RHT1, SREG_Z
  JP_FIFO_INIT_PUSH:        ; Z -> Y, Y->Z
        PUSHP_0P      Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
  JP_FIFO_INIT_MOV_PZ_2_FIFO_META_END:
        ;PLD_0A1P K16_FIFO_ADR,Z
        ;MOVW  XH:XL,XH:XL
        ;ADDI16  XH,XL,K16_FIFO_META_SIZE
        CLC
        MOVW    ZH:ZL,YH:YL   ; save FIFO RAM START ADR to Y
        ADDI16  YH,YL,(K16_FIFO_META_SIZE-1)
        ;ADD16   ZH,ZL,XH,XL
        LDI     RHT1,0
        ST      Y,RHT1                      ; clr last meta byte
        CLZ
  JP_FIFO_INIT_CLR_FIFO_LOOP:               ; clr meta bytes
        ;CPI16_01RHL2KI_R16PP ZH,ZL,K16_FIFO_ADR  ; Z point to FIFO start?
        CP16_01RHL23RHL YH,YL,ZH,ZL           ; Z point to FIFO start?
        BREQ  JP_FIFO_INIT_META             ; if back to start, go init meta
        ST    -Y,RHT1
        ;SBIW  XH:XL,1
        ;SBIW  ZH:ZL,1
        RJMP  JP_FIFO_INIT_CLR_FIFO_LOOP
  JP_FIFO_INIT_META: ; write FIFO_SIZE, set NUL
        ;SBIW  ZH:ZL,1+K16_FIFO_META_SIZE
        ;SUB16 ZH,ZL,XH,XL
        STD16_0P1DL23RHL      Y,AD_FIFO_SIZEL,XH,XL
        RAMBSET_0P1D2BIT3RHT  Y,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_INIT_POP:
        POPSREG_0RT RHT1
        POP         RHT1 
        POPP_0P     Z
  JP_FIFO_INIT_EXIT:
        RET
;-------------------------------------------------------------------------------
SR_FIFO_INIT_FILL0_0PX_SIZE_1PY_ADR_FIFO: 
; in PZ, SIZEHL, SRAM_ADR, via X_CNT, RHT1, SREG_Z
  JP_FIFO_INIT_FILL0_PUSH:
        PUSHP_0P      Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
  JP_FIFO_INIT_FILL0_MOV_PZ_2_FIFO_END:
        ;PLD_0A1P K16_FIFO_ADR,Z
        ;MOVW  XH:XL,XH:XL
        ;ADDI16  XH,XL,K16_FIFO_META_SIZE
        CLC
        MOVW    ZH:ZL,YH:YL   ; save FIFO START RAM ADR to Y
        ADDI16  YH,YL,(K16_FIFO_META_SIZE-1)
        ADD16   YH,YL,XH,XL
        LDI     RHT1,0
        ST      Y,RHT1
        CLZ
  JP_FIFO_INIT_FILL0_CLR_FIFO_LOOP:
        ;CPI16_01RHL2KI_R16PP ZH,ZL,K16_FIFO_ADR
        CP16_01RHL23RHL YH,YL,ZH,ZL           ; Z point to FIFO start?
        BREQ  JP_FIFO_INIT_FILL0_META
        ST    -Y,RHT1
        ;SBIW  XH:XL,1
        ;SBIW  ZH:ZL,1
        RJMP  JP_FIFO_INIT_FILL0_CLR_FIFO_LOOP
  JP_FIFO_INIT_FILL0_META: ; write FIFO_SIZE, set NUL
        ;SBIW  ZH:ZL,1+K16_FIFO_META_SIZE
        ;SUB16 ZH,ZL,XH,XL
        STD16_0P1DL23RHL      Y,AD_FIFO_SIZEL,XH,XL
        RAMBSET_0P1D2BIT3RHT  Y,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_INIT_FILL0_POP:
        POPSREG_0RT RHT1
        POP         RHT1 
        POPP_0P     Z
  JP_FIFO_INIT_FILL0_EXIT:
        RET
;-------------------------------------------------------------------------------
SR_FIFO_WR_CHK_FIFO_FUL_SET_T: ; in PZ, SRAM_ADR, out SREG_T, via  RHT1
        ;PLD_0A1P  K16_FIFO_ADR,Z          ; load FIFO ADR value into Z
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
;        STD   Z+AD_FIFO_BUF0,RBUF0
        RET
SR_FIFO_RD_FIFO_BUF0_2_R_FIFO_BUF0:
;        LDD   RBUF0,Z+AD_FIFO_BUF0
        RET
;-------------------------------------------------------------------------------
;SR_FIFO_WR_R17_Rh16T_PZ_MAX256:
SR_FIFO_WR_0BUF_1PY_ADR_FIFO: ; in PY, SRAM_ADR, via X/Z, RHT1, SREG_Z
; assume no full
        ;PLD_0A1P K16_FIFO_ADR,Y
  JP_FIFO_WR_PUSH_ENTRY:
        PUSHP_0P      Z
        PUSHP_0P      X
        ;PUSHP_0P      Y
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
   JP_FIFO_WR_INC_LEN:
        LDD16_0P1DL2P Y,AD_FIFO_LENL,Z  ; read LEN into Z
        ADDI16 ZH,ZL,1                  ; LEN+1
        STD16_0P1DL2P Y,AD_FIFO_LENL,Z  ; save LEN
  JP_FIFO_WR_CHK_LEN_FUL:
        LDD16_0P1DL2P Y,AD_FIFO_SIZEL,X ; read SIZE into X
        CLZ
        CP16_01RHL23RHL ZH,ZL,XH,XL       ; LEN ?=SIZE
        BRNE  JP_FIFO_WR_WRITE_DATA       ; not ful
  JP_FIFO_WR_SET_FLG_FIFO_SREG_FUL:       ; full, set flg
        RAMBSET_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
  JP_FIFO_WR_WRITE_DATA:
  JP_FIFO_WR_CLR_FLG_FIFO_SREG_NUL:
        LDD   RHT1,Y+AD_FIFO_SREG
        SBRC  RHT1,FLG_FIFO_SREG_NUL
        RAMBCLR_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_WR_LOAD_WRIN_PTR:
        LDD   RHT1,Y+AD_FIFO_BUF0         ; ld buf
        LDD16_0P1DL2P Y,AD_FIFO_WRINPL,Z  ; ld WRINP into Z
        ADD16 YH,YL,ZH,ZL                 ; mv PY to PY+PZ
        STD   Y+AD_FIFO_DATA,RHT1         ; wr buf into FIFO
        ;STD   Y+AD_FIFO_DATA,RBUF0 ;========> write buf into FIFO
        SUB16 YH,YL,ZH,ZL                 ; recover PY to SRAM start ADR
  JP_FIFO_WR_CHK_WRIN_PTR_END:
        SBIW  XH:XL,1                     ; X-1 = SIZE-1 = PTR_END
        CLZ
        CP16_01RHL23RHL ZH,ZL,XH,XL       ; WRP ?= FIFO_SIZE-1
        BRNE  JP_FIFO_WR_INC_WRIN_PTR  ; if WRIN_P point to last Byte, next OV
  JP_FIFO_WR_RESET_WRIN_PTR_0:            ; rollback ptr to 0
        CLR   ZH
        CLR   ZL
        RJMP  JP_FIFO_WR_UPDATE_WRIN_PTR
  JP_FIFO_WR_INC_WRIN_PTR:
        ADDI16 ZH,ZL,1
  JP_FIFO_WR_UPDATE_WRIN_PTR:
        STD16_0P1DL2P Y,AD_FIFO_WRINPL,Z
  JP_FIFO_WR_POP_EXIT:
        POPSREG_0RT RHT1 
        POP         RHT1 
        ;POPP_0P     Y
        POPP_0P     X
        POPP_0P     Z
  JP_FIFO_WR_ERR_FUL_EXIT:
        RET
;-------------------------------------------------------------------------------
;SR_FIFO_RD_R17_Rh16T_PZ_MAX256:
SR_FIFO_RD_0BUF_1PY_ADR_FIFO:
; assume no null
        ;PLD_0A1P K16_FIFO_ADR,Y
  JP_FIFO_RD_PUSH_ENTRY:
        PUSHP_0P      Z
        PUSHP_0P      X
        ;PUSHP_0P      Y
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
  JP_FIFO_RD_DEC_LEN:
        LDD16_0P1DL2P Y,AD_FIFO_LENL,Z
        SBIW  ZH:ZL,1
        STD16_0P1DL2P Y,AD_FIFO_LENL,Z
  JP_FIFO_RD_CHK_LEN_NUL:
        CLZ
        TST   ZL
        BRNE  JP_FIFO_RD_READ_DATA
        CLZ
        TST   ZH
        BRNE  JP_FIFO_RD_READ_DATA
  JP_FIFO_RD_SET_FLG_FIFO_SREG_NUL:
        RAMBSET_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_RD_READ_DATA:
  JP_FIFO_RD_CLR_FLG_FIFO_SREG_FUL:
        LDD   RHT1,Y+AD_FIFO_SREG
        SBRC  RHT1,FLG_FIFO_SREG_FUL
        RAMBCLR_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
  JP_FIFO_RD_LOAD_RDOUT_PTR:
        LDD16_0P1DL2P Y,AD_FIFO_RDOUTPL,Z   ; ld RDOUT_PTR
        ADD16 YH,YL,ZH,ZL                   ; mv PY to PY+PZ
        LDD   RHT1,Y+AD_FIFO_DATA ;=================>; ld byte into RBUF0
        SUB16 YH,YL,ZH,ZL         ; recover PY to SRAM start ADR
        STD   Y+AD_FIFO_BUF0,RHT1 ; write to BUF0
        ;LDD   RBUF0,Y+AD_FIFO_DATA ;=================>; load byte into RBUF0
  JP_FIFO_RD_CHK_RDOUT_PTR_END:
        LDD16_0P1DL2P Y,AD_FIFO_SIZEL,X
        SBIW  XH:XL,1                     ; FIFO_SIZE-1 = PTR_END
        CLZ
        CP16_01RHL23RHL ZH,ZL,XH,XL       ; RDP ?= FIFO_SIZE-1
        BRNE  JP_FIFO_RD_INC_ROUT_PTR
  JP_FIFO_RD_RESET_RDOUT_PTR_0:
        CLR   ZH
        CLR   ZL
        RJMP JP_FIFO_RD_UPDATE_RDOUT_PTR
  JP_FIFO_RD_INC_ROUT_PTR:
        ADDI16 ZH,ZL,1
  JP_FIFO_RD_UPDATE_RDOUT_PTR:
        STD16_0P1DL2P Y,AD_FIFO_RDOUTPL,Z
  JP_FIFO_RD_POP_EXIT:
        POPSREG_0RT RHT1 
        POP         RHT1 
        ;POPP_0P     Y
        POPP_0P     X
        POPP_0P     Z
  JP_FIFO_RD_ERR_NUL_EXIT:
        RET
;*******************************************************************************
; FIFO META in SRAM, data in PROM
;*******************************************************************************
SR_FIFO_RAOM_INIT_0PY_ADR_FIFO_SRAM_1PZ_ADR_FIFO_PROM: ; Y keep, Z+2
; in PZ, SIZEHL, SRAM_ADR, via Y, RHT1, SREG_Z
  JP_FIFO_RAOM_INIT_PUSH:
        ;PUSHP_0P  Y
        ;PUSHP_0P      X
        PUSH          RHT1
        ;PUSHSREG_0RT  RHT1
  RJMP  JP_FIFO_RAOM_INIT_META ; RAOM not need clr SRAM
  JP_FIFO_RAOM_INIT_MOV_PY_2_FIFO_META_END:
        ;PLD_0A1P ADR_FIFO_RAOM_META,Y
        CLC
        MOVW    XH:XL,YH:YL ; save FIFO RAM START ADR to X
        ADDI16  YH,YL,(K16_FIFO_META_SIZE-1)
        ;ADD16   YH,YL,XH,XL
        LDI     RHT1,0
        ST      Y,RHT1                      ; clr last meta byte
        CLZ
  JP_FIFO_RAOM_INIT_CLR_FIFO_LOOP:               ; clr meta bytes
        ;CPI16_01RHL2KI_R16PP YH,YL,K16_FIFO_ADR ; Y point to FIFO start?
        CP16_01RHL23RHL YH,YL,XH,XL           ; Y point to FIFO start?
        BREQ  JP_FIFO_RAOM_INIT_META        ; if back to start, go init meta
        ST    -Y,RHT1
        RJMP  JP_FIFO_RAOM_INIT_CLR_FIFO_LOOP
  JP_FIFO_RAOM_INIT_META: ; write FIFO_SIZE, set NUL
        ;SUB16 ZH,ZL,XH,XL
        ;STD16_0P1DL23RHL Z,AD_FIFO_SIZEL,XH,XL
        ;RAMBSET_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
        ;PLD_0A1P (ADR_FIFO_PROM<<1),Z
        LPM   RHT1,Z+               ; load FIFO_PROM_SIZEL
        STD   Y+AD_FIFO_SIZEL,RHT1  ; save size
        STD   Y+AD_FIFO_LENL,RHT1   ; save len
        LPM   RHT1,Z+               ; load FIFO_PROM_SIZEH
        STD   Y+AD_FIFO_SIZEH,RHT1
        STD   Y+AD_FIFO_LENH,RHT1
        STD   Y+AD_FIFO_RDOUTPH,ZH ; now the PZ-> 1st byte
        STD   Y+AD_FIFO_RDOUTPL,ZL ; PZ = RD_OUT_PTR
  JP_FIFO_RAOM_INIT_CLR_FLG_FIFO_SREG_NUL:
        ;RAMBCLR_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_RAOM_INIT_SET_FLG_FIFO_SREG_FUL:       ; full, set flg
        ;RAMBSET_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
        LDI   RHT1,(1<<FLG_FIFO_SREG_FUL) ; quickest way to reset sreg
        STD   Y+AD_FIFO_SREG,RHT1
  JP_FIFO_RAOM_INIT_POP:
        ;POPSREG_0RT RHT1
        POP         RHT1 
        ;POPP_0P     X
        ;POPP_0P Y
  JP_FIFO_RAOM_INIT_EXIT:
        RET
;-------------------------------------------------------------------------------
; NOT READY NOW, because need use SPM
;SR_FIFO_RAOM_WR_R17_Rh16T_PZ_MAX256:
SR_FIFO_RAOM_WR_0BUF_1PZ_ADR_FIFO_SRAM:; in PZ, SRAM_ADR, via X/Y, RHT1, SREG_Z
; assume no full
  JP_FIFO_RAOM_WR_PUSH_ENTRY:
        PUSHP_0P      X
        PUSHP_0P      Y
        ;PUSHP_0P      Z
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
        ;PLD_0A1P K16_FIFO_ADR,Z
   JP_FIFO_RAOM_WR_INC_LEN:
        LDD16_0P1DL2P Z,AD_FIFO_LENL,X
        ADDI16 XH,XL,1
        STD16_0P1DL2P Z,AD_FIFO_LENL,X
  JP_FIFO_RAOM_WR_CHK_LEN_FUL:
        LDD16_0P1DL2P Z,AD_FIFO_SIZEL,Y
        CLZ
        CP16_01RHL23RHL XH,XL,YH,YL       ; FIFO_LEN ?=FIFO_SIZE
        BRNE  JP_FIFO_RAOM_WR_WRITE_DATA       ; not ful
  JP_FIFO_RAOM_WR_SET_FLG_FIFO_SREG_FUL:       ; full, set flg
        RAMBSET_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
  JP_FIFO_RAOM_WR_WRITE_DATA:
  JP_FIFO_RAOM_WR_CLR_FLG_FIFO_SREG_NUL:
        LDD   RHT1,Z+AD_FIFO_SREG
        SBRC  RHT1,FLG_FIFO_SREG_NUL
        RAMBCLR_0P1D2BIT3RHT Z,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_RAOM_WR_LOAD_WRIN_PTR:
        LDD16_0P1DL2P Z,AD_FIFO_WRINPL,X  ; load WRINP into 
        ;LDD   RHT1,Z+AD_FIFO_BUF0
        ADD16 ZH,ZL,XH,XL                       ; move PZ to PZ+PX
        ;STD   Z+AD_FIFO_DATA,RHT1       ; write buf into FIFO
        ;STD   Z+AD_FIFO_DATA,RBUF0       ; write buf into FIFO
        ;SPM ----> need update SPM
        SUB16 ZH,ZL,XH,XL                       ; recover PZ
  JP_FIFO_RAOM_WR_CHK_WRIN_PTR_END:
        SBIW  YH:YL,1                     ; FIFO_SIZE-1 = PTR_END
        CLZ
        CP16_01RHL23RHL XH,XL,YH,YL       ; WRP ?= FIFO_SIZE-1
        BRNE  JP_FIFO_RAOM_WR_INC_WRIN_PTR  ; if WRIN_P point to last Byte, next OV
  JP_FIFO_RAOM_WR_RESET_WRIN_PTR_0:                   ; rollback ptr to 0
        CLR   XH
        CLR   XL
        RJMP  JP_FIFO_RAOM_WR_UPDATE_WRIN_PTR
  JP_FIFO_RAOM_WR_INC_WRIN_PTR:
        ADDI16 XH,XL,1
  JP_FIFO_RAOM_WR_UPDATE_WRIN_PTR:
        STD16_0P1DL2P Z,AD_FIFO_WRINPL,X
  JP_FIFO_RAOM_WR_POP_EXIT:
        POPSREG_0RT RHT1 
        POP         RHT1 
        ;POPP_0P     Z
        POPP_0P     Y
        POPP_0P     X
  JP_FIFO_RAOM_WR_ERR_FUL_EXIT:
        RET
;-------------------------------------------------------------------------------
;SR_FIFO_RAOM_RD_R17_Rh16T_PZ_MAX256:
SR_FIFO_RAOM_RD_0BUF_1PY_ADR_FIFO_SRAM: ;IN Y,OUT RBUF0, (PZX RHT1)
;        PLD_0A1P ADR_FIFO_RAOM_META,Y
;        PLD_0A1P (ADR_FIFO_PROM<<1),Z
; assume no null
  JP_FIFO_RAOM_RD_PUSH_ENTRY: ; Z->Y, Y->X, X->Z, 
        PUSHP_0P      Z
        PUSHP_0P      X
        ;PUSHP_0P      Y
        PUSH          RHT1
        PUSHSREG_0RT  RHT1
        ;PLD_0A1P ADR_FIFO_RAOM_META,Y
  JP_FIFO_RAOM_RD_DEC_LEN:
        LDD16_0P1DL2P Y,AD_FIFO_LENL,Z
        SBIW  ZH:ZL,1
        STD16_0P1DL2P Y,AD_FIFO_LENL,Z
  JP_FIFO_RAOM_RD_CHK_LEN_NUL:
        CLZ
        TST   ZL
        BRNE  JP_FIFO_RAOM_RD_READ_DATA
        CLZ
        TST   ZH
        BRNE  JP_FIFO_RAOM_RD_READ_DATA
  JP_FIFO_RAOM_RD_SET_FLG_FIFO_SREG_NUL:
        RAMBSET_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_NUL,RHT1
  JP_FIFO_RAOM_RD_READ_DATA:
  JP_FIFO_RAOM_RD_CLR_FLG_FIFO_SREG_FUL:
        LDD   RHT1,Y+AD_FIFO_SREG
        SBRC  RHT1,FLG_FIFO_SREG_FUL
        RAMBCLR_0P1D2BIT3RHT Y,AD_FIFO_SREG,FLG_FIFO_SREG_FUL,RHT1
  JP_FIFO_RAOM_RD_LOAD_RDOUT_PTR:
        LDD16_0P1DL2P Y,AD_FIFO_RDOUTPL,Z
        ;ADD16 YH,YL,ZH,ZL                       ; move PY to PY+PZ
        ;LDD   RHT1,Z+AD_FIFO_DATA        ; copy byte into RTH1
        ;STD   Y+AD_FIFO_BUF0,RBUF0
        ;LPM   RBUF0,Z ;=======================>; copy byte into RBUF0
        LPM   RHT1,Z ;=======================>; copy byte into RBUF0
        STD   Y+AD_FIFO_BUF0,RHT1
        ;SUB16 YH,YL,ZH,ZL                       ; recover PZ
  JP_FIFO_RAOM_RD_CHK_RDOUT_PTR_END:
        LDD16_0P1DL2P Y,AD_FIFO_SIZEL,X   ; X = SIZE
        ADDI16  XH,XL,2 ; FIFO END = ADR_FIFO_RAOM_META + SIZE + 2
        ADD16   XH,XL,YH,YL
        CLZ
        CP16_01RHL23RHL ZH,ZL,XH,XL       ; RDP ?= FIFO_SIZE-1
        BRNE  JP_FIFO_RAOM_RD_INC_ROUT_PTR
  JP_FIFO_RAOM_RD_RESET_RDOUT_PTR_0:
        ;PLD_0A1P (ADR_FIFO_RAOM_META+2),Z
        MOVW    ZH:ZL,YH:YL
        ADDI16  ZH,ZL,2
        RJMP JP_FIFO_RAOM_RD_UPDATE_RDOUT_PTR
  JP_FIFO_RAOM_RD_INC_ROUT_PTR:
        ADDI16 ZH,ZL,1
  JP_FIFO_RAOM_RD_UPDATE_RDOUT_PTR:
        STD16_0P1DL2P Y,AD_FIFO_RDOUTPL,Z
  JP_FIFO_RAOM_RD_POP_EXIT:
        POPSREG_0RT RHT1 
        POP         RHT1 
        ;POPP_0P     Y
        POPP_0P     X
        POPP_0P     Z
  JP_FIFO_RAOM_RD_ERR_NUL_EXIT:
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

