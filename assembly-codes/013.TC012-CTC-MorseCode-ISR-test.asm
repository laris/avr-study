.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"


INIT:
        INIT_SP
        ; init IO PIN
        SETB DDRB,DDB5        ; config PB5 as outpu

.EQU  MSG_ADR = 0x100
.DEF  RH_BUFF = R16
      LDI   RH_BUFF,0b10101010
      ;LDI   RH_BUFF,$FF
      STS   MSG_ADR,RH_BUFF
.DEF  RH_CNT  = R17
      LDI   RH_CNT, 0
.EQU  TXPORT  = PORTB
.EQU  TXPIN  = PORTB5

.DEF LOOP_CNT=R18
.SET T0_10MS_CNT= 20                       ; 1s = 100 x 0.01
      LDI LOOP_CNT,T0_10MS_CNT
.DEF  REGTMP = R19

;;SR_CHK_T0_10MS_CNT:
;;        TST LOOP_CNT
;;        BRBS SREG_Z, JP_RELOAD_T0_10MS_CNT
;;        DEC LOOP_CNT
;;        RJMP JP_ISR_OC0A_EXIT
;;  JP_RELOAD_T0_10MS_CNT:
;;        LDI LOOP_CNT,T0_10MS_CNT
;  JP_LEDINVERT_0:
;        RAMBNOTABRH PORTB,PB5,REGTMP
;  JP_ISR_OC0A_EXIT:
;        RETI

;ISR_OC0A:
  JP_TX_INIT:
  JP_CHK_BUFF: 
        TST   RH_CNT                                    ;1c
        BRNE  JP_SHIFT_BYTE_RIGHT_FIRST_LSB             ;1/2c 
  JP_TX_LOAD_NEW_BYTE:
        ;RAMCOPY0PXYZINC1A2R Z,MSG_ADR,RH_BUFF
        RAMCOPY0PXYZ1A2R Z,MSG_ADR,RH_BUFF              ; 2LDI+1LD 2+1=3
        ;LDI   RH_CNT,  0b10000000 ; R17 ------- 1      ; 
        LDI RH_CNT,8                    ; ----------2   ; 1c
  JP_SHIFT_BYTE_RIGHT_FIRST_LSB:
        ROR   RH_BUFF ; R16                             ; 1c
  JP_CFLG2PORT:
        BRCC  PC+0x3                                    ; 1/2c
        SBI   TXPORT, TXPIN                            ; 2c
        RJMP  PC+0x2                                    ; 2c
        CBI   TXPORT, TXPIN                            ; 2c
  JP_SHIFT_BYTE_RIGHT_FIRST_LSB_DEC_CNT:
        DEC   RH_CNT                    ;----------2    ; 1c
        ;CLC                            ;--------1
        ;ROR   RH_CNT ;R17              ;--------1
  JP_TX_EXIT:
  JP_ISR_OC0A_EXIT:

; max 13 cycle
        ; test
        RJMP JP_TX_INIT

