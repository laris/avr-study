Software UART FIFO Buffer   
* http://www.avrbeginners.net/architecture/uart/uart_buffer.html

The UART can only send and receive one byte at a time. I'm not talking about the simultaneous transmission and reception of data now, but the ability to receive data blocks, like strings or commands with arguments. Wouldn't it be nice to have the ability to read a whole string from UART and then process it? That's where a FIFO buffer comes in.

A FIFO buffer for the UART can make life easier and doesn't slow things down if it's properly programmed. While [Advanced Assembler -> Buffers and Queues](http://www.avrbeginners.net/advanced_asm/buffers.html) only gives a general overview of this topic, here's a complete and working example.

To make things faster, interrupts are used for both directions. The Rx FIFO and the Tx FIFO each need their own space in SRAM. Each need the space for received data / data to be sent, one pointer for writing (rx_in or tx_in), one pointer for consuming (rx_out or tx_out) and one byte for holding the number of bytes currently held in the FIFO (rx_n or tx_n).

When a byte is to be stored in the FIFO, the following is done:

The byte is stored at the write pointer address, which is then post-incremented. If needed, the pointer will have to roll over to the base address of the FIFO data space again. Then the number of bytes in the FIFO is incremented. As the pointers and the amount of data are stored in sram, we need to take care of re-storing them again. To make things safer, the routines have to check if storing/reading is actually possible. No data is allowed to be stored if the FIFO is full. Similarly, no data can be read if it is empty.

The FIFOs have to be initialised in order to work. The pointers have to point at the FIFOs base address (where the first byte will be stored) and rx_n or tx_n have to be set to zero before any FIFO operation is done. This can be placed at the location where the UART is initialised as well.

The initialisation looks like this:
.equ rx_size = 16  
      .equ tx_size = 16  
      .dseg  
      rx_fifo: .byte rx_size  
      rx_in: .byte 2  
      rx_out: .byte 2  
      rx_n: .byte 1  
      tx_fifo: .byte tx_size  
      tx_in: .byte 2  
      tx_out: .byte 2  
      tx_n: .byte 1  
      .cseg  
      init_FIFOs:  
      ldi r16, low(rx_fifo)  
      ldi r17, high(rx_fifo)  
      sts rx_in, r16  
      sts rx_in + 1, r17  
      sts rx_out, r16  
      sts rx_out + 1, r16  
      clr r16  
      sts rx_n, r16  
      ldi r16, low(tx_fifo)  
      ldi r17, high(tx_fifo)  
      sts tx_in, r16  
      sts tx_in + 1, r17  
      sts tx_out, r16  
      sts tx_out + 1, r16  
      clr r16  
      sts tx_n, r16  
      ret | ; first set the size of the receiver  
      ; and the transmitter FIFO  
      ;  
      ;  
      ; then reserve sram space for the rx FIFO  
      ; and its pointers  
      ;  
      ; and the counter  
      ;  
      ; same for the transmitter side;  
      ;  
      ;  
      ;  
      ;  
      ;  
      ; this is a routine we can call during init:  
      ; load address of the rx FIFO space to r16:r17  
      ;  
      ; and store it as the in and  
      ;  
      ; out pointer  
      ;  
      ; clear the counter  
      ; and store it as well.  
      ;  
      ; same for the transmitter  
      ;  
      ;  
      ;  
      ;  
      ;  
      ;  
      ;  
      ; return from the routine

Receiver FIFO:

As the UART receiver only has one interrupt source, we don't need to choose one (this will be needed for the transmitter). The UART Rx interrupt occurs whenever a byte has been received. This byte is then added to the Rx FIFO by the ISR. Another routine is needed to consume a byte from the buffer again during normal operation, for example when we need to process some received data.

That makes 2 routines for the Rx side. First, the ISR:
UART_RXC:  
      push r16  
      lds r16, rx_n  
      cpi r16, rx_size  
      brlo rx_fifo_store  
      pop r16  
            in r16, UDR  
      reti  
      rx_fifo_store:  
      in r16, SREG  
      push r16  
      push r17  
      push XL  
      push XH  
      in r16, UDR  
      lds XL, rx_in  
      lds XH, rx_in + 1  
      st X+, r16  
      ldi r16, low(rx_fifo + rx_size)  
      ldi r17, high(rx_fifo + rx_size)  
      cp XL, r16  
      cpc XH, r17  
      breq rx_fifo_w_rollover  
      rx_fifo_w_store:  
      sts rx_in, XL  
      sts rx_in + 1, XH  
      lds r16, rx_n  
      inc r16  
      sts rx_n, r16  
      pop XH  
      pop XL  
      pop r17  
      pop r16  
      out SREG, r16  
      pop r16  
      reti  
      rx_fifo_w_rollover:  
      ldi XL, low(rx_fifo)  
      ldi XH, high(rx_fifo)  
      rjmp rx_fifo_w_store | ; UART Rx Complete ISR  
      ; save r16  
      ;  
      ; get counter  
      ; if FIFO not full,  
      ; store data  
      ; else restore r16  
      ; clear interrupt by reading UDR  
      ;  
      ;  
      ;  
      ; SREG  
      ;  
      ;r17  
      ; and a pointer  
      ;  
      ;  
      ; get data  
      ; set up pointer  
      ;  
      ; and store in FIFO  
      ;  
      ; load r16:r17 with first invalid address after FIFO space  
      ;  
      ; do a 16-bit compare:  
      ; X = r16:r17?  
      ; if yes, roll over to beginning of FIFO space  
      ;  
      ; store pointer rx_in  
      ;  
      ;  
      ;  
      ; get counter  
      ; increment  
      ; store counter again  
      ;  
      ; restore registers we used  
      ;  
      ;  
      ;  
      ;  
      ;  
      ; return  
      ;  
      ; if X stored the data at the last fifo memory location,  
      ; roll over to the first address again  
      ;  
      ; and proceed as usual

Reading from the buffer requires another routine which uses the rx_out pointer to get data from the buffer. It also doesn't need to save stuff, as it's not an ISR and will be executed at a known time. The routine shall return the data from the buffer in r18.
UART_read_fifo:  
      lds r16, rx_n  
      cpi r16, 1  
      brsh rx_fifo_read  
      ret  
      rx_fifo_read:  
      lds XL, rx_out  
      lds XH, rx_out + 1  
      ld r18, X+  
      ldi r16, low(rx_fifo + rx_size)  
      ldi r17, high(rx_fifo + rx_size)  
      cp r16, XL  
      cpc r17, XH  
      breq rx_fifo_r_rollover  
      rx_fifo_w_store:  
      sts rx_out, XL  
      sts rx_out + 1, XH  
      lds r16, rx_n  
      dec r16  
      sts rx_n, r16  
      ret  
      rx_fifo_r_rollover:  
      ldi XL, low(rx_fifo)  
      ldi XH, high(rx_fifo)  
      rjmp rx_fifo_r_store | ; call this from within the application to get UART Rx data to r18  
      ; load number of received bytes  
      ; if one byte or more available,  
      ; branch to rx_fifo_read  
      ;else return  
      ;  
      ; data is available:  
      ; Get the Rx FIFO consume pointer  
      ;  
      ; and load data to r18  
      ;  
      ; check if end of mem space reached:  
      ; r16:r17 = first invalid address above Rx FIFO memory  
      ; 16-bit compare: X = invalid address above Rx FIFO memory?  
      ;  
      ; yes, roll over to base address  
      ;  
      ; store the new pointer  
      ;  
      ;  
      ;  
      ; load counter  
      ; decrease it  
      ; and store it again  
      ; return to application  
      ;  
      ; roll over to base address:  
      ; load base address to X  
      ;  
      ; and store the pointer

Transmitter FIFO

The transmitter FIFO for the UART works just like the receiving one, with a small difference: The ISR routine in this case reads from the FIFO and writes the data to UDR, while the write routine takes the data from a specified location or register (let's take r18) and writes it to the FIFO.

So which interrupt do we choose? The UART offers the UART Data Register Empty (UDRE) interrupt and the UART Transmit Complete (TXC) interrupt. The transmit complete interrupt only occurs when a transmission is finished, so we can't use it for our purpose for two reasons:

- The Transmission finishes and then the ISR is called. So what? Maximum speed can't be achieved when using this interrupt. By using the UDRE int, the next byte to be transmitted is already in UDR when the previous transmission finishes and can be tranmitted by the hardware. If the interrupt occurs when the previous transmission finishes, the next byte has to be taken from the buffer memory space first and time is lost between two transmissions.

- If the UDRE interrupt is used and no data is available (last transmission was the last byte in the buffer) we can just disable the UDRE int re-enable it as soon as new data is written to the transmit FIFO. By re-enabling it, the ISR will be called because UDR is emtpy and transmission will start again. The TXC int will not provide this automatical transmission start. The code for the transmit FIFO can be cut 'n pasted from the RX FIFO with the small changes described above. This will be no problem if you understood the RX FIFO.

The following code does the following (it's written for a 2313):

Stack and UART setup (38400 baud @ 7.3728 MHz)

FIFO setup

Receive data via Rx FIFO and loop it back via Tx FIFO

If you have an STK 500 you only need to plug in a 2313 and a 7.3728 MHz crystal, connect PD0 to the RS232 spare RxD pin and PD1 to the TxD pin. Don't forget power and the connection to your PC via a COM port...

Also change the first line (include directive for 2313def.inc) to suit your system.

[Here's the asm file](http://www.avrbeginners.net/architecture/uart/uart_buffer.asm)

# Buffers And Queues in Assembler
* http://www.avrbeginners.net/advanced_asm/buffers.html 
* https://web.archive.org/web/20030629221538/http://www.avrbeginners.net/advanced_asm/buffers.html#fifo_hints 

[How it Works] [[Memory Usage](https://web.archive.org/web/20030629221538/http://www.avrbeginners.net/advanced_asm/buffers.html#fifo_usage)] [[Coding Hints](https://web.archive.org/web/20030629221538/http://www.avrbeginners.net/advanced_asm/buffers.html#fifo_hints)]

How it works

Buffers and queues are basically the same. The term buffer is mainly used for queues holding I/O data, like from the UART or other peripherals.

The buffer type I'm describing here is the FIFO (First-In-First-Out) buffer. That means that the data written first is also read first. Another buffer type is the LIFO (Last-In-First-Out) buffer. The Stack is a LIFO buffer, as it always reads the data that was written last. The LIFO buffer can be explained as a stack of sheets on your desk: When something has to be done, you write it on a piece of paper and put it on top of the stack. When you've got time to accomplish one of these tasks, you take the sheet that's at the top. The problem is that if many tasks are coming in it can happen that the oldest sheet will never be looked at. If it's a FIFO buffer, you'd take the sheet that's at the bottom.

When writing code for a FIFO in software, a slightly different approach is taken: We add data to the buffer's memory space, increment the number of bytes in the buffer and have the pointer we used for writing the data point to the next buffer memory location. When the last location of buffer memory is reached, we roll over to the first location again. That's why the FIFO is also called a "ring buffer". When reading from the buffer, the same is done, but the number of bytes in the buffer is decremented.

![](https://web.archive.org/web/20030629221538im_/http://www.avrbeginners.net/advanced_asm/img/fifo_overview.gif)

This is a simple diagram of how a FIFO buffer works. In this case, a post-incrementing scheme is used (as the AVR supports post-incrementing of the index register pairs in hardware): When a location is read or written, the pointer is incremented and points at the next FIFO location. The numbers in the boxes (each representing one byte in memory) represent the order the bytes were written in. The first byte is already read (number in brackets), that's why the read pointer points at the second byte. Four bytes have already been written and the write pointer points at the fifth location (which is not written: "n"). So three bytes are still in the buffer. When the write pointer is post-incremented and points at the location following the buffer memory (grayed out), it rolls over to the first location again:

![](https://web.archive.org/web/20030629221538im_/http://www.avrbeginners.net/advanced_asm/img/fifo_rollover.gif)

In this image, 7 bytes are in the buffer. The first byte has already been read (it's in brackets). One more byte can be written until the buffer is full. The pointers would then both point at location 2.

To make life easier, a counter is added that holds the number of bytes in the buffer. It's also possible to compare the pointers to each other in order to determine if the buffer is full or empty, but that also comes with some problems. Just trust me: The counter solution is better...

Memory Usage

The FIFO buffer decribed above uses the following memory resources:  
- FIFO Data Memory (8 bytes)  
- Write pointer (2 bytes)  
- Read pointer (2 bytes)  
- Counter (1 byte)

This adds up to 13 bytes for a fuly functional 8-byte FIFO. If the FIFO size has to be greater than 256 bytes, a second counter byte has to be added, but I don't believe that such a size will ever be needed...

Coding Hints

It's possible to reduce the amount of program space needed for the FIFO operation if the FIFO size is equal to a power of 2 (4, 8, 16, 32, ...). Then the pointer can be masked with the buffer's greatest index (not really; explanation below). Example:

Base address: 0x0010  
Size: 16 bytes  
Write pointer is at index 15 (0x001F), which is the 16th byte of the buffer.

Now the write pointer is used to write data to the buffer (at base address + 0x0F = 0x001F) and post- incremented (0x0020). That means that it now doesn't point at the FIFO memory space any more and has to roll over. The write pointer is now masked with the greatest index (0x000F): write pointer &= 0x000F = 0x0000 and the high bytes are updated: Write pointer OR 0x0010 = 0x0010. This is the address we want (the base address).

Here's the code example:

;write from r16 to buffer using X as write pointer  
```
      ;size: 16 bytes  
      ;base address: 0x0010
write_fifo:  
      st X+, r16  
      ldi r16, 0x0F  
      and XL, r16  
      sbr XL, 0x10  
      ret | ;  
      ;store data  
      ;load r16 with mask for 16-byte size  
      ;mask pointer with r16  
      ;update high bits  
      ;done!!!
```
Try to write code that's uses a compare and then loads the pointer with the base address again. You'll see that it's impossible to do that without using more code space and cpu time. It can be necessary to do that though, because the conditions that make masking possible can't always be met. That depends on your application and memory usage. For testing this buffer type it might be better to do a compare and than change the code to masking operation.