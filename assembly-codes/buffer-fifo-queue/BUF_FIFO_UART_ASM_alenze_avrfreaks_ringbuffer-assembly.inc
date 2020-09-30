; https://www.avrfreaks.net/forum/ringbuffer-assembly
; Posted by alenze: Thu. Apr 13, 2006 - 02:02 PM
; An example for what I use 
; (originally based on code from a member of this forum - sorry, 
; lost the reference a long time ago):
.equ    rxfifo_length  =  0x40    ; length or size of RX fifo buffer (64 bytes)
.equ    txfifo_length  =  0x40    ; length or size of TX fifo buffer (64 bytes)
; Arrays
.DSEG
.org    0x00C0
; UART ring buffers
rxfifo_n: .byte 1 ; number of bytes currently stored in buffer (init: 0)
rxfifo_in: .byte 2 ; pointer to address in buffer written to (init: rxfifo_base)
rxfifo_out: .byte 2 ; pointer to address in buffer read from (init: rxfifo_base)
rxfifo_base: .byte rxfifo_length ; reserves fifo_length bytes for the
; buffer and is used as the buffer base address
txfifo_n: .byte 1 ; number of bytes currently stored in buffer (init: 0)
txfifo_in: .byte 2 ; pointer to address in buffer written to (init: txfifo_base)
txfifo_out: .byte 2 ; pointer to address in buffer read from (init: txfifo_base)
txfifo_base: .byte txfifo_length ; reserves fifo_length bytes for the
;***************************************************************************
;* "USART_UDRE" interrupt handler
;* Transmits data pending in TX buffer in predefined baud rate & 8n1
;* (RTS handshake via pin PB0, CTS_in; single- or multi-byte transfers)
;* All regs saved
;***************************************************************************
USART_UDRE:
        sbic    UCSRA,RXC       ; if receiving (RXC "1"),
        rjmp    U_Tx5           ; exit immediately
        push    Flags_tmp
        in      Flags_tmp,SREG  ; flags may be changed by arithmetics,
        push    Flags_tmp       ;       so save the status register
        push    Tmp1
        push    Tmp2
U_Tx1:
        sbrs    B_Flags4,2      ; set: use HW handshake
        rjmp    Get_txfifo      ; don't use handshake
        sbic    PINB,0          ; check CTS_in (low = pardner ready for receive)
        rjmp    U_Tx3           ; pardner not ready, try later (exit), else
Get_txfifo:     ; fetch a byte from TX buffer and transmit it
        lds     Tmp1,txfifo_n
        tst     Tmp1            ; buffer empty?
        breq    U_Tx3
        sts     save_Y,YL
        sts     save_Y + 1,YH
        lds     YL,txfifo_out
        lds     YH,txfifo_out + 1
        ld      Tmp2,Y+         ; ** data_out **
        out     UDR,Tmp2        ; to UART transmit
        dec     Tmp1
        sts     txfifo_n,Tmp1
        brne    tx_notclr       ; buffer not yet empty!
        cbr     B_Flags3,0x40   ; else: clear TX data pending flag
        cbi     UCSRB,UDRIE     ; and disable UDRE int
tx_notclr:
        ldi     Tmp2,high(txfifo_base + txfifo_length)
        cpi     YL,low(txfifo_base + txfifo_length)
        cpc     YH,Tmp2
        brne    end_get_txfifo
        ldi     YL,low(txfifo_base)
        ldi     YH,high(txfifo_base)
end_get_txfifo:
        sts     txfifo_out,YL
        sts     txfifo_out + 1,YH
        lds     YL,save_Y
        lds     YH,save_Y + 1
exit_get_txfifo:
        rjmp    U_Tx4           ; (remove this for multi-byte transmits)
        ;rjmp   U_Tx1           ; (loop until all bytes are sent -
                                ;  remove this for a one byte transmit)
U_Tx3:
        cbi     UCSRB,UDRIE     ; disable UDRE int to avoid ADC lock-up if
                                ; HW handshake is active
U_Tx4:
        pop     Tmp2
        pop     Tmp1
        pop     Flags_tmp
        out     SREG,Flags_tmp  ; restore the status register
        pop     Flags_tmp
U_Tx5:
        reti
;**** End of USART_UDRE interrupt handler **********************************
;***************************************************************************
;* "USART_Rx" interrupt handler
;* Stores received bytes to fifo ring buffer ("Add_txfifo")
;* Checks for errors, skips byte if error
;* All regs saved
;***************************************************************************
USART_RXC:
        push    Flags_tmp
        in      Flags_tmp,SREG  ; flags may be changed by arithmetics,
        push    Flags_tmp       ;       so save the status register
        push    Tmp1
        push    Tmp3
U_R1:
        in      Tmp1,UCSRA      ; get status
        ldi     Tmp3,0b00011100
        and     Tmp1,Tmp3       ; isolate bits 4-2
        tst     Tmp1            ; if not zero, we have an error
        breq    U_R_cont        ; no error
        in      Tmp1,UDR        ; fetch defective byte, clear its error bits
        ;call   Init_rxbuf      ; (flush rx fifo buffer, too)
        ;**call error handler
        rjmp    U_R2            ; continue -> test fifo for another byte
U_R_cont:
        in      Tmp1,UDR        ; get data from UDR ...
        ; Add_rxfifo - routine  ; ... and store it to RX buffer
        lds     Tmp3,rxfifo_n           ; check if buffer full
        cpi     Tmp3,rxfifo_length - 3  ; if buffer almost full, push RTS_out high
        brsh    stop_rx                 ; (-> HW handshake, "stop", used if no rollover)
add_rxfifo_HWoff:
        cpi     Tmp3,rxfifo_length      ; check if buffer full
        breq    full_rxfifo_n           ; if so, deal with that
add_rxfifo_cont:
        sts     save_Y,YL               ; save Y reg pair
        sts     save_Y + 1,YH
        lds     YL,rxfifo_in            ; setup FIFO_in
        lds     YH,rxfifo_in + 1
        st      Y+,Tmp1                 ; ** store data from Tmp1 **
        sbr     B_Flags4,0x02           ; set bit 1: rx data received and pending
        inc     Tmp3                    ; inc FIFO_n
        sts     rxfifo_n,Tmp3
        ; 16-bit cpi with FIFO_base + FIFO_length
        ldi     Tmp3,high(rxfifo_base + rxfifo_length)
        cpi     YL,low(rxfifo_base + rxfifo_length)
        cpc     YH,Tmp3
        brne    end_add_rxfifo          ; if end of buffer ram reached,
        ldi     YL,low(rxfifo_base)     ; rollover: load FIFO_in with
        ldi     YH,high(rxfifo_base)    ; FIFO_base
end_add_rxfifo:
        sts     rxfifo_in,YL            ; store FIFO_in
        sts     rxfifo_in + 1,YH
        lds     YL,save_Y               ; restore Y reg pair
        lds     YH,save_Y + 1
exit_add_rxfifo:
        rjmp    U_R2                    ; return (Exit Add_rxfifo routine)
full_rxfifo_n:  ; buffer is full: either
                ; overwrite older data in buffer (buffer rollover)
        ;sbr    B_Flags4,0x02           ; clear bit 1, no more rx data pending
        ;rjmp   add_rxfifo_cont
                ; or, alternative: call error handler and end
                ;**call rxfifo_err
        rjmp    exit_add_rxfifo
stop_rx:        ; alternative if no buffer rollover
        RTS_high                        ; HW handshake, "stop"
        rjmp    add_rxfifo_HWoff
U_R2:
        sbic    UCSRA,RXC       ; another byte pending in fifo?
        rjmp    U_R1            ; yes, loop
U_R_exit:
        pop     Tmp3
        pop     Tmp1
        pop     Flags_tmp
        out     SREG,Flags_tmp  ; restore the status register
        pop     Flags_tmp
        reti
;**** End of USART_RCX interrupt handler -------------------------------****
;***************************************************************************
;* UART ring buffer functions
;* ("Add_rxfifo":   store a byte from IO_Source (UDR) to buffer, -> ISR)
;* "Get_rxfifo":    return 1 byte from rx ring buffer in Tmp2
;* "Add_txfifo":    store a byte to transmit buffer
;* ("Get_txfifo":   transmits the bytes stored in transmit buffer -> ISR)
;* "Init_rx/txbuf": initialize/clear fifo ring buffers (RX / TX)
;***************************************************************************
Get_rxfifo:     ; returns a byte from RX buffer in Tmp2
                ; (all regs saved, comments cf. add_rxFIFO)
        push    Tmp1
        push    Tmp3
        lds     Tmp1,rxfifo_n
        cpi     Tmp1,rxfifo_length - 8  ; if buffer no longer almost full,
        brlo    start_rx                ; set RTS_out low again (ready to receive)
get_rxfifo_cont:
        tst     Tmp1
        breq    exit_get_rxfifo
        sts     save_Y,YL
        sts     save_Y + 1,YH
        lds     YL,rxfifo_out
        lds     YH,rxfifo_out + 1
        ld      Tmp2,Y+                 ; ** data_out to Tmp2 **
        dec     Tmp1
        sts     rxfifo_n,Tmp1
        brne    rx_notclr
        cbr     B_Flags4,0x02           ; rx buffer empty: clear "data pending" bit
rx_notclr:
        ldi     Tmp3,high(rxfifo_base + rxfifo_length)
        cpi     YL,low(rxfifo_base + rxfifo_length)
        cpc     YH,Tmp3
        brne    end_get_rxfifo
        ldi     YL,low(rxfifo_base)
        ldi     YH,high(rxfifo_base)
end_get_rxfifo:
        sts     rxfifo_out,YL
        sts     rxfifo_out + 1,YH
        lds     YL,save_Y
        lds     YH,save_Y + 1
exit_get_rxfifo:
        pop     Tmp3
        pop     Tmp1
        ret                             ; EXIT function
start_rx:                               ; switch receive on again
        RTS_low
        rjmp    get_rxfifo_cont
;**************************************************************************
Add_txfifo:     ; stores a byte to TX buffer / all regs saved
                ; entry with data to-be-stored in Tmp2
        push    Tmp1
        push    Tmp3
        lds     Tmp3,txfifo_n           ; load # of chars pending
        cpi     Tmp3,txfifo_length      ; check if buffer full
        breq    full_txfifo_n           ; if so, deal with that
add_txfifo_cont:
        sbr     B_Flags3,0x40           ; set TX data pending flag
        ;sbi    UCSRB,UDRIE             ; (enable UDRE int to start transmit!)
        sts     save_Y,YL               ; save Y reg pair
        sts     save_Y + 1,YH
        lds     YL,txfifo_in            ; setup FIFO_in
        lds     YH,txfifo_in + 1
        st      Y+,Tmp2                 ; ** store data from Tmp2 **
        inc     Tmp3                    ; inc FIFO_n
        sts     txfifo_n,Tmp3
        ; 16-bit cpi with FIFO_base + FIFO_length
        ldi     Tmp3,high(txfifo_base + txfifo_length)
        cpi     YL,low(txfifo_base + txfifo_length)
        cpc     YH,Tmp3
        brne    end_add_txfifo          ; if end of buffer ram reached,
        ldi     YL,low(txfifo_base)     ; rollover: load FIFO_in with
        ldi     YH,high(txfifo_base)    ; FIFO_base
end_add_txfifo:
        sts     txfifo_in,YL            ; store FIFO_in
        sts     txfifo_in + 1,YH
        lds     YL,save_Y               ; restore Y reg pair
        lds     YH,save_Y + 1
exit_add_txfifo:
        pop     Tmp3
        pop     Tmp1
        ret                             ; return
full_txfifo_n:  ; buffer is full: either
                ; overwrite older data in buffer (buffer rollover)
        ;rjmp   add_txfifo_cont
                ; or, alternative: call error handler and end
                ;call   txfifo_err
        rjmp    exit_add_txfifo
;**************************************************************************
; routines to initialize the RX / TX buffers
; All regs saved
Init_rxbuf:
        push    YH
        push    YL
        push    Tmp1
        ; init rx buffer
        clr     YL
        sts     rxfifo_n,YL
        ldi     YL,low(rxfifo_base)
        ldi     YH,high(rxfifo_base)
        sts     rxfifo_in,YL
        sts     rxfifo_in + 1,YH
        sts     rxfifo_out,YL
        sts     rxfifo_out + 1,YH
        cbr     B_Flags4,0x02           ; clear "rx data pending" bit
        clr     Tmp1                    ; reset FIFO_n
        sts     rxfifo_n,Tmp1
        pop     Tmp1
        pop     YL
        pop     YH
        ret
Init_txbuf:
        push    YH
        push    YL
        push    Tmp1
        ; init tx buffer
        clr     YL
        sts     txfifo_n,YL
        ldi     YL,low(txfifo_base)
        ldi     YH,high(txfifo_base)
        sts     txfifo_in,YL
        sts     txfifo_in + 1,YH
        sts     txfifo_out,YL
        sts     txfifo_out + 1,YH
        cbr     B_Flags3,0x40           ; clear "tx data pending" bit
        clr     Tmp1                    ; reset FIFO_n
        sts     txfifo_n,Tmp1
        pop     Tmp1
        pop     YL
        pop     YH
        ret
;**** End of UART buffer functions -------------------------------****