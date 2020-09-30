# Software UART with FIFO
* https://www.avrfreaks.net/projects/software-uart-fifo
* Posted by danni on Sat. Feb 14, 2009 - 04:00 PM
* TYPE: Complete code
* COMPILER/ASSEMBLER: WinAVR (GNU GCC)

Following a software UART with buffered transmit and receive.
It can be used to get a second UART or for ATtiny without UART.

It uses a timer with clear on compare feature to determine the transmission baud rate.
Another compare interrupt of the same timer was used for the receive interrupt.
To catch the start bit, the input capture interrupt was used.
On AVRs without ICP, also an external interrupt can be used, but then on high baud rates the interrupt delay should be substract from the bit scan time.

Peter

* ATTACHMENT(S): 
[softuartfifo.zip](https://www.avrfreaks.net/sites/default/files/project_files/softuartfifo.zip)
* Tags: [Complete code](https://www.avrfreaks.net/projects-types/complete-code), [WinAVR (GNU GCC)](https://www.avrfreaks.net/compilers/winavr-gnu-gcc)

# UART with FIFO
* https://www.avrfreaks.net/projects/uart-fifo
* Posted by danni on Wed. Aug 6, 2008 - 05:20 PM
* TYPE: Complete code
* COMPILER/ASSEMBLER: WinAVR (GNU GCC)

Following an example code with interrupt driven UART with FIFO.

Typically a FIFO size of about 256 Bytes was sufficient for most AVR projects. Thus the FIFO was realized with index instead pointer, which saves code space and CPU-time, because the index can be hold in a char (8bit).  
Also because 8bit access is atomic, no interrupts must be disabled during the main loop functions.  
The maximum size is 257 (TX) and 258 (RX) byte, because also the hardware buffers are used.

Especially care was taken to support all the different AVR derivates.  
Thus are some definitions for the different vectors, bytes and bits inside the uart0.h.

Also all functions are named **0, so its easy to copy the code for the other UARTs and rename it to **1 ... **3.

Peter

* Attachment(s): 
[uartfifo.zip](https://www.avrfreaks.net/sites/default/files/project_files/uartfifo.zip)

* Tags: [Complete code](https://www.avrfreaks.net/projects-types/complete-code), [WinAVR (GNU GCC)](https://www.avrfreaks.net/compilers/winavr-gnu-gcc)
