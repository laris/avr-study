/* optimized half-duplex serial uart implementation
 * 26 instructions, 52 bytes
 * bit-bang RxByte based on AVR305, TxByte original code
 * hard-coded for 81N, 115.2kbps @8Mhz = 69.4 cycles/bit
 * @16Mhz = 139 cycles/bit
 * @author: Ralph Doncaster 2014
 * @version: $Id$
 * macro definition help from Joey Morin
 */
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"

#define UART_DDR  DDRB-0x20    //__SFR_OFFSET 0x20
#define UART_Port PORTB-0x20
#define UART_Tx   PB0
#define UART_Rx   PB1

##define UART_Port PORTB     // PORT
##define UART_Tx 0           // txPin
##define UART_Rx 1           // rxPin
#define BAUD_RATE 115200
#define F_CPU 8000000

#define txrxBUF r24
#define txCNT  r25
#define txRTMP r0

#define DIVIDE_ROUNDED(NUMERATOR, DIVISOR) ((((2*(NUMERATOR))/(DIVISOR))+1)/2)

#define delayArg r22

; txbit takes 3*RXDELAY + 14 cycles
;define TXDELAY 18
#define BIT_CYCLES DIVIDE_ROUNDED(F_CPU,BAUD_RATE) 
#define TXDELAY DIVIDE_ROUNDED(BIT_CYCLES - 14, 3)

; rxbit takes 3*RXDELAY + 12 cycles
#define RXSTART_CYCLES DIVIDE_ROUNDED(3*F_CPU,2*BAUD_RATE) 
#define RXSTART DIVIDE_ROUNDED(RXSTART_CYCLES - 12, 3)
#define RXDELAY DIVIDE_ROUNDED(BIT_CYCLES - 12, 3)

; transmit byte contained in txrxBUF - 12 instructions, 26 bytes
; AVR305 has 1 cycle of jitter per bit, this has none
TxByte:
  sbi UART_Port-1, UART_Tx  //1c set Tx line 1 to output, wr DDR txPin 1 as output
  cbi UART_Port, UART_Tx    //1c start bit, wr 0 to txPin
  in  txRTMP, UART_Port     //1c save PORT to txRTMP 
  ldi txCNT, 3              //1c stop bit & idle state, 3=0b0000_0011
TxLoop:
  ; 8 cycle loop + delay
_tx_delay:
  ldi delayArg, TXDELAY     //1c init  delay
  rcall Delay3Cycle         //3c rcall delay + 3 cycles
_tx_wr_tmp:
  bst txrxBUF, 0             //1c store lsb in T
  bld txRTMP, UART_Tx       //1c wr lsb -> txRTMP, txPin
_tx_next_bit:
  lsr txCNT                 //1c 0000_0011 --> txrxBUF
  ror txrxBUF                //1c 2-byte shift register
_tx_wr_out:
  out UART_Port, txRTMP     //1c wr txRTMP to PORT
_tx_check_done:
  brne TxLoop               //1/2c 
  ret                       //4c

; receive byte into txrxBUF - 11 instructions, 20 bytes
RxByte:
  ldi txrxBUF, 0x80          ; bit shift counter
WaitStart:
  sbic UART_Port-2, UART_Rx    ; wait for start edge
  rjmp WaitStart
  ldi delayArg, RXSTART      ; 1.5 bit delay
RxBit:
  rcall Delay3Cycle        ; delay and clear carry
  ; 6 cycle loop
  ldi delayArg, RXDELAY 
  sbic UART_Port-2, UART_Rx
  sec
  ror txrxBUF
  brcc RxBit
  ; fall into delay for stop bit

; delay (3 cycle * delayArg) -1 + 4 cycles (ret instruction)
; also clears carry (subi instead of dec) to save 1 instr in RxBit
Delay3Cycle:                // 6 bytes
  subi delayArg, 1
  brne Delay3Cycle
  ret
