; template
.MACRO TMP
NOP
.ENDMACRO

.MACRO INIT_SP
    LDI R16, HIGH(RAMEND)
    OUT SPH, R16
    LDI R16, LOW(RAMEND)
    OUT SPL, R16
.ENDMACRO
.MACRO PUSHSREG_0RT ; @0=REG Temp
  in    @0,SREG
  push  @0
.ENDMACRO

.MACRO POPSREG_0RT
  pop   @0
  out   SREG,@0
.ENDMACRO
.MACRO PUSHP_0P
        PUSH  @0H
        PUSH  @0L
.ENDMACRO
.MACRO POPP_0P
        POP  @0L
        POP  @0H
.ENDMACRO

.MACRO INIT_UART ; 115200 bps, Async, no Parity, 1 StopBit, 8 bit
; UCSRnC: URSEL=00 Async, UPM=00 Disable, USBS=0 1StopBit, *UCSZ=11 8bit
    LDI R16, (1<<UCSZ01)|(1<<UCSZ00)
    STS UCSR0C, R16
; UCSRnB: R/TXEN=1, RXCIE=1, UDRIE=1
    ;LDI R16, (1<<RXEN0)|(1<<TXEN0)
    LDI R16, (1<<RXEN0)|(1<<RXCIE0)|(1<<TXEN0)|(1<<UDRIE0)
    STS UCSR0B, R16
; UCSRnA: U2X=1 BAUD error to 2.1% @ 115200 bps, UBRR=16
    LDI R16, 1<<U2X0
    STS UCSR0A, R16
; UBRRnH:UBRRnL, BAUD Rate=115200 bps
    LDI R16, 16
    STS UBRR0L, R16
; Enable INT
;        SEI
.ENDMACRO
.MACRO INIT_UART1_9600 ; 115200 bps, Async, no Parity, 1 StopBit, 8 bit
; UCSRnC: URSEL=00 Async, UPM=00 Disable, USBS=0 1StopBit, *UCSZ=11 8bit
    LDI R16, (1<<UCSZ11)|(1<<UCSZ10)
    STS UCSR1C, R16
; UCSRnB: R/TXEN=1, RXCIE=1, UDRIE=1
    ;LDI R16, (1<<RXEN0)|(1<<TXEN0)
    LDI R16, (1<<RXEN1)|(1<<RXCIE1)|(1<<TXEN1)|(1<<UDRIE1)
    STS UCSR1B, R16
; UCSRnA: U2X=1 BAUD error to 2.1% @ 115200 bps, UBRR=16
    LDI R16, 1<<U2X1
    STS UCSR1A, R16
; UBRRnH:UBRRnL, BAUD Rate=115200 bps
    LDI R16, 207
    STS UBRR1L, R16
; Enable INT
;        SEI
.ENDMACRO

.MACRO INIT_RXPIN; init input pin
        ;.DEF  RXPORT = PORTC
        ;.DEF  PXPIN  = PINC0
        CBI   DDRC, DDC0
        ;SBI   RXPORT, 1<<RXPIN
; INIT_IN_PORT PORTC, PINC0
.ENDMACRO

.MACRO COPYR2AR ; copy GPR[R0,R31] to Adress_GPR[0,31][K-value=index_GPR]
    LDI ZL, @1 ; Load GPR address into Z pointer
    LDI ZH, 0  ; This will result in R30/31 being changed
    ST  Z,  @0
.ENDMACRO
;*********************************************************
;*    16-bit Pointer Register load
;*********************************************************
.MACRO PLDPA ; @0=XYZ_PTR, @1=DATA
;Arguments: Register, Data
    ldi        @0L, LOW(2*@1)            ;set rL to low byte
    ldi        @0H, HIGH(2*@1)            ;set rH to high byte
.ENDMACRO
.MACRO PLD_0A1P ;Pointer LoaD @0=RAM Address into @1=Pointer[XYZ]
        LDI   @1L,  LOW(@0)
        LDI   @1H, HIGH(@0)
.ENDMACRO
.MACRO PLDRAM_0AL1P ; RAM 0ADRL+0ADRH have a address, load word into P
        LDS   @1L, @0
        LDS   @1H, @0+1
.ENDMACRO
.MACRO PWRRAM_0AL1P ; RAM 0ADRL+0ADRH have a address, load word into P
        STS   @0,   @1L
        STS   @0+1, @1H
.ENDMACRO
.MACRO PXLDA ; @0 = RAM_ADR
      ;Pointer X LoaD RAM Address
        LDI   XL, LOW(@0)
        LDI   XH, HIGH(@0)
.ENDMACRO
.MACRO PYLDA ; @0 = RAM_ADR
      ;Pointer Y LoaD RAM Address
        LDI   YL, LOW(@0)
        LDI   YH, HIGH(@0)
.ENDMACRO
.MACRO PZLDA ; @0 = RAM_ADR
      ;Pointer Z LoaD RAM Address
        LDI   ZL, LOW(@0)
        LDI   ZH, HIGH(@0)
.ENDMACRO
.MACRO RAMRDPZRA   ; @0=REG @1=RAM_ADR
      ;ReaD RAM Address into REG
        PZLDA @1
        LDD   @0, Z
.ENDMACRO
.MACRO LDD16_0P1DL23RHL  ; LDD @0=PTR_XYZ, @1=DISLOW, @2=RH, @3=RL
        LDD   @3, @0+@1
        LDD   @2, @0+(@1+1)
.ENDMACRO
.MACRO LDD16_0P1DL2P    ; LDD @0=PTR_XYZ, @1=DISLOW, @2=PTR (H,L)
        LDD16_0P1DL23RHL @0,@1,@2H,@2L
.ENDMACRO
.MACRO STD16_0P1DL23RHL
        STD   @0+@1,    @3
        STD   @0+(@1+1),  @2
.ENDMACRO
.MACRO STD16_0P1DL2P
        STD16_0P1DL23RHL @0,@1,@2H,@2L
.ENDMACRO
.MACRO RAMWRPZAR   ; @0=RAM_ADR, @1=REG
      ;WRite REG into RAM Address
        PZLDA @0
        STD   Z, @1
.ENDMACRO
.MACRO RAMRDPZRAD   ; @0=REG @1=RAM_ADR, @2=K6D[0,63]
      ;ReaD RAM Address + Displacement into REG
        PZLDA @1
        LDD   @0, Z+@2
.ENDMACRO
.MACRO RAMWRPZADR   ; @0=RAM_ADR, @1=K6D[0,63], @2=REG
      ;WRite REG into RAM Address + Displacement
        PZLDA @0
        STD  Z+@1, @2
.ENDMACRO
.MACRO RAMINCPZA ; @0=RAM_ADR, @1=REG[GPR]
        PZLDA  @0
        LD      @1, Z
        INC     @1
        ST      Z,  @1
.ENDMACRO
.MACRO RAMDECPZA ; @0=RAM_ADR, @1=REG[GPR]
        PZLDA  @0
        LD      @1, Z
        DEC     @1
        ST      Z,  @1
.ENDMACRO
.MACRO RAMINCPZAD   ; @0=RAM_ADR, @1=K6D[0,63], @2=REG
        ;RAMRDPZRAD @2, @0, @1
        PZLDA @0
        LDD   @2, Z+@1
        INC @2
        ;RAMWRPZADR @0, @1, @2
        STD  Z+@1, @2
.ENDMACRO
.MACRO RAMDECPZAD   ; @0=RAM_ADR, @1=K6D[0,63], @2=REG
        ;RAMRDPZRAD @2, @0, @1
        PZLDA @0
        LDD   @2, Z+@1
        DEC @2
        ;RAMWRPZADR @0, @1, @2
        STD  Z+@1, @2
.ENDMACRO
.MACRO RAMWRAIR ; @0=RAM_ADR, @1=K[0,255], @2=TRH
        ; like LDI GPR(RAM), K ; RAM=IOSFR+RAM
  .IF   @0<=$3F ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        LDI   @2,   @1
        OUT   @0,   @2
  .ELSE ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDI   @2,   @1
        STS   @0,   @2
  .ENDIF
.ENDMACRO
;*******************************************************************************
.MACRO GetGPR1stLSBCLRID ; @0=REG[GPR], @1=REG[GPR]4INDEX[0,7]
      CLC
      CLR   @1
      INC   @1
      LSR   @0
      BRCS  PC-0x02
      DEC   @1
      CLC
.ENDMACRO
.MACRO GetGPR1stLSBSETID ; @0=REG[GPR], @1=REG[GPR]4INDEX[0,7]
      CLC
      CLR   @1
      INC   @1
      LSR   @0
      BRCC  PC-0x02
      DEC   @1
      CLC
.ENDMACRO

;***********************************************************************
; WRI - WRite Immediate value into SRAM
;***********************************************************************
.MACRO WRI  ; @0=RAM ADR, @1=K8, @2=REGTMP[GPR]
        PZLDA @0
        LDI   @2 ,@1        ;get fill value in r16
        ST    Z, @2       ;out in SRAM and decrement X-pointer
.ENDMACRO
;***********************************************************************
;FILL THE RAM.  Uses Z pointer and R16
;* start location : @0 (16-bit pointer)
;* end location   : @1 (16-bit)
;* fill value   : @2 (8-bit)
;***********************************************************************
; WRI - WRite Immediate value into SRAM
;***********************************************************************
.MACRO RAMFILLPZAAKR ; @0=RAM_ADR_START,@1=RAM_ADR_END,@2=K8,@3=REGTMP[GPR]
        PZLDA @0
        LDI   @3 ,@2        ;get fill value in r16
        ST    Z+, @3        ;out in SRAM and decrement X-pointer
        CPI   ZH, HIGH(@1)
        BRCS  PC-0x02       ;if not end of table, loop more
        CPI   ZL, LOW(@1)
        BRNE  PC-0x04       ;if not end of table, loop more
.ENDMACRO
.MACRO RAMCLRPZAAR ; @0=RAM_ADR_START,@1=RAM_ADR_END,@2=REGT[GPR]
        PZLDA @0
        CLR   @2
        ST    Z+, @2
        CPI   ZH, HIGH(@1)
        BRCS  PC-0x02       ;if not end of table, loop more
        CPI   ZL, LOW(@1)
        BRNE  PC-0x04       ;if not end of table, loop more
.ENDMACRO
.MACRO RAMSETPZAARH ; @0=RAM_ADR_START,@1=RAM_ADR_END,@2=REGT[RH]
        PZLDA @0
        SER   @2
        ST    Z+, @2
        CPI   ZH, HIGH(@1)
        BRCS  PC-0x02       ;if not end of table, loop more
        CPI   ZL, LOW(@1)
        BRNE  PC-0x04       ;if not end of table, loop more
.ENDMACRO
.MACRO RAMSET ; @0=RAM_ADR,@1=REGTMP[GPR]
        WRI @0, $FF, @1
.ENDMACRO
.MACRO RAMCLR ; @0=RAM_ADR,@1=REGTMP[GPR]
        WRI @0, $0, @1
.ENDMACRO
.MACRO RAMBSET_0P1D2BIT3RHT
        LDD   @3,     @0+@1
        SBR   @3,     (1<<@2)
        STD   @0+@1,  @3
.ENDMACRO
.MACRO RAMBCLR_0P1D2BIT3RHT
        LDD   @3,     @0+@1
        CBR   @3,     (1<<@2)
        STD   @0+@1,  @3
.ENDMACRO
.MACRO RAMBSETPZADBR ; @0=ADR, @1=DISP @2=BIT index, @3=REGTMP
    PZLDA @0
    LDD  @3, Z+@1
    SBR  @3, (1<<@2)
    STD  Z+@1, @3
.ENDMACRO
.MACRO RAMBCLRPZADBR ; @0=ADR, @1=DISP @2=BIT index, @3=REGTMP
    PZLDA @0
    LDD  @3, Z+@1
    CBR  @3, (1<<@2)
    STD  Z+@1, @3
.ENDMACRO
; ---- BIT manipulating
.MACRO RAMBNOTABRH   ; @0=RAMADR, @1=BITID[0,7], @2=TRH [SREG-T]
  .IF @1>7
    .message "Only values 0-7 allowed for Bit parameter"
  .ENDIF
  PUSHSREG_0RT @2
  .IF @0>$3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
    LDS  @2, @0
    ; now @2=TRH contain @1=BITID[0,7] status
    ; method 1, not work
    ;BST   @2, @1 ; 1c
    ;BRTS  PC+0x2 ; 1c; clr_t
    ;BRTC  PC+0x3 ;set_t
    ;CBR  @2, @1
    ;PC+0x1       ; no work
    ;SBR  @2, @1
    ; method 2
    COM  @2      ; NOT BYTE
    BST  @2, @1  ; save not bit to SREG-T
    COM  @2      ; recover BYTE
    BLD  @2, @1  ; write not bit to orginal BYTE
    STS  @0, @2
  .ELSE             ; IOSFR
    IN   @2, @0
    COM  @2      ; NOT BYTE
    BST  @2, @1  ; save not bit to SREG-T
    COM  @2      ; recover BYTE
    BLD  @2, @1  ; write not bit to orginal BYTE
    OUT  @0, @2
  .ENDIF
  POPSREG_0RT @2
.ENDMACRO
.MACRO BNOTSREGTR ; @0=TR
        IN    @0,   SREG
        ;BRTS  PC+0x2  ; if set, clear T
        BRTC  PC+0x3  ; if clear, set T
        CLT
        SBRS  @0, SREG_T  ; skip next if set, keep clear
        SET 
.ENDMACRO
.MACRO     BITCOPY; @0=REGs[0,31],@1=bit[0,7],@2=REGd[0,31],@3=bit[0,7]
                ;let IGSOUTLST = IGSOUT         ; save last IGSOUT
                BST     @0,        @1                     ; SAVE IGSOUT INTO SREG-T
                BLD     @2,        @3                     ; SAVE SREG-T INTO IGSOUTLST
.ENDMACRO
.MACRO BITEOR ; @0=R1,@1=BIT1,@2=R2,@3=BIT2,@4=RTMP1,@5=RTMP2
                CLR     @4
                CLR     @5
                ;PUSH     SREG
                BITCOPY @0, @1, @4, 0
                ;BST     @0,    @1
                ;BLD     @4,    0
                BITCOPY @2, @3, @5, 0
                ;BST     @2,    @3
                ;BLD     @5,    0
                EOR     @4,    @5; if IGSOUT!=RGMOUT, Z=1, == Z=0
.ENDMACRO
; ---- RAM (GPRs/SFRs/SRAM) read/write
.MACRO MIOLD        ; MIO LoaD REGdst[0,31], IOADRsrc[0,$FF]; 1/2 cycles
    .IF    @1<0x40
        IN    @0, @1
    .ELSE
        LDS    @0, @1
    .ENDIF
.ENDMACRO

.MACRO MIOLDW        ; MIO LoaDWord REGdst[RegH:RegL], IOADRsrc[0,$FF]; 2/4 cycles
    .IF    @2<0x40
        IN    @1, @2
        IN    @0, @2+1
    .ELSE
        LDS    @1, @2
        LDS    @0, @2+1
    .ENDIF
.ENDMACRO

.MACRO MIOLDWI            ; MIO LoaD into @0 RegH, @1 RegL, with @2 ImmediateWord, [@4 TempReg]; ? cycles
    .IF (@0>=16)&&(@0<=31)
        LDI    @1,  LOW(@2)    ; save low value to low address
        LDI    @0, HIGH(@2)
    .ELIF (@0<16) || ((@0>31)&&(@0<0x40))
        LDI    @4,  LOW(@2)
        OUT    @1, @4        ; save low value to low address
        LDI    @4, HIGH(@2)
        OUT    @0, @4
    .ELSE
        LDI    @4,  LOW(@2)
        STS @1, @4
        LDI    @4, HIGH(@2)
        STS @0, @4
    .ENDIF
.ENDMACRO

.MACRO MIOST        ; MIO STore IOADRdst[0,$FF], REGsrc[0,31]; 1/2 cycles
    .IF    @0<0x40
        OUT    @0, @1
    .ELSE
        STS    @0, @1
    .ENDIF
.ENDMACRO

.MACRO MIOSTI        ; MIO STore IOADRdst[0,$FF], ImmediateValue; ? cycles
    .IF (@0>=16)&&(@0<=31)
        LDI @0, @1
    .ELSE
        LDI R16, @1
    .IF    @0<0x16
        OUT    @0,    R16
    .ELSE
        STS    @0,    R16
    .ENDIF
.ENDMACRO

.MACRO MIOSTW        ; MIO STore IOADRdst, RegH, RegL; 2/4 cycles
    .IF    @0<0x40
        OUT    @0, @2
        OUT    @0+1, @1
    .ELSE
        STS    @0, @2
        STS    @0+1, @1
    .ENDIF
.ENDMACRO
; ---- RAM (GPRs/SFRs/SRAM) read/write
; ---- RAM bit manipulating
; re-map relative IO address to absolute MEM address for BIT manipulating
.MACRO SETB   ; @0=IOSFR/EIOSFR/RAM ADR, @1=BITK[0,7], @2=REGT[RH]
      ;Arguments: Address, Bit, Register
    .IF @1>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @2, @0
        SBR  @2, (1<<@1)
        STS  @0, @2
    .ELIF @0>0x1F     ; IOSFRH [D64=$40,D95=$5F] IO[D32=$20,D63=$3F]
        IN   @2, @0
        SBR  @2, (1<<@1)
        OUT  @0, @2
    .ELSE             ; IOSFRL [D32=$20,D63=$3F] IO[D00=$00,D31=$1F]
        SBI  @0, @1
    .ENDIF
.ENDMACRO
.MACRO CLRB         ;Arguments: Address, Bit, Register
    .IF @1>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F
        LDS  @2, @0
        cbr  @2, (1<<@1)
        STS  @0, @2
    .ELIF @0>0x1F
        in   @2, @0
        cbr  @2, (1<<@1)
        out  @0, @2
    .ELSE
        CBI  @0, @1
    .ENDIF
.ENDMACRO
;*******************************************************************************
.MACRO IOREGSET1B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @2=TRH
        SETB  @0, @1, @2
.ENDMACRO
.MACRO IOREGCLR1B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @2=TRH
        CLRB  @0, @1, @2
.ENDMACRO
.MACRO IOREGSET2B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @3, @0
        SBR  @3, (1<<@1)|(1<<@2)
        STS  @0, @3
    .ELIF @0>0x1F     ; IOSFRH [D64=$40,D95=$5F] IO[D32=$20,D63=$3F]
        IN   @3, @0
        SBR  @3, (1<<@1)|(1<<@2)
        OUT  @0, @3
    .ELSE             ; IOSFRL [D32=$20,D63=$3F] IO[D00=$00,D31=$1F]
        SBI  @0, @1
        SBI  @0, @2
    .ENDIF
.ENDMACRO
.MACRO IOREGCLR2B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @3, @0
        CBR  @3, (1<<@1)|(1<<@2)
        STS  @0, @3
    .ELIF @0>0x1F     ; IOSFRH [D64=$40,D95=$5F] IO[D32=$20,D63=$3F]
        IN   @3, @0
        CBR  @3, (1<<@1)|(1<<@2)
        OUT  @0, @3
    .ELSE             ; IOSFRL [D32=$20,D63=$3F] IO[D00=$00,D31=$1F]
        CBI  @0, @1
        CBI  @0, @2
    .ENDIF
.ENDMACRO
.MACRO IOREGSET3B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @4, @0
        SBR  @4, (1<<@1)|(1<<@2)|(1<<@3)
        STS  @0, @4
    .ELIF @0>0x1F     ; IOSFRH [D64=$40,D95=$5F] IO[D32=$20,D63=$3F]
        IN   @4, @0
        SBR  @4, (1<<@1)|(1<<@2)|(1<<@3)
        OUT  @0, @4
    .ELSE             ; IOSFRL [D32=$20,D63=$3F] IO[D00=$00,D31=$1F]
        SBI  @0, @1
        SBI  @0, @2
        SBI  @0, @3
    .ENDIF
.ENDMACRO
.MACRO IOREGCLR3B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @4, @0
        CBR  @4, (1<<@1)|(1<<@2)|(1<<@3)
        STS  @0, @4
    .ELIF @0>0x1F     ; IOSFRH [D64=$40,D95=$5F] IO[D32=$20,D63=$3F]
        IN   @4, @0
        CBR  @4, (1<<@1)|(1<<@2)|(1<<@3)
        OUT  @0, @4
    .ELSE             ; IOSFRL [D32=$20,D63=$3F] IO[D00=$00,D31=$1F]
        CBI  @0, @1
        CBI  @0, @2
        CBI  @0, @3
    .ENDIF
.ENDMACRO
.MACRO IOREGSET4B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @5, @0
        SBR  @5, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)
        STS  @0, @5
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @5, @0
        SBR  @5, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)
        OUT  @0, @5
    .ENDIF
.ENDMACRO
.MACRO IOREGCLR4B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @5, @0
        CBR  @5, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)
        STS  @0, @5
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @5, @0
        CBR  @5, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)
        OUT  @0, @5
    .ENDIF
.ENDMACRO
.MACRO IOREGSET5B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7 || @5>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @6, @0
        SBR  @6, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)
        STS  @0, @6
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @6, @0
        SBR  @6, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)
        OUT  @0, @6
    .ENDIF
.ENDMACRO
.MACRO IOREGCLR5B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7 || @5>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @6, @0
        CBR  @6, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)
        STS  @0, @6
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @6, @0
        CBR  @6, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)
        OUT  @0, @6
    .ENDIF
.ENDMACRO
.MACRO IOREGSET6B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7 || @5>7 || @6>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @7, @0
        SBR  @7, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)
        STS  @0, @7
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @7, @0
        SBR  @7, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)
        OUT  @0, @7
    .ENDIF
.ENDMACRO
.MACRO IOREGCLR6B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7 || @5>7 || @6>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @7, @0
        CBR  @7, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)
        STS  @0, @7
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @7, @0
        CBR  @7, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)
        OUT  @0, @7
    .ENDIF
.ENDMACRO
.MACRO IOREGSET7B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7 || @5>7 || @6>7 || @7>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @8, @0
        SBR  @8, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)|(1<<@7)
        STS  @0, @8
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @8, @0
        SBR  @8, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)|(1<<@7)
        OUT  @0, @8
    .ENDIF
.ENDMACRO
.MACRO IOREGCLR7B   ; @0=IOSFR/EIOSFR/RAM_ADR, @1=BITNAME_K[0,7], @[2,N] @$=TRH
    .IF @1>7 || @2>7 || @3>7 || @4>7 || @5>7 || @6>7 || @7>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
        LDS  @8, @0
        CBR  @8, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)|(1<<@7)
        STS  @0, @8
    .ELSE             ; IOSFR [D32=$20,D95=$5F] IO[D00=$00,D63=$3F]
        IN   @8, @0
        CBR  @8, (1<<@1)|(1<<@2)|(1<<@3)|(1<<@4)|(1<<@5)|(1<<@6)|(1<<@7)
        OUT  @0, @8
    .ENDIF
.ENDMACRO
;*******************************************************************************
.MACRO RDEC2RBIN ; @0=REG_BIT_INDEX_DEC_TYP[0,7], @1=REG[RH]_BIN_TYPE
       ; REGT Result DEC [1,2,4,8,16,32,64,128]
        PUSH  @0  ; save to stack
        CLZ
        CLR   @1
RDEC2RBIN_CHK_0:
        TST   @0
        BREQ  RDEC2RBIN_SET1
        LDI   @1, 1
        LSL   @1
        DEC   @0
        TST   @0
        BRNE  PC-0x3
        RJMP  RDEC2RBIN_EXIT
;        BREQ  PC+0x2
RDEC2RBIN_SET1:
        LDI   @1, 1
RDEC2RBIN_EXIT:
        POP   @0
.ENDMACRO
.MACRO BSETAVR ; @0=RAM_ADR, @1=REG with index variable[0,7], @2=REG[RH]
      ; BIT SET RAM_ADR[SFR+SRAM] bit index with variable value[0,7], REG_RH
    PUSH  @1
    RDEC2RBIN @1, @2
  .IF     @0>0x3F
        LDS   @1, @0
    OR    @1, @2
        STS   @0, @1
    POP   @1
  .ELSE
        IN    @1, @0
        OR    @1, @2
        OUT   @0, @1
    POP   @1
    .ENDIF
.ENDMACRO
.MACRO BCLRAVR ; @0=RAM_ADR, @1=REG with index variable[0,7], @2=REG[RH]
      ; BIT SET RAM_ADR[SFR+SRAM] bit index with variable value[0,7], REG_RH
    PUSH  @1
    RDEC2RBIN @1, @2
    COM   @2
  .IF     @0>0x3F
        LDS   @1, @0
    AND   @1, @2
        STS   @0, @1
    POP   @1
  .ELSE
        IN    @1, @0
        AND   @1, @2
        OUT   @0, @1
    POP   @1
    .ENDIF
.ENDMACRO
.MACRO BSETPZAVR ; @0=RAM_ADR, @1=REG with index variable[0,7], @2=REG[RH]
      ; BIT SET RAM_ADR[SFR+SRAM] (load into Z PTR) bit index with variable value[0,7], REG_RH
    PUSH  @1
    RDEC2RBIN @1, @2
    PZLDA @0
  .IF     @0>0x3F
        LD    @1, Z
    OR    @1, @2
        ST    Z, @1
    POP   @1
  .ELSE
        IN    @1, @0
        OR    @1, @2
        OUT   @0, @1
    POP   @1
    .ENDIF
.ENDMACRO
.MACRO BCLRPZAVR ; @0=RAM_ADR, @1=REG with index variable[0,7], @2=REG[RH]
      ; BIT SET RAM_ADR[SFR+SRAM] (load into Z PTR) bit index with variable value[0,7], REG_RH
    PUSH  @1
    RDEC2RBIN @1, @2
    COM   @2
    PZLDA @0
  .IF     @0>0x3F
        LD    @1, Z
    AND   @1, @2
        ST    Z, @1
    POP   @1
  .ELSE
        IN    @1, @0
        AND   @1, @2
        OUT   @0, @1
    POP   @1
    .ENDIF
.ENDMACRO
.MACRO BSETPZADVR ; @0=RAM_ADR, @1=DISP, @2=REG with index variable[0,7], @3=REG[RH]
      ; BIT SET RAM_ADR[SFR+SRAM] (load into Z PTR) bit index with variable value[0,7], REG_RH
    PUSH  @2
    RDEC2RBIN @2, @3
    PZLDA @0
  .IF     @0>0x3F
        LDD   @2, Z+@1
    OR    @2, @3
        ST    Z+@1, @2
    POP   @2
  .ELSE
        IN    @2, @0
        OR    @2 @3
        OUT   @0, @3
    POP   @2
    .ENDIF
.ENDMACRO
.MACRO BCLRPZADVR ; @0=RAM_ADR, @1=DISP, @2=REG with index variable[0,7], @3=REG[RH]
      ; BIT SET RAM_ADR[SFR+SRAM] (load into Z PTR) bit index with variable value[0,7], REG_RH
    PUSH  @2
    RDEC2RBIN @2, @3
    COM   @3
    PZLDA @0
  .IF     @0>0x3F
        LDD   @2, Z+@1
    AND   @2, @3
        STD   Z+@1, @2
    POP   @2
  .ELSE
        IN    @2, @0
        AND   @2, @3
        OUT   @0, @2
    POP   @2
    .ENDIF
.ENDMACRO
.MACRO BITSET            ; BIT SET @0=ADR, @1=BIT index, @2 REGTMP[16,31]
  .IF @1>7
          .message "Only values 0-7 allowed for Bit parameter"
  .ENDIF
  .IF @0>=0 && @0<=15                                ;   ; RL [0,D15=$0F]
          PUSH R16
          MOV    R16, @0
          SBR R16, (1<<@1)
          MOV @0, R16
          POP R16
  .ELIF @0>=16 && (@0<=31)    ;   ; RH [D16=$10,D31=$1F]
        SBR @0, (1<<@1)                    ; 1c;
    .ELIF (@0>=32)&&(@0<=63)    ;   ; SFRL [D32=$20,D63=$3F],IO[0,D31=$1F]
        SBI (@0-0x20), (1<<@1)    ; 2c;
    .ELIF (@0>=64)&&(@0<=94)    ;   ; SFRH [D64=$40,D95=$5F],IO[D32=$20,D63=$3F]
        PUSH R16
        IN     R16, @0                            ; 1c; @2 TempReg[16,31]
        SBR R16, (1<<@1)                ; 1c
        OUT @0, R16                            ; 1c
    .ELIF (@0==95)
        BSET @1
    .ELIF (@0>63)&&(@0<=RAMEND) ; [D64=$40,RAMEND]
      PUSH R16
        LDS R16, @0                ; 2c
        SBR R16, (1<<@1)            ; 1c
        STS @0, R16                ; 2c
        POP R16
    .ELSE
        .message "Address out of RAMEND)"
    .ENDIF
.ENDMACRO

.MACRO BSETM            ; MIO SET @0 REG/SRAM, with @1 mask, @2 TempReg[16,31]
    .IF @0<=15                    ; [0,D15=$0F]
        MOV    @2, @0                ; 1c
        SBR @2, @1                ; 2c
        MOV @0, @2                ; 1c
    .ELIF (@0>=16)&&(@0<=31)    ; [D16=$10,D31=$1F]
        SBR @0, @1                ; 1c
    .ELIF (@0>=32)&&(@0<=63)    ; D63=$3F
        IN     @2, @0                ; 1c; @2 TempReg[16,31];
        SBR @2, @1                ; 1c
        OUT @0, @2                ; 1c
    .ELIF (@0>63)&&(@0<=RAMEND)
        LDS @2, @0                ; 2c
        SBR @2, @1                ; 1c
        STS @0, @2                ; 2c
    .ELSE
        .message "out RAMEND"
    .ENDIF
.ENDMACRO

.MACRO CLRB1            ; MIO CLeaR bit in @0 REG/SRAM, at @1 one BIT Address, @2 TempReg[16,31]
    .IF @1>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0<=15                    ; [0,D15=$0F]
        CBI @0, @1                ; 2c
    .ELIF (@0>=16)&&(@0<=31)    ; [D16=$10,D31=$1F]
        CBR @0, (1<<@1)            ; 1c
    .ELIF (@0>=32)&&(@0<=63)    ; [D32=$20,D63=$3F]
        IN     @2, @0                ; 1c; @2 TempReg[16,31]
        CBR @2, (1<<@1)            ; 1c
        OUT @0, @2                ; 1c
    .ELIF (@0>63)&&(@0<=RAMEND) ; [D64=$40,RAMEND]
        LDS @2, @0                ; 2c
        CBR @2, (1<<@1)            ; 1c
        STS @0, @2                ; 2c
    .ELSE
        .message "Address out of RAMEND)"
    .ENDIF
.ENDMACRO

.MACRO CLRBM            ; MIO CLeaR bit in @0 REG/SRAM, with @1 mask, @2 TempReg[16,31]
    .IF @0<=15                    ; [0,D15=$0F]
        MOV    @2, @0                ; 1c
        SBR @2, @1                ; 2c
        MOV @0, @2                ; 1c
    .ELIF (@0>=16)&&(@0<=31)    ; [D16=$10,D31=$1F]
        SBR @0, @1                ; 1c
    .ELIF (@0>=32)&&(@0<=63)    ; D63=$3F
        IN     @2, @0                ; 1c; @2 TempReg[16,31];
        SBR @2, @1                ; 1c
        OUT @0, @2                ; 1c
    .ELIF (@0>63)&&(@0<=RAMEND)
        LDS @2, @0                ; 2c
        SBR @2, @1                ; 1c
        STS @0, @2                ; 2c
    .ELSE
        .message "out RAMEND"
    .ENDIF
.ENDMACRO
; ---- RAM bit manipulating ----------------------------------------------------------------------------
; ---- Condition ----
;*********************************************************
;*    Bit test anywhere in IO or in lower $FF of data space
;*  SKBS : SKip if Bit Set
;*  SKBC : SKip if Bit Cleared
;*********************************************************
.MACRO SKBS          ;Arguments: Address, Bit, Register
;    .IF @1>7
;        .message "Only values 0-7 allowed for Bit parameter"
;    .ENDIF
    .IF @0>0x3F
        LDS  @2, @0
        sbrs @2, @1
    .ELIF @0>0x1F
        in   @2, @0
        sbrs @2, @1
    .ELSE
        sbis @0, @1
    .ENDIF
.ENDMACRO

.MACRO SKBC          ;Arguments: Address, Bit, Register
    .IF @1>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
    .IF @0>0x3F
        LDS     @2, @0
        sbrc @2, @1
    .ELIF @0>0x1F
        in     @2, @0
        sbrc @2, @1
    .ELSE
        sbic @0, @1
    .ENDIF
.ENDMACRO
.MACRO SKRAMBSPZADB  ; @0=RAM_ADR, @1=DISP, @2BIT_index, @3=REGT[GPR]
      ;skip if bit set in RAM_ADR + Displacement
    .IF @2>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
  RAMRDPZRAD @3, @0, @1
  SBRS       @3, @2
.ENDMACRO
.MACRO SKRAMBCPZADB  ; @0=RAM_ADR, @1=DISP, @2BIT_index, @3=REGT[GPR]
      ;skip if bit clr in RAM_ADR + Displacement
    .IF @2>7
        .message "Only values 0-7 allowed for Bit parameter"
    .ENDIF
  RAMRDPZRAD @3, @0, @1
  SBRC       @3, @2
.ENDMACRO
.MACRO MCEQ                ; Macro Condition if EQual; MCEQ Rd, Rs, ADREQ, ADRNE
            ; If equal
            ; M_IEQ Rd,Rs,k,l   0<=d<=31,0<=s<=31,0<=k<=$FFFF,0<=l<=$FFFF   4-5 cycles

            ; M_IEQ R16,R17,LBL_A_Act1,LBL_A_Act2 - usage example
            ; use secondary rjmp/jmp just after the M_IEQ instruction
            ; to make jumps longer than 64 instructions

            cp @0,@1                ;1
            breq @2                    ;1-2        // limited by 64 instructions
            rjmp @3                    ;2
.ENDMACRO

; ---- Condition ----
;
;****************************************************************************/
;MACRO TWI_Start: used to initiate a START condition and start the TWI
;                    state machine.
;****************************************************************************/
.MACRO TWI_START
    ldi        Wreg1,((1<<TWINT)|(1<<TWSTA)|(1<<TWEN)|(1<<TWIE))
    ;STORE    TWCR,Wreg1                    ;Set START condition
.ENDMACRO

.MACRO COPY16_01RrHL23RdHL ; from source RrH,RrL to dest RdH,RdL
    movw    @2:@3,@0:@1
.ENDMACRO

.MACRO CopyZtoX
    movw    XH:XL,ZH:ZL
.ENDMACRO

.MACRO CopyXtoZ
    movw    ZH:ZL,XH:XL
.ENDMACRO

.MACRO CopyZtoY
    movw    YH:YL,ZH:ZL
.ENDMACRO

.MACRO CopyYtoZ
    movw    ZH:ZL,YH:YL
.ENDMACRO

.MACRO CopyYtoX
    movw    XH:XL,YH:YL
.ENDMACRO

.MACRO CopyXtoY
    movw    YH:YL,XH:XL
.ENDMACRO

.MACRO RAMCOPYAR ; @0=RAMADR, @1=GPR
  .IF @0>$3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
    LDS  @1, @0
  .ELSE             ; IOSFR
    IN   @1, @0
  .ENDIF
.ENDMACRO
.MACRO RAMRDAR ; @0=RAMADR,@1=GPR
    RAMCOPYAR @0,@1
.ENDMACRO
.MACRO RAMWRRA ; @0=GPR, @1=RAMADR
  .IF @1>$3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
    STS  @1, @0
  .ELSE             ; IOSFR
    OUT  @1, @0
  .ENDIF
.ENDMACRO

.MACRO RAMCOPYAAR ; @0=RAMADR, @1=RAMADR, @2=TR
  .IF @0>$3F       ; EIOSFR+RAM [D96=$60,RAMEND] IO[D64=$40,RAMEND]
    LDS  @2, @0
    .IF  @1>$3F
      STS  @1, @2
    .ELSE
      OUT  @1, @2
    .ENDIF
  .ELSE             ; IOSFR
    IN   @2, @0
    .IF  @1>$3F
      STS  @1, @2
    .ELSE
      OUT  @1, @2
    .ENDIF
  .ENDIF
.ENDMACRO
;***********************************************************************
;COPY STRING FROM RAM TO RAM.  Uses X and Y pointer, R16 and R17.
;* RAM Source        : @0 (16-bit pointer)
;* RAM Destination    : @1 (16-bit pointer)
;* number of bytes    : @2 (8-bit)
;***********************************************************************
.MACRO COPYRAM        ; CopyRAM    Source(16),Destination(16),count(8)
    ldi        XH,high(@0)
    ldi        XL,low(@0)
    ldi        YH,high(@1)
    ldi        YL,low(@1)
    ldi        r16,@2                ;get fill value in r16
_COPYRAM1:
    ld        r17,X+                ;get byte, increment source pointer
    st        Y+,r17                ;save byte, increment destination pointer
    dec        r16                    ;decrement and test for zero
    brne    _COPYRAM1            ;loop for more
.ENDMACRO
.MACRO RAMCOPYPA2PAR
      ;RAMCOPY @0=XYZPTR,@1=RAM_ADR_src, to @2=XYZPTR,@3=RADR_dst,@4=REGT[GPR]
  LDI   @0H,  HIGH(@1)
  LDI   @0L,   LOW(@1)
  LDI   @2H,  HIGH(@3)
  LDI   @2L,   LOW(@3)
  LD    @4,   @0
  ST    @2,   @4
.ENDMACRO
.MACRO RAMCOPYPAD2PADR
      ;RAMCOPY @0=XYZPTR,@1=RAM_ADR,@2=DIS to @3=XYZPTR,@4=RA,@5=DIS via @6=REGT[GPR]
  LDI   @0H,  HIGH(@1)
  LDI   @0L,   LOW(@1)
  LDI   @3H,  HIGH(@4)
  LDI   @3L,   LOW(@4)
  LDD   @6,       @0+@2
  ST    @3+@5,    @6
.ENDMACRO
.MACRO RAMCOPYP2PADR
      ;RAMCOPY @0=XYZPTR to @1=XYZPTR,@2=RA,@3=DIS via @4=REGT[GPR]
  LDI   @1H,  HIGH(@2)
  LDI   @1L,   LOW(@2)
  LD    @4,       @0
  STD   @1+@3,    @4
.ENDMACRO
.MACRO RAMRD_0PXYZ1A2D3R ;RAMRD @0=XYZPTR,@1=RA,@2=DIS to @3=REG[GPR]
  LDI   @0H,  HIGH(@1)
  LDI   @0L,   LOW(@1)
  LDD   @3,   @0+@2
.ENDMACRO
.MACRO RAMCOPYPAD2PR
      ;RAMCOPY @0=XYZPTR,@1=RA,@2=DIS to @3=XYZPTR via @4=REGT[GPR]
  LDI   @0H,  HIGH(@1)
  LDI   @0L,   LOW(@1)
  LDD   @4,       @0+@1
  ST    @3,       @4
.ENDMACRO
.MACRO RAMCOPY0PXYZ1A2R
      ;copy RAM data @0=PTR 3(X/Y/Z) with @1=ADR to @2=GPR
      LDI   @0H,  HIGH(@1)
      LDI   @0L,   LOW(@1)
      LD    @2,       @0
.ENDMACRO
.MACRO RAMCOPY0PXYZINC1A2R
      ;copy RAM data @0=PTR 3(X/Y/Z)+ with @1=ADR to @2=GPR
      LDI   @0H,  HIGH(@1)
      LDI   @0L,   LOW(@1)
      LD    @2,       @0+
.ENDMACRO
.MACRO RAMCOPY0PXYZDEC1A2R
      ;copy RAM data @0=PTR 3(X/Y/Z)+ with @1=ADR to @2=GPR
      LDI   @0H,  HIGH(@1)
      LDI   @0L,   LOW(@1)
      LD    @2,       @0-
.ENDMACRO
