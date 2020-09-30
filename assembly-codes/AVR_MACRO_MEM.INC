;.INCLUDE "M328PDEF.INC"
;.INCLUDE "AVR_MACRO.INC"
; This code only implement allocating, can free but cannot handle fragment
.MACRO INIT_MALC8  ; @0=ADR_START, [@1=CAP=8]
      ;Memory Allocation 8-bit data width, max 32 x Bytes
.DEF  REGTMP1 = R16
.DEF  REGTMP2 = R17
.EQU  MALC8_RAM_AS        = @0                  ; RAM ADR_START
.EQU  MALC8_CAP       = 8                  ; RAM capacity max = 8
.EQU  MALC8_A         = MALC8_RAM_AS             ; RAM ADR 
.EQU  MALC8_RAM_AE        =  MALC8_RAM_AS + MALC8_CAP; ? not OK now
.EQU  MALC8_SREG_NULL = 7
.EQU  MALC8_SREG_FULL = 6
.EQU  MALC8_SREG_REQ_OK = 6   ; FULL mean fail REQ=1
.EQU  MALC8_SREG_NUM  = 0  ; Status REG 6=FULL, 7=NULL + 0:1:2=ALC_NUM
.EQU  MALC8_SREG_ID   = 1  ; 8 x ID ALCed or not [0000-0000]
.EQU  MALC8_REQ_ID    = 2
.EQU  MALC8_REQ_AH    = 3
.EQU  MALC8_REQ_AL    = 4
.EQU  MALC8_REQ_CAP   = 5
.EQU  MALC8_ID_META_AS    = 6
        ; AID1_META=[A+BASE+0,+2], AID2=[A+BASE+3,+2]... AID_N[0,7]=[A+BASE+ID_N*3,+2]
        ; init 0 for all ram
.EQU  MALC8_ID_META_RAM_AS = MALC8_A + MALC8_ID_META_AS
.EQU  MALC8_ID_META_RAM_AE = MALC8_ID_META_RAM_AS + 3*7
.EQU  MALC8_BUF_RAM_AS = MALC8_ID_META_RAM_AE + 1
        RAMCLRPZAAR MALC8_RAM_AS,MALC8_RAM_AE,REGTMP1
SR_MAL_BUF_CHK_NFULL:
        RAMRDPZRAD REGTMP1,MALC8_A,MALC8_SREG_ID
        BREQ JP_MAL_SET_NULL ; FULL, SREG_ID=$00, Z=0
        COM     REGTMP1
        BREQ JP_MAL_SET_FULL ; FULL, SREG_ID=$FF, COM R0 = $00, Z=0
JP_MAL_SET_NULL:
        RAMBSETPZADBR MALC8_A,MALC8_SREG_NULL
        RET
JP_MAL_SET_FULL:
        RAMBSETPZADBR MALC8_A,MALC8_SREG_FULL
        RET
SR_MAL_BUF:
        ; check if full
        ; skip next (do mal) if full bit set and branch to error
        SKRAMBSPZADB MALC8_A,MALC8_SREG_NUM,MALC8_SREG_FULL
        RJMP JP_MAL_NEW
        RJMP JP_MAL_FULL_ERR
JP_MAL_FULL_ERR:  ; do nothing because FULL=REQ_OK=1, REQ failed 
        RET
JP_MAL_NEW: 
    ; client send REQ_CAP and call SR_MAL_BUF, return REQ_OK, ID, ADR, CAP
        ; do alloc new buff
        ; exec this mean not FULL and REQ_OK=FULL=0
        ; lookup first empty ID slot for adding
        RAMRDPZRAD REGTMP1,MALC8_A,MALC8_SREG_ID  ; copy SREG_ID = RT1
        GetGPR1stLSBCLRID REGTMP1,REGTMP2  ; find SREG_ID 0 bit and convert to ID_N[0,7]=REGTMP2
        BSETPZADVR MALC8_A,MALC8_SREG_ID,REGTMP2,REGTMP1  ; set found SREG_ID bit from 0 to 1
        RAMWRPZADR MALC8_A,MALC8_REQ_ID,REGTMP2 ; return/copy ID_N=REQ_ID
        ; REQ_AH = HIGH(MALC8_ID_META_RAM_AS+3*MALC8_REQ_ID)
        ; REQ_AL =  LOW(MALC8_ID_META_RAM_AS+3*MALC8_REQ_ID)
        ; REQ_AH = MALC8_BUF_RAM_AS+REQ_ID+
        RCALL SR_GET_IDN_RAM_ADR
        ;RAMRDPZRAD REGTMP1,MALC8_A,MALC8_REQ_CAP ; read REQ_CAP into RT1
        RAMCOPYPAD2PR Z,MALC8_A,MALC8_REQ_CAP,Y,REGTMP1 ; copy REQ_CAP to IDN_CAP_RAM_ADR
        CopyYtoX  ; save y to x
        RCALL   SR_GET_IDN_AHL  ; Z=new IDN_RAM_AS, using Z,Y
        INC XL ; X -> IDN_AH_RAM_ADR
        ; get AH and write into meta
        ST  X, HIGH(Z)
        INC XL ; X -> IDN_AL_RAM_ADR
        ; get AL and write into meta
        ST  X, LOW(Z)
        ; post process and update status
        RAMINCPZAD MALC8_A,MALC8_SREG_NUM ; ALCed NUM +1
        RCALL SR_MAL_BUF_CHK_NFULL
        RET
SR_GET_IDN_RAM_ADR: ; input REQ_ID, output RAM_ADR in YPTR
        PZLDA MALC8_ID_META_RAM_AS
        RAMRDPZRAD REGTMP1,MALC8_A,MALC8_REQ_ID
        LDI REGTMP2, 3
        MUL REGTMP1, REGTMP2 ; R0=REQ_IDN*3=[0,3,6,9,12,15,18,21]
        ADD ZL, R0  ; get IDN_META_ADR=IDN_CAP_RAM_ADR
        CopyZtoY
        RET
SR_GET_IDN_AHL: ; input REQ_ID, output AHL in Z
        PZLDA MALC8_BUF_RAM_AS
JP_GET_IDN_AHL:
        RAMRDPZRAD REGTMP1,MALC8_A,MALC8_SREG_NUM ; RT1=allocated ID num
        RCALL SR_GET_IDN_RAM_ADR    ; get IDN(ALCed) CAP ADR in YPTR
        LDD REGTMP2,Y  ; get IDN(ALCed) CAP
        ADD ZL,REGTMP2
        DEC REGTMP1
        BRNE JP_GET_IDN_AHL   ; Z = new IDN_RAM_ADR
        RET
SR_FREE_BUF: not easy to implement
        NOP
.ENDMACRO

.MACRO BUF8
.EQU  MALC8_BUF_ID   = 
.EQU  MALC8_BUF_CAP  =  
.EQU  MALC8_BUF_AL   = 
.EQU  MALC8_BUF_AH   = 
.ENDMACRO