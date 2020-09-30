/* AVR305 half-duplex serial uart implementation 
 * 28 instructions, 54 bytes
 * hard-coded for 81N, 115.2kbps @8Mhz = 69.4 cycles/bit
 * @16Mhz = 139 cycles/bit
 * @author: Ralph Doncaster
 * @version: $Id$
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

#define txBUFFER  R16
#define rxBUF     R17

#define bitcnt r19   //; stop 1/2 bit -> frame format bit number
#define delayArg r24 //; delay cyc num -> baudrate or bps

; transmit byte contained in txBUFFER - 12 instru 24 bytes(TxLoop->TxDone End)
TxByte: // init
    sbi UART_DDR, UART_Tx   //1c set Tx line to output, set pin
    ldi bitcnt, 10          //1c 1 start + 8 bit + 1 stop
    com txBUFFER   //1c invert and set carry, init start bit
                  // because default c=0, com -> c=1, send rule: c=0 tx->1
TxLoop: // TxLoop, TxStart, Tx0, send 0 // TxLoop to fall 12c
/*
; 9 cycle loop
    //;brcc tx1                     //2 convert to _if
    _if_bit SREG,SREG_C,set,tx1     //t1f2c if c=1, tx 0, fail jp to tx1
    cbi UART_Port, UART_Tx          //1c transmit a 0
tx1: // send 1
    //brcs TxDone                   //2 conver to _if
    _if_bit SREG,SREG_C,clr,TxDone  //t1f2c if c=0, tx 1, fail jp to TxDone
    sbi UART_Port, UART_Tx          //1c transmit a 1
*/
tx_ifelse:
    _if <>,cs,else                  //t1f2c
      cbi UART_Port, UART_Tx        //1c transmit a 0
      //rjmp Eif                    //2c
    _else
      sbi UART_Port, UART_Tx        //1c transmit a 1
      NOP
    _Eif                            //t4f4
TxDone: // one bit tx done, prepare next bit, go delay
    ldi delayArg, 18                //1c 8MHz  3 * 18 - 1 = 53 cycles
  ; ldi delayArg, 41                //-1c 16Mhz 3 x 41 - 1 = 122 cycles
    rcall Delay3Cycle               //3c rcall overhead
    lsr txBUFFER                    //1c shift MSB -> SREG_C
    dec bitcnt                      //1c --tx bits counter
  //brne TxLoop                     // _if
  // if all bits tx done, go next tx stop bit, false, jmp back to txLoop
    _if_bit SREG,SREG_Z,set,TxLoop  //t1f2c
_tx_exit:
; fall into delay for stop bit = 1 = idle state

; delay (3 cycle * delayArg) -1 + 4 cycles (ret instruction)
; also clears carry (subi instead of dec) to save 1 instr in RxBit
Delay3Cycle:                        // 6 bytes
    subi delayArg, 1                //1c
  //brne Delay3Cycle
    _if_bit SREG,SREG_Z,set,Delay3Cycle //t1f2c
    ret                                 //4c

; receive byte into rxBUF - 11 instructions, 20 bytes
RxByte:
    ldi rxBUF, 0x80   //1c 0x80=b1000_0000 bit shift counter
WaitStart:
    //sbic UART_Port-2, UART_Rx ; wait for start edge
    //rjmp WaitStart
    _if_bit UART_Port-2,UART_Rx,clr,WaitStart //t2+f3=1+2, PORT-2=PIN
      ldi delayArg, 32 //1c   8MHz 3x32=96 =1.5x69.4 ? 1.5 x bit
     ;ldi delayArg, 67 //-1c 16Mhz 3x67=201
RxBit:
    rcall Delay3Cycle  ; delay and clear carry
  ; 6 cycle loop
  ; adiw delayArg, 19 clears carry 
    ldi delayArg, 19  ; 3 * 19 - 1 = 56 cycles
   ;ldi delayArg, 40 for 16Mhz
    sbic UART_Port-2, UART_Rx // if read 0, ror, read 1, sec+ror
      sec
    ror rxBUF
    //brcc RxBit                  // c->1000_0000->c
    _if_bit SREG,SREG_C,set,RxBit // first c=1 indicate rx done
_rx_exit:
 ; fall into UartInit

UartInit:                         //4 bytes
    sbi UART_Port, UART_Tx  ; set Tx line to idle state (high)
    ret
