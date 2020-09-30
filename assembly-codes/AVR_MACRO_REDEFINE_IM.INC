; Re-Define Instruction mnemonics
; need a modified AM328PDEF.inc from M328PDEF.inc
; use for AVR_MACRO.INC
.MACRO  SBII ; @0=SFRL_IO5[0,31], @1=BIT index
        ;IOMEMSET
;        .SET EECR = 0x10
        ;SBI @0, @1
        ;IOMEMUNSET
.ENDM
.MACRO  CBI ; @0=SFRL_IO5[0,31], @1=BIT index
        BCLR @0, @1
.ENDM
.MACRO  IN
        IN @0, @1
.ENDM
.MACRO  OUT
        OUT @0, @1
.ENDM
.MACRO IOMEMSET
.SET	SREG	= 0x3f
.SET	SPL		= 0x3d
.SET	SPH		= 0x3e
.SET	SPMCSR= 0x30
.SET	MCUCR	= 0x30
.SET	MCUSR	= 0x30
.SET	SMCR	= 0x33
.SET	ACSR	= 0x30
.SET	SPDR	= 0x2e
.SET	SPSR	= 0x2d
.SET	SPCR	= 0x2c
.SET	GPIOR2= 0x20
.SET	GPIOR1= 0x20
.SET	OCR0B	= 0x20
.SET	OCR0A	= 0x20
.SET	TCNT0	= 0x20
.SET	TCCR0B= 0x20
.SET	TCCR0A= 0x20
.SET	GTCCR	= 0x20
.SET	EEARH	= 0x20
.SET	EEARL	= 0x20
.SET	EEDR	= 0x20
.SET	EECR	= 0x1f
.SET	GPIOR0= 0x10
.SET	EIMSK	= 0x10
.SET	EIFR	= 0x1c
.SET	PCIFR	= 0x10
.SET	TIFR2	= 0x10
.SET	TIFR1	= 0x10
.SET	TIFR0	= 0x10
.SET	PORTD	= 0x00
.SET	DDRD	= 0x0a
.SET	PIND	= 0x09
.SET	PORTC	= 0x00
.SET	DDRC	= 0x07
.SET	PINC	= 0x06
.SET	PORTB	= 0x00
.SET	DDRB	= 0x04
.SET	PINB	= 0x03
.ENDM
.MACRO IOMEMUNSET
.SET	SREG   = 0x3f + 0x20
.SET	SPL    = 0x3d + 0x20
.SET	SPH    = 0x3e + 0x20
.SET	SPMCSR = 0x37 + 0x20
.SET	MCUCR  = 0x35 + 0x20
.SET	MCUSR  = 0x34 + 0x20
.SET	SMCR   = 0x33 + 0x20
.SET	ACSR   = 0x30 + 0x20
.SET	SPDR   = 0x2e + 0x20
.SET	SPSR   = 0x2d + 0x20
.SET	SPCR   = 0x2c + 0x20
.SET	GPIOR2 = 0x2b + 0x20
.SET	GPIOR1 = 0x2a + 0x20
.SET	OCR0B  = 0x28 + 0x20
.SET	OCR0A  = 0x27 + 0x20
.SET	TCNT0  = 0x26 + 0x20
.SET	TCCR0B = 0x25 + 0x20
.SET	TCCR0A = 0x24 + 0x20
.SET	GTCCR  = 0x23 + 0x20
.SET	EEARH  = 0x22 + 0x20
.SET	EEARL  = 0x21 + 0x20
.SET	EEDR   = 0x20 + 0x20
.SET	EECR   = 0x1f + 0x20
.SET	GPIOR0 = 0x1e + 0x20
.SET	EIMSK  = 0x1d + 0x20
.SET	EIFR   = 0x1c + 0x20
.SET	PCIFR  = 0x1b + 0x20
.SET	TIFR2  = 0x17 + 0x20
.SET	TIFR1  = 0x16 + 0x20
.SET	TIFR0  = 0x15 + 0x20
.SET	PORTD  = 0x0b + 0x20
.SET	DDRD   = 0x0a + 0x20
.SET	PIND   = 0x09 + 0x20
.SET	PORTC  = 0x08 + 0x20
.SET	DDRC   = 0x07 + 0x20
.SET	PINC   = 0x06 + 0x20
.SET	PORTB  = 0x05 + 0x20
.SET	DDRB   = 0x04 + 0x20
.SET	PINB   = 0x03 + 0x20
.ENDM

;.SET EECR = 0x1f
.MACRO BSET1			; BIT SET @0=ADR, @1=BIT index, @2 REGTMP[16,31]
	.IF @1>7
		.message "Only values 0-7 allowed for Bit parameter"
	.ENDIF
;	.IF @0<=15								;   ; RL [0,D15=$0F]
;		SBI @0, @1							; 2c;
;	.ELIF (@0>=16)&&(@0<=31)	;   ; RH [D16=$10,D31=$1F]
;		SBR @0, (1<<@1)					; 1c;
	.IF (@0>=32)&&(@0<=63)	;   ; SFRL [D32=$20,D63=$3F],IO[0,D31=$1F]
;        .SET EECR = 0x10
;        IOMEMSET
    ;.SET ADR = @0-20
;    .SET ADR = ADR-20
		SBI (@0-20), @1							; 2c;
		;SBI DDRB, @1							; 2c;
;        IOMEMUNSET
  .ELIF @0>=64
;  .ELSE
;        IOMEMUNSET
        LDS R16, @0
        SBR  r16,@1
        STS @0, r16
  .ENDIF
.ENDM
