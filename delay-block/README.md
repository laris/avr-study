# AVR block/jam delay routines

## AVR glibc delay 3N x cycles, [3, 768]/step=3, 3 x words

* https://www.nongnu.org/avr-libc/user-manual/delay__basic_8h_source.html
* http://uzebox.org/wiki/index.php?title=Assembler_Tips&oldid=5226#To_kill_off_3N_cycles:_.28N_.3E_0.29
* `include/util/delay_basic.h`

> The functions in this header file implement simple delay loops that perform a busy-waiting. They are typically used to facilitate short delays in the program execution. They are implemented as count-down loops with a well-known CPU cycle count per loop iteration. As such, no other processing can occur simultaneously.

> It should be kept in mind that the functions described here do not disable interrupts.

> In general, for long delays, the use of hardware timers is much preferrable, as they free the CPU, and allow for concurrent processing of other events while the timer is running. However, in particular for very short delays, the overhead of setting up a hardware timer is too much compared to the overall delay time.

> Two inline functions are provided for the actual delay algorithms.

> Delay loop using an 8-bit counter \c __count, so up to 256 iterations are possible.  (The value 256 would have to be passed as 0.)  The loop executes three CPU cycles per iteration, not including the overhead the compiler needs to setup the counter register.

> Thus, at a CPU speed of 1 MHz, delays of up to 768 microseconds can be achieved.

  ```c
  80 void
  81 _delay_loop_1(uint8_t __count)
  82 {
  83     __asm__ volatile (
  84         "1: dec %0" "\n\t"
  85         "brne 1b"
  86         : "=r" (__count)
  87         : "0" (__count)
  88     );
  89 }
  ```

* avr asm version
  ```
  .DEF REG_CNT      = R16               ; counter register, 8-bit, [0,255]
  .EQU CNT_CYC      = 1                 ; cycles cost = 3 x N
                                        ; cycles, words
  LOAD_CNT: LDI   REG_CNT, CNT_CYC      ; 1c,     1w
  REP_LOOP: DEC   REG_CNT               ; 1c,     1w
            BRNE  REP_LOOP              ; 1f/2t,  1w
  ```

* delay range analysis
  * `REG_CNT` = 1
    * cycles = 1/`LDI` + <u>( 1/`DEC` + 1/`BRNE`/false )</u> = 3
    * `BRNE`, `REG_CNT != 0`, True, cost 2 cycle
    * `BRNE`, `REG_CNT == 0`, False, cost 1 cycle
  * `REG_CNT` = 2
    * cycles = 1/`LDI` + <u>1 x (1 + 2/ture)</u> + <u>1/`DEC` + 1/`BRNE`</u> = 6
  * `REG_CNT` = 255
    * cycles = 1 + <u>254 x (1+2)</u> + <u>1 + 1</u> = 765
  * `REG_CNT` = 0 
    * cycles = 1 + <u>255 x (1+2)</u> + <u>1 + 1</u> = 768

* Algorithm
  * Range = 3 x N, N=[1, 255]+[0] = [3, 768]/step=3
  * cycles(n) =
    ```
    if n == 0 { 3 x 256 }
    else if n = [1, 255] { 3 x n }
    ```
  * n(c) = 
    ```
    if c%3 == 0
    {
      if c = 768 { 0 }
      else { c/3 }
    }
    ```

* code size = 3 x words

## AVR glibc delay 4N x cycles, [4+1, 262,148+1]/step=4, 4 x words
> Delay loop using a 16-bit counter \c __count, so up to 65536 iterations are possible.  (The value 65536 would have to be passed as 0.)  The loop executes four CPU cycles per iteration, not including the overhead the compiler requires to setup the counter register pair.

> Thus, at a CPU speed of 1 MHz, delays of up to about 262.1 milliseconds can be achieved.

  ```c
  102 void
  103 _delay_loop_2(uint16_t __count)
  104 {
  105     __asm__ volatile (
  106         "1: sbiw %0,1" "\n\t"
  107         "brne 1b"
  108         : "=w" (__count)
  109         : "0" (__count)
  110     );
  111 }
  ```

  ```
  .DEF REG_CNTL     = R24                 ; counter reg pair 16-bit, [0,65535]
  .DEF REG_CNTH     = R25                 ; counter reg pair 16-bit, [0,65535]
  .EQU CNT_CYC      = 1                   ; cycles cost = 4 x N
                                          ; cycles, words
  LOAD_CNT: LDI   REG_CNTL,  low(CNT_CYC) ; 1c,     1w
            LDI   REG_CNTH, high(CNT_CYC) ; 1c,     1w
  REP_LOOP: SBIW  REG_CNTL, 1             ; 2c,     1w
            BRNE  REP_LOOP                ; 1f/2t,  1w
  ```

* delay range analysis
  * `REG_CNT` = 1
    * cycles = 2/`LDI` + <u>( 2/`SBIW` + 1/`BRNE`/false )</u> = 5 = 4+1
  * `REG_CNT` = 2
    * cycles = 2/`LDI` + <u>1 x (2+2/ture)</u> + <u>2/`SBIW` + 1/`BRNE`</u>=8+1
  * `REG_CNT` = 65535 
    * cycles = 2 + <u>65535 x (2+2)</u> + <u>2 + 1</u>=2 + 262,140 + 3 = 262,145
  * `REG_CNT` = 0
    * cycles = 2 + <u>65536 x (2+2)</u> + <u>2 + 1</u> = 262,145 + 4 = 262,149

* Algorithm
  * Range = 4 x N + 1, N=[1, 65,535]+[0] = [4+1, 262,148+1]/step=4
  * cycles(n) =
    ```
    if n == 0 { 4 x 65536 + 1 }
    else if n = [1, 65535] { 2/ldi + 2/sbiw + 1/brne/f + (n-1)*4 = 4*n + 1 }
    ```
  * n(c) = 
    ```
    if c%4 == 1
    {
      if c = 262,148 + 1 { 0 }
      else { c/4 }
    }
    ```

* code size = 4 x words

#### other same 4xCycles
* http://avr-mcu.dxp.pl Radoslaw Kwiecien, 2008
  * http://en.radzio.dxp.pl/avr-mcu/delay-assembly-utilities.html
* sansan@AVRFreaks
  * https://www.avrfreaks.net/comment/557666#comment-557666
  * This macro handles delays from 1 to 262148 (1 + 65536*4 + 3)
* http://www.avr-asm-tutorial.net/avr_en/timingloops/index.html
  * http://www.avr-asm-tutorial.net/avr_en/timingloops/delay8.html
  * http://www.avr-asm-tutorial.net/avr_en/timingloops/delay16.html
  * http://www.avr-asm-tutorial.net/avr_en/timingloops/delay24.html
* https://stackoverflow.com/questions/47453737/how-do-avr-assembly-brne-delay-loops-work

```
sansan@AVRFreaks 20100119
The macro looks like subroutine converted to macro.
With conditional compilation macro can be smarter (avr-gcc as)

.macro delay_cycles_2 cycles:req
 .set _cy_, \cycles
 ; up to 10 cycles can be done by 'nop' and 'rjmp .' sequence
 ; in less-or-equal words than loop
 .if _cy_ >= 11
  .set _cy_, _cy_ - 1
  ldi r24, lo8( _cy_ / 4 )
  ldi r25, hi8( _cy_ / 4 )
  sbiw r24, 1
  brne .-4
  .set _cy_, _cy_ % 4
 .endif
    ; less than 11 cycles or remainder from long delay
 .if (_cy_ & 0x01)
  nop
 .endif
 .rept  (_cy_ / 2)
  rjmp .
 .endr
.endm
This macro handles delays from 1 to 262148 (1 + 65536*4 + 3)
```

```

```

## UZEBox version http://uzebox.org [8, 263]/step=1
* http://uzebox.org/wiki/index.php?title=Assembler_Tips&oldid=5226#To_kill_off_variable_number_of_cycles

> To kill off variable number of cycles

> You may also write a routine which waits an arbitrary amount of cycles as follows:

> This produces a delay of 12 cycles (excluding the CALL or RCALL used to call it), when r24 is 4. By incrementing r24, you can increment the delay cycle by cycle, up to 267 (r24 = 3, after wrapping around).

  ```
  delay_cycles:
    lsr   r24
    brcs  .    ; +1 if bit0 was set
    lsr   r24
    brcs  .    ; +1 if bit1 was set
    brcs  .    ; +1 if bit1 was set
    dec   r24
    nop
    brne  .-6  ; 4 cycle loop
    ret
  ```

* avr asm version

  ```
  .DEF REG_CNT      = R16               ; counter register, 8-bit, [0,255]
  .EQU CNT_CYC      = 7                 ; cycles cost = N
                                        ; cycles, words
  LOAD_CNT: LDI   REG_CNT, CNT_CYC      ; 1c,     1w
  ADJ_0123: LSR   REG_CNT               ; 1c,     1w, /2, get reg%2 for next
            BRCS  PC+1/.                ; 1f/2t,  1w, if 0/1c, 1/2c
            LSR   REG_CNT               ; 1c,     1w, /2, get reg%2 for next
            BRCS  PC+1/.                ; 1f/2t,  1w, if 2/1c
  ADJ_END:  BRCS  PC+1/.                ; 1f/2t,  1w, if 3/1c
  REP_LOOP: DEC   REG_CNT               ; 1c,     1w
            NOP                         ; 1c,     1w
            BRNE  REP_LOOP              ; 1f/2t,  1w
  ```
* delay range analysis
* repeat loop
  * 4 x cycles per iteration
  * same glibc 3 x cycles, add one cycle via NOP
* 1-cycle-step adjust
  * because `REP_LOOP:` start decrease 1, so before `DEC`, the `REG_CNT >= 1` to start analyze for good init status
    * when `REG_CNT = 1`, go back from `ADJ_END` to `ADJ_0123`
    * there are 2 x `LSR`, so the initial `REG_CNT=4: 0b0001 -> 0b0010 -> 0b0100`
  * `REG_CNT=4=0b0100`
    * cycles = 1/`LSR/0010(0)` + 1/`BRCS` + 1/`LSR/0001(0)` + 2/`BRCS` = 5
  * `REG_CNT=5=0b0101`
    * cycles = 1/`LSR/0010(1)` + 2/`BRCS` + 1/`LSR/0001(0)` + 2/`BRCS` = 6
  * `REG_CNT=6=0b0110`
    * cycles = 1/`LSR/0011(0)` + 1/`BRCS` + 1/`LSR/0001(1)` + 2x2/`BRCS` = 7
  * `REG_CNT=7=0b0111`
    * cycles = 1/`LSR/0011(1)` + 2/`BRCS` + 1/`LSR/0001(1)` + 2x2/`BRCS` = 8
  * then go to loop, no loop, just go through
    * cycles = 1/`LDI` + 1/`DEC` + 1/`NOP` + 1/`BRNE` = 4

  * `REG_CNT=8`
    * cycles = (1-step)<u>4+1=5</u> + (last loop)<u>1/`LDI`+1/`DEC` + 1/`NOP` + 1/`BRNE`=4</u> + (1xloop)<u>1/`DEC` + 1/`NOP` + 2/`BRNE`=4</u> = 5+4+4 = 13

  * `REG_CNT=252` 252 = 63 * 4 + 0
    * cycles = 4+1 + 63*4 = 257
  * `REG_CNT=253` 253 = 63 * 4 + 1
    * cycles = 4+2 + 63*4 = 258
  * `REG_CNT=254` 254 = 63 * 4 + 2
    * cycles = 4+3 + 63*4 = 259
  * `REG_CNT=255` 255 = 63 * 4 + 3
    * cycles = 4+4 + 63*4 = 260

  * `REG_CNT=0` 255+1=256 = 64 * 4 + 0
    * cycles = 4+1 + 64*4 = 261
  * `REG_CNT=1` 255+2=257 = 64 * 4 + 1
    * cycles = 4+2 + 64*4 = 262
  * `REG_CNT=2` 255+3=258 = 64 * 4 + 2
    * cycles = 4+3 + 64*4 = 263
  * `REG_CNT=3` 255+4=259 = 64 * 4 + 3
    * cycles = 4+4 + 64*4 = 264

* actually, 1x`LSR` mean divide by `2`, 2x`LSR` mean divide by `4`
  * the 1-step adjust have a pattern from previous analysis
  * if `REG_CNT % 4 == 0` then cost `4 + 1 = 5 cycles`
  * if `REG_CNT % 4 == 1` then cost `4 + 2 = 6 cycles`
  * if `REG_CNT % 4 == 2` then cost `4 + 3 = 7 cycles`
  * if `REG_CNT % 4 == 3` then cost `4 + 4 = 8 cycles`
 
* Algorithm
  * Range = [9=4x2+1, 264=4x66]=[9,264]=9+[0,255]
  * cycles(n=reg)= <br>n=register counter value</br>
    ```
    if n>=4 && n<=255 {
      (n/4+1)*4 +
      {
        if n % 4 == 0 { 0+1 = 1 }
        if n % 4 == 1 { 2+1 = 2 }
        if n % 4 == 2 { 2+1 = 3 }
        if n % 4 == 3 { 3+1 = 4 }
      }
    }
    else if n>=0 && n<=3
    {
      ((255+1+n)/4+1)*4 +
      {
        if n % 4 == 0 { 0+1 = 1 }
        if n % 4 == 1 { 2+1 = 2 }
        if n % 4 == 2 { 2+1 = 3 }
        if n % 4 == 3 { 3+1 = 4 }
      }
    }
    ```

  * n(n=reg)(cycles)=
    ```
    if c >=9 && c<=256
    { c-5 }
    else c>=261 && c<=264
    { c-5-256 }
    ```

* code size = 9 words

## Peter Dannegger danni@AVRFreaks 8+[0,65535]=[8,65543]/step=1
* https://www.avrfreaks.net/forum/precise-delay-asm
* https://www.avrfreaks.net/comment/557598#comment-557598
  ```
  ;************************************************************************
  ;*                      Delay Macro 8 ... 65543 Cycle                   *
  ;*              Author: Peter Dannegger                                 *
  ;************************************************************************
  .listmac
  ;delay 8 ... 65543 cycle
  .macro  mdelay
          ldi    r24,  low(@0 - 8)
          ldi    r25, high(@0 - 8)
          sbiw   r24, 4
          brcc   pc - 1
          cpi    r24, 0xFD
          brcs   pc + 4
          breq   pc + 3
          cpi    r24, 0xFF
          breq   pc + 1
  .endmacro
          mdelay   8
          mdelay   9
          mdelay  10
          mdelay  11
          mdelay  12
          mdelay  65543
          rjmp    pc
  /************************************************************************/
  ```

* avr asm version
  ```
  .DEF REG_CNTL     = R24                 ; counter reg pair 16-bit, [0,65535]
  .DEF REG_CNTH     = R25                 ; counter reg pair 16-bit, [0,65535]
  .EQU CNT_CYC      = 0                   ; cycles cost = 4 x N
                                          ; cycles, words
  LOAD_CNT: LDI   REG_CNTL,  low(CNT_CYC) ; 1c,     1w
            LDI   REG_CNTH, high(CNT_CYC) ; 1c,     1w
  REP_LOOP: SBIW  REG_CNTL, 4             ; 2c,     1w, dec 4
  ADJ_0123: BRCC  REP_LOOP/PC-1           ; 1f/2t,  1w, if no overflow, go loop 
            CPI   REG_CNTL, 0xFD          ; 1c,     1w, Rd - 253
            BRCS  DELAY_END/PC+4          ; 1f/2t,  1w, if < 253, +2c, go end
            BREQ  DELAY_END/PC+3          ; 1f/2t,  1w, if = 253, +2c
            CPI   REG_CNTL, 0xFF          ; 1c,     1w, Rd - 255
  ADJ_01:   BREQ  DELAY_END/PC+1          ; 1f/2t,  1w, if = 255, +2c
  DELAY_END:
  ```

* analysis
  ```
  REG_CNT   LDI   SBIW     BRCC   CPI       BRCS  BREQ  CPI       BREQ  total
  0         2     2/252/-4 1      1/252-253 2     0     0         0     8
  1         2     2/253/-3 1      1/253-253 1     2     0         0     9
  2         2     2/254/-2 1      1/254-253 1     1     1/254-255 1     10
  3         2     2/255/-1 1      1/255-253 1     1     1/255-255 2     11

  4         2     2/0      2
                  2/252/-4 1      1         2     0     0         0     12
  5         2     2/1      2
                  2/253/-3 1      1         1     2     0         0     13

  r         2     2        1      1         [2,3,4,5]
                  (r/4)*4
  ```

* Algorithm
  * Range = 8+[0,65535]=[8,65543]/step=1
  * cycles(n=reg) =
    ```
    (n/4)*4 +
    {
      if n%4 == 0 { 6 + 2 }
      if n%4 == 1 { 6 + 3 }
      if n%4 == 2 { 6 + 4 }
      if n%4 == 3 { 6 + 5 }
    }
    ```
  * n=reg(cycles) = 
    ```
    c-8
    ```
* code size = 9 words

## Bret Mulvey `AVR Delay Loop Calculator` asm generator via Javascript
* Developed originally by Bret Mulvey. 
  * http://www.bretmulvey.com/avrdelay.html (cannot work now)
  * https://web.archive.org/web/20200310222125/http://www.bretmulvey.com/avrdelay.html
* Register enhancement by T. Morland. (ACES '18)
  * http://darcy.rsgc.on.ca/ACES/TEI4M/AVRdelay.html

```
RJMP PC+1 (asm)
RJMP .+2  (avr-gcc/gas)
LPM will destroy R0

cyc formula   words NOP RJMP  LPM
0       0     0     0   0     0
1       1     1     1   0     0
2       2     1     0   1     0

3       3     1     0   0     1
4     2x2     2     0   2     0
5     2+3     2     0   1     1

6     1x2     2     0   0     2
7   1+1x2     3     1   0     2
8   2+1x2     3     0   1     2

9   1x3+0     3     0   0     3
10  1x3+1     4     1   0     3
11  1x3+2     4     0   1     3

12  4x3+0     3     0   0     0   12 = LDI R16,N=4; L(N-3):DEC R16; BRNE L(N-3)
13  4x3+1     4     1   0     0   13 = 12 + 1
14  4x3+2     4     0   1     0

15  = 5x3 + 0                     N=5
16  = 5x3 + 1/RJMP
17  = 5x3 + 2/LPM

18  = 6x3 + 0
19  = 6x3 + 1
20  = 6x3 + 2
...

765 = 255x3+0
766 = 255x3+1
767 = 255x3+2

768 256x3+0 N=0
769 256x3+1 N=0
770 256x3+2 N=0

771 256x3+3 N=1
772 256x3+4 N=1
773 256x3+5 N=1

774 256x3+6 N=2
775 256x3+7 N=2
776 256x3+8 N=2
777 256x3+9 N=2

778 
    ldi  r16, 2
    ldi  r17, 1
L1: dec  r17
    brne L1
    dec  r16
    brne L1
    rjmp PC+1

```

```

```

```

```