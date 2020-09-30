.INCLUDE "AM328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
;.EQU R0 = 0x0
;.EQU R1 = 0x1
;.EQU R17 = 17

.MACRO BITSET1
.IF @0>95 ; [D64=$40,RAMEND]
  PUSH R16
  LDS R16, @0				; 2c
  SBR R16, (1<<@1)			; 1c
  STS @0, R16				; 2c
  POP R16
.ELIF @0<=95
  BSET @1
.ELSE
.ENDIF
.ENDM

.MACRO BITSET2 ;Arguments: Address, Bit, Register
	.if @0>0x5F
;		lds  @2, @0
;		sbr  @2, (1<<@1)
;		sts  @0, @2
    LDS R16, @0				; 2c
    SBR R16, (1<<@1)			; 1c
    STS @0, R16				; 2c
;  .elif @0==0x5F
;    BSET @1
	.elif (@0<=0x5F && @0>0x3F)
    .if @0==0x5F
    BSET @1
    .endif
		in   R16, @0
		sbr  R16, (1<<@1)
		out  @0, R16
	.else
		sbi  @0-0x20, @1
	.endif
.ENDMACRO

NOP
; test R+index
; MOV 0,1
NOP
BITSET 0, 0
;BITSET R17, 0
;BITSET 1, 0
;BITSET R17, 0
;BITSET DDRB, 0
NOP
;BITSET SPH, 0   ; SFRH
;BITSET SREG, SREG_V
;BITSET 0x5f, SREG_V
NOP
;BITSET 0x100, 0
NOP
