.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
.INCLUDE "AVR_MACRO_MISC1.INC"
;*******************************************************************************
.DSEG                   ; allocate data space for FIFO + meta data
.EQU FIFO_SIZE    = 255 ; [1,256] max=256
A_FIFO_BASE_RX:  .BYTE (FIFO_SIZE+3)
A_FIFO_BASE_TX:  .BYTE (FIFO_SIZE+3)
;*******************************************************************************
.CSEG
;-------------------------------------------------------------------------------
.ORG $0  ; RESET JP
    RJMP INIT
;-------------------------------------------------------------------------------
.ORG URXCaddr
    RJMP ISR_URXC
.ORG UDREaddr
    RJMP ISR_UDRE
.ORG  OC0Aaddr
    RJMP ISR_OC0A
;-------------------------------------------------------------------------------
.ORG INT_VECTORS_SIZE ; first usable CSEG
;*******************************************************************************
INIT:
        INIT_SP
        INIT_UART
;-------------------------------------------------------------------------------
        ; init TC0
        CFG_SET_TCN_CTLRNAME_MODN 0,WGM,0b010     ; config WGM, 010-CTC-OCR0A
        CFG_SET_TCN_CTLRNAME_MODN 0,CS,0b101      ; config CS, 101-1024
        CFG_SET_IOREGNAME_1_REGBIT TIMSK0,OCIE0A  ; config COMIE0A
        RAMWRAIR OCR0A,156,R16                    ; write OCR0A, 156 -> 0.01 s
        ;RAMWRAIR OCR0B,0,R16                      ; write OCR0B
        CFG_WR_IOREGNAME_TRH TIMSK0,R16           ; write TIMSK0
        CFG_WR_IOREGNAME_TRH TCCR0A,R16           ; write TCCR0A
        CFG_WR_IOREGNAME_TRH TCCR0B,R16           ; write TCCR0B, now start
        ; end init TC0
;-------------------------------------------------------------------------------
        SEI
;-------------------------------------------------------------------------------
        ; set LED
        SETB DDRB,DDB5        ; config PB5 as output
;*******************************************************************************
MAIN:
TJP_END:
    RJMP TJP_END
;-------------------------------------------------------------------------------
.INCLUDE "007.BUF_FIFO_V2_MAX255.INC"
.INCLUDE "DELAY_ASM_dxp.pl_2008_busy-delay-assembly-utilities.inc"
;-------------------------------------------------------------------------------
;*******************************************************************************
SR_UART_RX_FIFO_RD_R17_RhT16_PZ:
        PLD_0A1P A_FIFO_BASE_RX,Z
        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
        TST R16                       ; check if null
        BREQ JP_SR_UART_RX_FIFO_RD_R17_RhT16_PZ_NUL_EXIT ; nul exit
        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX255  ; not nul, read out into R17
  JP_SR_UART_RX_FIFO_RD_R17_RhT16_PZ_CHK_FUL_TRUE_SET_RXCIE0:
        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
        CPI R16,FIFO_SIZE-1             ; check full (size-1)
        BRNE JP_SR_UART_RX_FIFO_RD_R17_RhT16_PZ_EXIT
        LDS R16, UCSR0B                       ; enable RXCIE0
        SBR R16, (1<<RXCIE0)
        STS UCSR0B, R16
  JP_SR_UART_RX_FIFO_RD_R17_RhT16_PZ_NUL_EXIT:
  JP_SR_UART_RX_FIFO_RD_R17_RhT16_PZ_EXIT:
        RET
;*******************************************************************************
SR_UART_TX_FIFO_WR_R17_RhT16_PZ:
        PLD_0A1P A_FIFO_BASE_TX,Z
        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
        CPI R16,FIFO_SIZE             ; check full (size)
        BREQ JP_SR_UART_TX_FIFO_WR_R17_RhT16_PZ_FUL_EXIT ; ful exit
        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX255  ; not ful, write R17 to FIFO
  JP_SR_UART_TX_FIFO_WR_R17_RhT16_PZ_CHK_NUL_TRUE_SET_UDRIE0:
        LDD R16,Z+D_FIFO_LEN          ; load FIFO_LEN value into R16
        CPI R16, 1                    ; check if null (0+1)
        BRNE JP_SR_UART_TX_FIFO_WR_R17_RhT16_PZ_EXIT
        LDS R16, UCSR0B                       ; enable UDRIE0
        SBR R16, (1<<UDRIE0)
        STS UCSR0B, R16
  JP_SR_UART_TX_FIFO_WR_R17_RhT16_PZ_FUL_EXIT:
  JP_SR_UART_TX_FIFO_WR_R17_RhT16_PZ_EXIT:
        RET
;*******************************************************************************
ISR_URXC:
        PUSH R16
        PUSHSREG_0RT R16
        PUSHP_0P Z
  JP_ISR_URXC_FIFO_CHK_FUL:
        PLD_0A1P  A_FIFO_BASE_RX,Z
        LDD R16,Z+D_FIFO_LEN            ; load FIFO_LEN value into R16
        CPI R16,FIFO_SIZE               ; check if full
        BREQ JP_ISR_URXC_CLR_RXCIE0     ; if full, disable RXCIE0
  JP_ISR_URXC_FIFO_WR:
        PUSH R17
        LDS R17, UDR0
        RCALL SR_FIFO_WR_R17_Rh16T_PZ_MAX255
        POP R17
        RJMP JP_ISR_URXC_EXIT
  JP_ISR_URXC_CLR_RXCIE0:
        LDS R16, UCSR0B
        CBR R16, RXCIE0
        STS UCSR0B, R16
  JP_ISR_URXC_EXIT:
        POPP_0P Z
        POPSREG_0RT R16
        POP R16
        RETI
;*******************************************************************************
ISR_UDRE:
        PUSH R16
        PUSHSREG_0RT R16
        PUSHP_0P Z
  JP_ISR_UDRE_FIFO_CHK_NUL:
        PLD_0A1P  A_FIFO_BASE_TX,Z
        LDD R16,Z+D_FIFO_LEN            ; load FIFO_LEN value into R16
        TST R16                         ; check if null
        BREQ JP_ISR_UDRE_CLR_UDRIE0     ; if null, disable UDRIE0
  JP_ISR_UDRE_FIFO_RD:
        PUSH R17
        RCALL SR_FIFO_RD_R17_Rh16T_PZ_MAX255
        STS UDR0, R17
        POP R17
        RJMP JP_ISR_URXC_EXIT
  JP_ISR_UDRE_CLR_UDRIE0:
        LDS R16, UCSR0B                 ; disable UDRIE0
        CBR R16, UDRIE0
        STS UCSR0B, R16
  JP_ISR_UDRE_EXIT:
        POPP_0P Z
        POPSREG_0RT R16
        POP R16
        RETI
;*******************************************************************************
ISR_OC0A:
SR_CHK_T10MSCNT:
        .SET T10MSCNT=99      ; 1s = 100 x 0.01
        ;.SET T10MSCNT=200      ; 1s = 100 x 0.01
        TST R18
        BRBS SREG_Z, JP_RELOAD_T10MSCNT
        DEC R18
        RJMP JP_ISR_OC0A_EXIT
  JP_RELOAD_T10MSCNT:
        LDI R18,T10MSCNT
  JP_LEDINVERT:
        RAMBNOTABRH PORTB,PB5,R16
;-------------------------------------------------------------------------------
ISR_ICP1_CAL_PERIOD_JITTER_CORE_8BIT_SIMU:
.DEF RTminL = R20
.DEF RhICRL = R21
.DEF RhTNL  = R22
.DEF RhTJL  = R23
.DEF RhT1L  = R24
;-------------------------------------------------------------------------------
ICR8BIT_TN111_TJ0_ONLY_TN_NO_JITTER:
;    0N%,1N%,2N%,3N%,4N%,5N%,6N%,7N%,8N%,9N%,10N%,...
;.DB   0,111,222, 77,188, 43,154,  9,120,231, 86,$FF
 .DB $00,$6F,$DE,$4D,$BC,$2B,$9A,$09,$78,$E7,$56,$FF
;-------------------------------------------------------------------------------
; Tn=111,Tj=0, ICR_8BIT= N_delay + (n*111)%/256
; N_delay=11
ICR8BIT_TN111_TJ0_ONLY_TN_NO_JITTER_ADD_DELAY:
;    0N%,1N%,2N%,3N%,4N%,5N%,6N%,7N%,8N%,9N%,10N%,...
;.DB   0,111,222, 77,188, 43,154,  9,120,231,    $FF
;.DB  11,122,233, 88,199, 54,165, 20,131,242, 97,$FF
 .DB $0B,$7A,$E9,$58,$C7,$36,$A5,$14,$83,$F2,$61,$FF
;-------------------------------------------------------------------------------
ICR8BIT_TN111_TJ0_TN_2TN_NO_JITTER:
.DB $00,$6F,$DE,$BC,$2B,$9A,$09,$78,$56,$34,$12,$81,$F0,$5F,$CE,$AC,$1B,$FF
;-------------------------------------------------------------------------------
        LDI R21,0
        LDI R22,0
        LDI R23,0
  JP_LD_Tmin:
        LDI   RTminL, 111
  JP_LD_ICR_PZ:   ; load PZ for LPM to get ICR, only do once for each 1s
;-------------------------------------------------------------------------------
;        LDI   ZH, HIGH(ICR8BIT_TN111_TJ0_ONLY_TN_NO_JITTER<<1)
;        LDI   ZL,  LOW(ICR8BIT_TN111_TJ0_ONLY_TN_NO_JITTER<<1)
;-------------------------------------------------------------------------------
        LDI   ZH, HIGH(ICR8BIT_TN111_TJ0_TN_2TN_NO_JITTER<<1)
        LDI   ZL,  LOW(ICR8BIT_TN111_TJ0_TN_2TN_NO_JITTER<<1)
;-------------------------------------------------------------------------------
  JP_ISR_ICP1_IN_ICR_OUT_TN_TJ:   ; in ICR and out TN/TJ, loop scan
  JP_MOV_TN_TJ:
        MOV   RhTJL, RhTNL        ; set Tn-1 (Tn) in TJ to cal TJ
        MOV   RhTNL, RhICRL       ; set ICRn-1 (Tn) in TN to cal TN
  JP_LD_ICR:
        LPM   RhICRL, Z+
  JP_CHK_ICR_FF_TRUE_EXIT:
        CLZ
        CPI   RhICRL, $FF
        BREQ JP_ISR_ICP1_CAL_PERIOD_JITTER_CORE_8BIT_SIMU_EXIT
        ;BREQ JP_ISR_OC0A_EXIT
  JP_SAVE_PT_TO_PX:
        CopyZtoX
  JP_CAL_TN:
        NEG   RhTNL
        ADD   RhTNL, RhICRL
  JP_CAL_TN_EXIT:
  JP_CAL_TJ:
;        CP    RhTJL, RhTNL
;        BRLO  JP_CAL_TJ_GET_ABS ; if C=0 BRSH, TJ>=TN, if C=1 BRLO, TJ<TN
        NEG   RhTJL
        ADD   RhTJL, RhTNL
;        RJMP  JP_CAL_TJ_EXIT
;  JP_CAL_TJ_GET_ABS:
;        SUB   RhTJL, RhTNL
;        COM   RhTJL
;        INC   RhTJL
  JP_CAL_TJ_EXIT:
        LDI   R17,$F0
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ

        MOV   R17,RhICRL
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ

        LDI   R17,$F1
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ

        MOV   R17,RhTNL
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ

        LDI   R17,$F2
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ

        MOV   R17,RhTJL
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ

        RCALL SR_MANCODE_DIFFMAN_DECODE_CORE_8BIT_SIMU
        
        RCALL SR_OUT_CRLF
        CopyXtoZ
        RJMP JP_ISR_ICP1_IN_ICR_OUT_TN_TJ

;-------------------------------------------------------------------------------
; test
;        LDI   ZH,   HIGH(ASCII_TABLE<<1)
;        LDI   ZL,    LOW(ASCII_TABLE<<1)
;TJP_LPM_1:
;        ;---------- just output 0
;        ;LPM   R17,  Z
;        ;RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
;        ;---------- loop scan
;        LPM   R17,  Z+
;        CLZ
;        TST   R17
;        BREQ  JP_ISR_OC0A_EXIT
;        CopyZtoX
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
;        CopyXtoZ
;        RJMP  TJP_LPM_1
;-------------------------------------------------------------------------------
;ASCII_TABLE:
;        .DB '0','1','2','3','4','5','6','7','8','9',$0D,$0A,0
;-------------------------------------------------------------------------------
;-------------------------------------------------------------------------------
;        TST R21
;        BREQ JP_RD_ONE_CHAR
;  JP_WR_ONE_CHAR:
;        MOV R17,R20
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
;        LDI  R21,0
;        RJMP JP_ISR_OC0A_EXIT
;  JP_RD_ONE_CHAR:
;        RCALL SR_UART_RX_FIFO_RD_R17_RhT16_PZ   ; read one from rx_fifo
;        MOV R20,R17   ; copy R17 input to R20
;        LDI R21,1
;-------------------------------------------------------------------------------
; test
; D[33,126] can display char set, delay 1 sec and output them
; test1, input the char into TX FIFO
;        LDI R17, 32
;JP_LOOP_ASCII:
;        INC R17
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;        CLZ
;        CPI R17,126
;        BRNE JP_LOOP_ASCII
;
;        LDI R17, $0D ; CR
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;        LDI R17, $0A ; LF
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;-------------------------------------------------------------------------------
; test to output #\r\n
;        RJMP TJP1_LOOP0
;  JP_TX:
;        LDI R17, '#' ; $23
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;;        LDI R17, $20 ; SP
;;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;        LDI R17, $0D ; CR
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;        LDI R17, $0A ; LF
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;;        LDI R17, $0D ; CR
;;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;;        LDI R17, $0A ; LF
;;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;-------------------------------------------------------------------------------
  JP_ISR_OC0A_EXIT:
  JP_ISR_ICP1_CAL_PERIOD_JITTER_CORE_8BIT_SIMU_EXIT:
        RCALL SR_OUT_CRLF
;-------------------------------------------------------------------------------
        RETI
;*******************************************************************************
SR_MANCODE_DIFFMAN_DECODE_CORE_8BIT_SIMU:
        TST   RhTJL
        BRNE  JP_SR_MAN_DECODE_1x5BITWIDTH_DECODE
        CP    RhTNL,  RTminL
        BRNE  JP_SR_MAN_DECODE_2xBITWIDTH_DECODE
        BRTS  JP_SR_MAN_DECODE_CLRT_SKIP_OUT
  JP_SR_MAN_DECODE_SET_OUT_0:
        SET
        RCALL SR_OUT_ASCII_0          ; output 0
        RJMP  JP_SR_MAN_DECODE_EXIT
  JP_SR_MAN_DECODE_CLRT_SKIP_OUT:
        CLT
        RJMP  JP_SR_MAN_DECODE_EXIT
  JP_SR_MAN_DECODE_2xBITWIDTH_DECODE:
        MOV   RhT1L,  RhTNL
        LSR   RhT1L
        CP    RhT1L,  RTminL
        BRNE  JP_SR_MAN_DECODE_ERR_EXIT
        RCALL SR_OUT_ASCII_1          ; output 1
        RJMP  JP_SR_MAN_DECODE_EXIT
  JP_SR_MAN_DECODE_1x5BITWIDTH_DECODE:
        CP    RhTJL,  RTminL
        BRNE  JP_SR_MAN_DECODE_ERR_EXIT
        CLT
        RCALL SR_OUT_ASCII_1          ; output 1
  JP_SR_MAN_DECODE_ERR_EXIT:
  JP_SR_MAN_DECODE_EXIT:
        RET
;-------------------------------------------------------------------------------
; 01-00-111-001
;[f0][00][f1][01][f2][01]
;[f0][6f][f1][6f][f2][6e]
;[f0][de][f1][6f][f2][00][30]
;[f0][bc][f1][de][f2][6f][31]
;[f0][2b][f1][6f][f2][91]
;[f0][9a][f1][6f][f2][00][30]
;[f0][09][f1][6f][f2][00]
;[f0][78][f1][6f][f2][00][30]
;[f0][56][f1][de][f2][6f][31]
;[f0][34][f1][de][f2][00][31]
;[f0][12][f1][de][f2][00][31]
;[f0][81][f1][6f][f2][91]
;[f0][f0][f1][6f][f2][00][30]
;[f0][5f][f1][6f][f2][00]
;[f0][ce][f1][6f][f2][00][30]
;[f0][ac][f1][de][f2][6f][31]
;[f0][1b][f1][6f][f2][91]
;*******************************************************************************
SR_OUT_CRLF:  ; append CR/LF
        PUSH R17
        PUSHP_0P Z
        LDI R17, $0D ; CR
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
        LDI R17, $0A ; LF
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
        POPP_0P Z
        POP R17
        RET
SR_OUT_CR:  ; append CR/LF
        PUSH R17
        PUSHP_0P Z
        LDI R17, $0D ; CR
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
        POPP_0P Z
        POP R17
        RET
SR_OUT_LF:  ; append CR/LF
        PUSH R17
        PUSHP_0P Z
        LDI R17, $0A ; LF
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
        POPP_0P Z
        POP R17
        RET
;-------------------------------------------------------------------------------
SR_OUT_ASCII_0:
        PUSH R17
        PUSHP_0P Z
        LDI   R17, $30 ; '0'
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
        POPP_0P Z
        POP R17
        RET
SR_OUT_ASCII_1:
        PUSH R17
        PUSHP_0P Z
        LDI   R17, $31 ; '1'
        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
        POPP_0P Z
        POP R17
        RET
;-------------------------------------------------------------------------------
; TJP2: ; fill tx buffer 256 x 'Y' then delay 10s and SEI send out
; NOP
; NOP
; NOP
; NOP
; NOP
;TJP2_LOOP_FILL_Y:
;     ;LDI R19, 85
;     LDI R17, '#'
;     RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;     LDI R17, $0D
;     RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;     LDI R17, $0A
;     RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;     ;BRNE TJP2_LOOP_FILL_Y
;
;     SEI
;     LDI R16, 1
;     RCALL WaitSecLoop
;     RJMP TJP2_LOOP_FILL_Y
; NOP
; NOP
; NOP
; NOP
;     RJMP TJP_END
;-------------------------------------------------------------------------------
;    LDI R17, 'E'
;    RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;    LDI R17, 'S'
;    RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;    LDI R17, $0D
;    RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;    LDI R17, $0A
;    RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ
;
;    RJMP TP2
;-------------------------------------------------------------------------------
; echo test
; tty input/write 'Y''E''S' '\r[$0D/CR]' '\n[$0A/LF]'
; loop check if there '\n', output buffer
;TJP1: ; delay 10s to sei
;        LDI R16, 1
;        RCALL WaitSecLoop
;        SEI
;        CLR R17
;        LDI R17,'#'
;TJP1_LOOP0:
;        ;RCALL SR_UART_RX_FIFO_RD_R17_RhT16_PZ   ; read one from rx_fifo
;        ;CLC
;        ;TST R17
;        ;BREQ TJP1_LOOP0
;        ;RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
;        ;RJMP TJP1
;        ;RJMP TJP1_LOOP0
;
;TJP1_LOOP1:  ; read rx len >0
;        PLD_0A1P A_FIFO_BASE_RX,Z
;        LDD R19,Z+D_FIFO_LEN ; read len = R19
;        CLC
;        TST R19
;;        BREQ TJP1_LOOP1 ; if null, loop
;        BREQ JP_ISR_OC0A_EXIT
;
;        LDI R16, 0
;TJP1_LOOP2: ; read rx_fifo into R17, scan rx_fifo have '\n'
;        CLC
;        CPSE R16,R19
;        RJMP JP_ISR_OC0A_EXIT
;        
;        RCALL SR_FIFO_SCAN_RD_IN_R16_DATA_INDEX_OUT_R17_PZ
;        CLC
;        CPI R17, $0A
;        BREQ TJP1_RD_OUT_BUF
;        INC R16
;        RJMP TJP1_LOOP2   ; if scan complete, R16=R19=len
;;        RJMP TJP1_LOOP1   ; if scan not finish, R16<R19, scan next 
;
;TJP1_RD_OUT_BUF: ; have '\n', send to tx_fifo
;        RCALL SR_UART_RX_FIFO_RD_R17_RhT16_PZ   ; read one from rx_fifo
;        RCALL SR_UART_TX_FIFO_WR_R17_RhT16_PZ   ; write one to tx_fifo
;        CLC
;        DEC R19
;        BRNE TJP1_RD_OUT_BUF      ; send rest
;;        RJMP TJP1_LOOP1           ; read and send all, back to read len >0
;        RJMP JP_ISR_OC0A_EXIT
;-------------------------------------------------------------------------------
