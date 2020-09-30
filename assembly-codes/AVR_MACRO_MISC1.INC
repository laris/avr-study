;*******************************************************************************
.MACRO SREG_CFLG2PORT_0PADR_1PBIT
      ; Convert SREG-C flag status to PORT IO [IOSFRL] status, max 5 cycle
        BRCC  PC+0x3              ;1/2c; C 0 = PIN OUT 0
        SBI   @0, @1              ;1c  ; C 1 = PIN OUT 1
        RJMP  PC+0x2              ;2c  ; if 1, cost brcc 1 + sbi 2 + rjmp 2 = 5
        CBI   @0, @1              ;1c  ; if 0, cost brcc 2 + cbi 2 = 4
.ENDMACRO
;-------------------------------------------------------------------------------
; PASS test for macro
;TJP_SREG_CFLG2PORT_0PADR_1PBIT:
;        SEC
;        SREG_CFLG2PORT_0PADR_1PBIT PORTB,PORTB0
;        CLC
;        SREG_CFLG2PORT_0PADR_1PBIT PORTB,PORTB0
;        RJMP TJP_SREG_CFLG2PORT_0PADR_1PBIT
;*******************************************************************************
.MACRO ROR8X_0RhBYTE_1RhCNT ; use reg as counter way
      ; 0RhBYTE keep orig, 1RhCNT clear
        LDI   @1,  8                ;1c ;
        ROR   @0                    ;1c ;
        DEC   @1                    ;1c ;
        BRNE  PC-0x02               ;1/2;
.ENDMACRO
.MACRO ROR8X2_0RhBYTE_1RhCNT ; use SREG-C as counter flag
      ; 0RhBYTE keep orig, 1RhCNT keep orig, 25 cycle
        LDI   @1,  0b10000000       ;1c ;
        ROR   @0                    ;1c ;
        ROR   @1                    ;1c ;
        BRCC  PC-0x02               ;1/2;
.ENDMACRO
.MACRO ROL8X_0RhBYTE_1RhCNT
      ; 0RhBYTE keep orig, 1RhCNT clear
        LDI   @1,  8
        ROL   @0
        DEC   @1
        BRCC  PC-0x02
.ENDMACRO
.MACRO ROL8X2_0RhBYTE_1RhCNT
      ; 0RhBYTE set 0, 1RhCNT save 0RhBYTE value, 25 cycle
        LDI   @1,  0b00000001  
        ROL   @0
        ROL   @1
        BRCC  PC-0x02
.ENDMACRO
;-------------------------------------------------------------------------------
; PASS test for macro
;.DEF    RhBYTE  = R16
;.DEF    RhCNT   = R17
;;       shift LSB first
;        LDI   RhBYTE,  0b10101010
;        ROR8X_0RhBYTE_1RhCNT RhBYTE,RhCNT
;        PUSH RhBYTE
;        PUSH RhCNT
;;       shift MSB first
;        LDI   RhBYTE,  0b10101010
;        ROL8X_0RhBYTE_1RhCNT RhBYTE,RhCNT
;        PUSH RhBYTE
;        PUSH RhCNT
;*******************************************************************************
.MACRO SERIALIZE_BYTE2PORT_0RhBYTE_1RhCNT_2PADR_3PBIT
        LDI   @1,  8
  JP_LOOP_SERIALIZE_BYTE2PORT_0RhBYTE_1RhCNT_2PADR_3PBIT:
        ROL   @0
        DEC   @1
  JP_BYTE2PORT_SERIALIZE_BYTE2PORT_0RhBYTE_1RhCNT_2PADR_3PBIT:
        SREG_CFLG2PORT_0PADR_1PBIT @2,@3
        BRCC  JP_LOOP_SERIALIZE_BYTE2PORT_0RhBYTE_1RhCNT_2PADR_3PBIT
.ENDMACRO
;-------------------------------------------------------------------------------
;        LDI   R16,0b10101010
;        SERIALIZE_BYTE2PORT_0RhBYTE_1RhCNT_2PADR_3PBIT R16,R17,PORTB,PORTB5
;*******************************************************************************
;* https://www.avrfreaks.net/forum/16bit-compare
;* https://www.avrfreaks.net/forum/using-cpc-command-16bit-number-using-assembly
;* "cpi16" - Comparing 16-bit register with 16-bit immediate 
;* This example compares the register pairs (cpi1l,cpi1h) with the value
;* cpi2.  If they are equal the zero flag is set(one), otherwise it is 
;* cleared(zero). This is enabled by the AVR's zero propagation. Carry is
;* also set if the result is negative. This means that all conditional
;* branch instructions can be used after the comparison. 
;* Number of words    :3
;* Number of cycles    :3
;* Low registers used    :None
;* High registers used    :3
;***** Register Variables
;.def cp1l =r16
;.def cp1h =r17
;.def c_tmp=r18 
;.equ cp2 = 0x3412        ;Immediate to compare with
;***** Code
;       cpi16:  cpi    cp1l,low(cp2)    ;Compare low byte
;       ldi     c_tmp,high(cp2)    ;
;       cpc     cp1h,c_tmp    ;Compare high byte
;Expected result is Z=1, C=0/1(-result)
;*******************************************************************************
.MACRO CPI16_01RHL2KI3RhT
      ;CPI16 @0=UH, @1=UL, @2=U-immediate, @3=Rh Temp
        CPI   @1, LOW(@2)
        LDI   @3,HIGH(@2)
        CPC   @0, @3
.ENDMACRO
.MACRO CPI16_01RHL2KI3RhTPP
      ;CPI16 @0=UH, @1=UL, @2=U-immediate, @3=Rh Temp(PushPop)
        PUSH  @3
        CPI   @1, LOW(@2)
        LDI   @3,HIGH(@2)
        CPC   @0, @3
        POP   @3
.ENDMACRO
.MACRO CPI16_01RHL2KI_R16PP
      ;CPI16 @0=UH, @1=UL, @2=U-immediate, Rh=R16 as Temp(PushPop)
        PUSH  R16
        CPI   @1, LOW(@2)
        LDI   R16,HIGH(@2)
        CPC   @0, R16
        POP   R16
.ENDMACRO
.MACRO CP16_01RHL23RHL
      ;CP16 UH,UL,VH,VL (U-V)
        CP    @1, @3
        CPC   @0, @2
.ENDMACRO
.MACRO CP24_012RHML345RHML
      ;CP16 UH,UM,UL,VH,VM,VL (U-V)
        CP  @2, @5
        CPC @1, @4
        CPC @0, @3
.ENDMACRO
;*******************************************************************************
.MACRO COM16_01RHL  ; COM16 UH,UL
        COM @0
        COM @1
.ENDMACRO
;*******************************************************************************
.MACRO NEG16_01RHHL ; NEG16 @0=REGH[Unsigned]H, @1=REGH[Unsigned]L
        COM   @1              ;1 ;
        COM   @0              ;1 ;
        SUBI  @1,  LOW(-1)    ;1 ;  LOW(-1)=$FF
        SBCI  @0, HIGH(-1)    ;1 ; HIGH(-1)=$FF
.ENDMACRO
; NEG16 demo
;        257 ( )0000-0001 ( )0000-0001 
;com     257 ( )1111-1110 ( )1111-1110
;subi-1   -1    1111-1111    1111-1111
;       -257 ( )1111-1110 (b)1111-1111
;       +258 ( )0000-0001 ( )0000_0010
;-------------------------------------
;       =  1 (b)0000-0000 (b)0000-0001
;***********************************************************************
;16bit - from App note 202
.MACRO ADD16    ; Add16 @0=UH,@1=UL,@2=VH,@3=VL      U:=U+V
        add    @1,@3               ;1 ;
        adc    @0,@2               ;1 ;
.ENDMACRO
.MACRO ADD168   ; ADD 16bit + 8bit @0UH,@1=UL,@2=8BIT
        ADD   @1, @2
        CLR   @2
        ADC   @0, @2
.ENDMACRO
.MACRO SUB16    ; Sub16 UH,UL,VH,VL    U:=U-V
        sub    @1,@3               ;1 ;
        sbc    @0,@2               ;1 ;
.ENDMACRO
.MACRO SUB168   ; SUB 16bit + 8bit @0UH,@1=UL,@2=8BIT
        SUB   @1, @2
        CLR   @2
        SBC   @0, @2
.ENDMACRO
;*******************************************************************************
.MACRO SWAP8  ; Swap value in register (Assume: @0=a, @1=b)
;.MACRO m_SwapReg    ; Swap value in register (Assume: @0=a, @1=b)
        EOR @0, @1   ; @0 := @0 xor @1    (@0: a -> ab
        EOR @1, @0   ; @1 := @1 xor @0    (@1: b -> bab -> a)
        EOR @0, @1   ; @0 := @0 xor @1    (@0  ab -> aba ->b)
.ENDMACRO
.MACRO SWAP16_01XYZ
        EOR @1L, @0L  ; assembler does not allow using m_SwapReg in macro
        EOR @0L, @1L
        EOR @1L, @0L
        EOR @1H, @0H  ; assembler does not allow using m_SwapReg in macro
        EOR @0H, @1H
        EOR @1H, @0H
.ENDMACRO
.MACRO SWAP16       ; @01=RH:RL @23=RH:RL
         SWAP8 @1, @3
         SWAP8 @0, @2
;        EOR @3, @1  ; assembler does not allow using m_SwapReg in macro
;        EOR @1, @3
;        EOR @3, @1
;        EOR @2, @0 ; assembler does not allow using m_SwapReg in macro
;        EOR @0, @2
;        EOR @2, @0
.ENDMACRO
;*******************************************************************************
.MACRO LSR16 ; @0=RH, @1=RL
        LSR @0  ; shift RH[LSB] to C
        ROR @1  ; shift C (from RH-LSB) into RL[MSB]
.ENDMACRO
.MACRO LSL16 ; @0=RH, @1=RL
        LSL @1  ; shift RL[MSB] to C
        ROL @0  ; shift C (from RL-MSB) into RH[LSB]
.ENDMACRO
;*******************************************************************************
; 16 bit binary to 5 digit ASCII conversion 
; the 16 bit value is in bytes ZH & ZL 
; each of the 5 ascii digits is stored (sent) to a separate memory located (UART xmt buffer) 
; 10000 base 10=$2710, 1000 base 10 =$03E8, 100 base 10=0064, 10 base 10= $000A 
; D4/3/2/1ASCII can also be called to convert a 4/3/2/1 digit number to ASCII
.MACRO B2A
bin2ascii: ldi temp2, $2F ;Init ASCII conversion ('0'-1) 
loop10k: inc temp2      ;count up 10k "counts" 
         subi ZL, $10 
         sbci ZH, $27 
         brcc loop10k  ;IF >=10000 THEN subtract again 
         subi ZL, $F0  ;subtract -10000 (add 10000) 
         sbci ZH, $D8 
         rcall send_char    ;send 10k ASCII char

d4ascii: ldi temp2, $2F ;Init ASCII conversion ('0'-1) 
loop1k:  inc temp2      ;count up 1k "counts" 
         subi ZL, $E8 
         sbci ZH, $03 
         brcc loop1k   ;IF >=1000 THEN subtract again 
         subi ZL, $18  ;subtract -1000 (add 1000) 
         sbci ZH, $FC 
         rcall send_char

d3ascii: ldi temp2, $2F ;Init ASCII conversion ('0'-1) 
loop100: inc temp2      ;count up 100 "counts" 
         subi ZL, $64 
         sbci ZH, $00 
         brcc loop100  ;IF >=100 THEN subtract again 
         subi ZL, $9C  ;subtract -100 (add 100) 
         sbci ZH, $FF 
         rcall send_char
                ;(ZH==0) so ignore ZH from here 

d2ascii: ldi temp2, $2F ;Init ASCII conversion ('0'-1) 
loop10:  inc temp2      ;count up 10 "counts" 
         subi ZL, $0A
         brcc loop10   ;IF >=10 THEN subtract again 
         subi ZL, $F6  ;subtract -10 (add 10) 
         rcall send_char 
         mov temp2, ZL
d1ascii: subi temp2, $D0  ;convert 1's digit to ASCII 
         rcall send_char 
         ldi temp2, ' '
         rcall send_char  ;send out a space
         ret
;*******************************************************************************
.ENDMACRO
.MACRO ADDI16   ;Addi16    ; Addi16 UH,UL,immediate   U:=U+immediate
    subi    @1,  LOW(-@2)
    sbci    @0, HIGH(-@2)
.ENDMACRO
