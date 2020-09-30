.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
; software FIFO buffer implement from:
; BUF_FIFO_UART_ASM_avrbeginners_software_UART_Buffer_uart_buffer.md
; http://www.avrbeginners.net/architecture/uart/uart_buffer.html
.equ rx_size = 255               ; [1,255] max= 255
.dseg                           ; space for the buffer
rx_n:     .byte 1               ; [0,255]
rx_in:    .byte 2               ; 2 x Bytes for write/in ptr adr [LOW]:[HIGH]
rx_out:   .byte 2
rx_fifo:  .byte rx_size         ; Memory Address FIFO
;rx_fifo_test: .byte rx_size     ; test space for read
.cseg
;.org 0x0000
;        rjmp TJP1
init_FIFO:  ; RhT R16,R17, meta data (fifo/in/out/n) address
        ldi r16, low(rx_fifo)        ;r16:r17 = rx_fifo base address
        ldi r17, high(rx_fifo)
        sts rx_in, r16            ;rx_in = rx_fifo base address
        sts rx_in + 1, r17
        sts rx_out, r16            ;rx_out = rx_fifo base address
        sts rx_out + 1, r17
        clr r16                ;rx_n = 0
        sts rx_n, r16
        ret
rx_fifo_store_chk_full_false_write: ; r17 REGT
        lds r17, rx_n
        cpi r17, rx_size
        brlo rx_fifo_store  ;space available, go on and store
        ret
rx_fifo_store: ; PX, r16,r17, move data into r16 for write into buffer
        ;in r16, UDR            ;get received data
        lds XL, rx_in            ;X = rx_in
        lds XH, rx_in + 1
        st X+, r16            ;X+ = data
        ;r16:r17 = first invalid address above rx FIFO memory space
        ldi r16,  low(rx_fifo + rx_size)
        ldi r17, high(rx_fifo + rx_size)
        cp  XL, r16            ;r16:r17 = X?
        cpc XH, r17
        breq rx_fifo_w_rollover        ;yes? roll over
    rx_fifo_w_store:        ;store pointer (rx_in)
        sts rx_in,     XL
        sts rx_in + 1, XH
        lds r16, rx_n            ;increase number of bytes in the buffer
        inc r16
        sts rx_n, r16
        ret
    rx_fifo_w_rollover:        ;roll over to base address:
        ldi XL,  low(rx_fifo)        ;X = rx_fifo base address
        ldi XH, high(rx_fifo)
        rjmp rx_fifo_w_store;go to the point where the ptr is stored and proceed
;*******************************************************************************
rx_fifo_read_chk_null_false_read:  ; PX, r16/r17 REGT, r18 read out
        lds r16, rx_n            ;data available?
        cpi r16, 1
        brsh rx_fifo_read        ;if more than zero bytes available, read data
        ret                ;else return
rx_fifo_read:
        lds XL, rx_out            ;X = rx_out pointer
        lds XH, rx_out + 1
        ld  r18, X+            ;r18 = X+
        ldi r16,  low(rx_fifo + rx_size)
        ldi r17, high(rx_fifo + rx_size)
        cp  r16, XL            ;r16:r17 = X?
        cpc r17, XH
        breq rx_fifo_r_rollover        ;yes? roll over
    rx_fifo_r_store:
        sts rx_out,     XL        ;same as in rx_fifo_store
        sts rx_out + 1, XH        ;save the pointer
        lds r16, rx_n       ;and this time decrease the number of bytes in the
        dec r16                ;FIFO (we read one byte)
        sts rx_n, r16
        ret                       ;and return to app r18 = data
    rx_fifo_r_rollover:
        ldi XL,  low(rx_fifo)        ;X = rx fifo base address
        ldi XH, high(rx_fifo)
        rjmp rx_fifo_r_store

;TJP1:
;        RCALL init_FIFO
;        LDI R19, 255
;TJP1_LOOP:
;        MOV R16, R19
;        RCALL rx_fifo_store_chk_full_false_write
;        DEC R19 
;        BRNE TJP1_LOOP
;
;TJP2:
;        LDI R19, 255
;        PLD_0A1P rx_fifo_test,Z 
;TJP2_LOOP:
;        RCALL rx_fifo_read_chk_null_false_read
;        ST  Z+, R18
;        DEC R19
;        BRNE TJP2_LOOP
;        LDI R19, 255

;TJP3:
;        LDI R19, 255
;TJP3_LOOP:
;        MOV R16, R19
;        RCALL rx_fifo_store_chk_full_false_write
;        DEC R19 
;        BRNE TJP3_LOOP
;
;TJP4:
;        LDI R19, 100
;        PLD_0A1P rx_fifo_test,Z 
;TJP4_LOOP:
;        RCALL rx_fifo_read_chk_null_false_read
;        ST  Z+, R18
;        DEC R19
;        BRNE TJP4_LOOP
;
;TJP5:
;        LDI R19, 100
;TJP5_LOOP:
;        ;MOV R16, R19
;        LDI R16, $FF
;        RCALL rx_fifo_store_chk_full_false_write
;        DEC R19 
;        BRNE TJP5_LOOP
;TJP6:
;        LDI R19, 255
;        ;PLD_0A1P rx_fifo_test,Z 
;TJP6_LOOP:
;        RCALL rx_fifo_read_chk_null_false_read
;        ST  Z+, R18
;        DEC R19
;        BRNE TJP6_LOOP
;
;TJP_END:
;        RJMP TJP_END