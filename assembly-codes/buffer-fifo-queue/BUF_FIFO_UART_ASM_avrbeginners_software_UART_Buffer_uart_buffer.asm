; BUF_FIFO_UART_ASM_avrbeginners_software_UART_Buffer_uart_buffer.md
; http://www.avrbeginners.net/architecture/uart/uart_buffer.html
;.include "c:\programme\atmel\avr studio\appnotes\2313def.inc"
.equ rx_size = 16
.equ tx_size = 16
.dseg                ;space for the buffer
rx_fifo:  .byte rx_size        ;Rx FIFO
rx_in:    .byte 2
rx_out:   .byte 2
rx_n:     .byte 1
tx_fifo:  .byte tx_size        ;Tx FIFO
tx_in:    .byte 2
tx_out:   .byte 2
tx_n:     .byte 1
.cseg
.org 0x0000
        rjmp reset
.org 0x0007
        rjmp UART_RxC            ;Interrupt vectors for UART usage
        rjmp UART_UDRE
reset:
        ldi r16, low(RAMEND)        ;Stack setup
        out SPL, r16
        rcall init_fifos        ;FIFO setup
        ldi r16, 0b10011000        ;enable Rx, Tx and RxC int
        out UCR, r16
        ldi r16, 11            ;38400 baud @ 7.3728 MHz
        out UBRR, r16
        sei                ;enable ints
loop:
        lds r16, rx_n            ;r16 = rx_n
        tst r16                ;rx_n = 0?
        breq loop            ;yes, loop and check again
        rcall UART_read_fifo        ;rx_n > 0, read data from buffer
        rcall UART_write_fifo        ;write data to tx buffer
        rjmp loop            ;and loop
init_FIFOs:
        ldi r16, low(rx_fifo)        ;r16:r17 = rx_fifo base address
        ldi r17, high(rx_fifo)
        sts rx_in, r16            ;rx_in = rx_fifo base address
        sts rx_in + 1, r17
        sts rx_out, r16            ;rx_out = rx_fifo base address
        sts rx_out + 1, r17
        clr r16                ;rx_n = 0
        sts rx_n, r16
        ldi r16, low(tx_fifo)        ;same for tx FIFO
        ldi r17, high(tx_fifo)
        sts tx_in, r16
        sts tx_in + 1, r17
        sts tx_out, r16
        sts tx_out + 1, r17
        clr r16
        sts tx_n, r16
        ret
;*******************************************************************************
UART_RXC:            ;UART Rx Complete ISR
        push r16            ;save r16
        in r16, SREG            ;save SREG
        push r16
        lds r16, rx_n            ;rx_n < rx_size ?
        cpi r16, rx_size
        brlo rx_fifo_store        ;space available, go on and store
        in r16, UDR            ;not enough space, read data to clear INT
        pop r16                ;data is lost now
        out SREG, r16            ;restore SREG
        pop r16                ;restore r16
        reti                ;return from INT
rx_fifo_store:
        push r17            ;save r17
        push XL                ;and X
        push XH
        in r16, UDR            ;get received data
        lds XL, rx_in            ;X = rx_in
        lds XH, rx_in + 1
        st X+, r16            ;X+ = data
        ldi r16, low(rx_fifo + rx_size)    ;r16:r17 = first invalid address above rx FIFO
        ldi r17, high(rx_fifo + rx_size);memory space
        cp XL, r16            ;r16:r17 = X?
        cpc XH, r17
        breq rx_fifo_w_rollover        ;yes? roll over
rx_fifo_w_store:        ;store pointer (rx_in)
        sts rx_in, XL
        sts rx_in + 1, XH
        lds r16, rx_n            ;increase number of bytes in the buffer
        inc r16
        sts rx_n, r16
        pop XH                ;restore X
        pop XL
        pop r17                ;restore r17
        pop r16
        out SREG, r16            ;restore SREG
        pop r16                ;restore r16
        reti                ;return from INT
rx_fifo_w_rollover:        ;roll over to base address:
        ldi XL, low(rx_fifo)        ;X = rx_fifo base address
        ldi XH, high(rx_fifo)
        rjmp rx_fifo_w_store        ;go to the point where the pointer is stored and proceed
;*******************************************************************************
UART_read_fifo:            ;read data from the Rx FIFO
        lds r16, rx_n            ;data available?
        cpi r16, 1
        brsh rx_fifo_read        ;if more than zero bytes available, read data
        ret                ;else return
rx_fifo_read:
        lds XL, rx_out            ;X = rx_out pointer
        lds XH, rx_out + 1
        ld r18, X+            ;r18 = X+
        ldi r16, low(rx_fifo + rx_size)    ;r16:r17 = first invalid address above rx fifo
        ldi r17, high(rx_fifo + rx_size);memory space
        cp r16, XL            ;r16:r17 = X?
        cpc r17, XH
        breq rx_fifo_r_rollover        ;yes? roll over
rx_fifo_r_store:
        sts rx_out, XL            ;same as in rx_fifo_store
        sts rx_out + 1, XH        ;save the pointer
        lds r16, rx_n            ;and this time decrease the number of bytes in the
        dec r16                ;FIFO (we read one byte)
        sts rx_n, r16
        ret                ;and return to app r18 = data
rx_fifo_r_rollover:
        ldi XL, low(rx_fifo)        ;X = rx fifo base address
        ldi XH, high(rx_fifo)
        rjmp rx_fifo_r_store
;*******************************************************************************
UART_write_fifo:        ;write from r18 to TX FIFO
        lds r16, tx_n            ;space available?
        cpi r16, tx_size
        brlo uart_fifo_w_store        ;yes? store data
        ret
uart_fifo_w_store:
        lds XL, tx_in            ;X = tx_in
        lds XH, tx_in + 1
        st X+, r18            ;X+ = r18
        ldi r16, low(tx_fifo + tx_size)    ;r16:r17 = first invalid address above fifo mem
        ldi r17, high(tx_fifo + tx_size);space
        cp r16, XL            ;r16:r17 = X?
        cpc r17, XH
        breq tx_fifo_w_rollover        ;yes? roll over
        tx_fifo_w_store:        ;store pointer
        sts tx_in, XL            ;tx_in = X
        sts tx_in + 1, XH
        lds r16, tx_n            ;incrase number of bytes in tx FIFO
        inc r16
        sts tx_n, r16
        in r16, UCR            ;enable UDRE int (necessary if it was disabled before,
        sbr r16, (1<<UDRIE)
        out UCR, r16
        ret
tx_fifo_w_rollover:        ;roll over:
        ldi XL, low(tx_fifo)        ;X = Tx fifo base address
        ldi XH, high(tx_fifo)
        rjmp tx_fifo_w_store
;*******************************************************************************
UART_UDRE:            ;UART Data Register is empty: get data from FIFO
        push r16            ;and send
        in r16, SREG            ;save r16 and SREG
        push r16
        lds r16, tx_n            ;data available?
        cpi r16, 1            ;(tx_n >= 1)
        brsh UART_r_tx_fifo        ;yes? get it and send
        in r16, UCR            ;else disable UDRE int
        cbr r16, (1<<UDRIE)
        out UCR, r16
        pop r16                ;restore SREG
        out SREG, r16
        pop r16                ;restore r16
        reti
UART_r_tx_fifo:
        push r17            ;save r17
        push XL                ;save X
        push XH
        lds XL, tx_out            ;X = tx_out
        lds XH, tx_out + 1
        ld r16, X+            ;get data from FIFO
        out UDR, r16            ;and send it
        ldi r16, low(tx_fifo + tx_size)    ;r16:r17 = first invalid address above TX fifo
        ldi r17, high(tx_fifo + tx_size)
        cp r16, XL            ;r16:r17 = X?
        cpc r17, XH
        breq tx_fifo_r_rollover        ;yes? roll over to base address
tx_fifo_r_store:
        sts tx_out, XL            ;tx_out = X
        sts tx_out + 1, XH
        lds r16, tx_n            ;decrease number of bytes in FIFO
        dec r16
        sts tx_n, r16
        pop XH                ;restore X
        pop XL
        pop r17                ;restore r17
        pop r16                ;restore SREG
        out SREG, r16
        pop r16                ;restore r16
        reti                ;return from INT
tx_fifo_r_rollover:
        ldi XL, low(tx_fifo)        ;X = tx fifo base address
        ldi XH, high(tx_fifo)
        rjmp tx_fifo_r_store
