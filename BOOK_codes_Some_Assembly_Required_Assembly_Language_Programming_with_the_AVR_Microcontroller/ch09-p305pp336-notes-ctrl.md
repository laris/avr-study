

## 9.1 Altering the flow of control

### 9.1.1. JUMP

### 9.1.2 CALL and RETURN

### 9.1.3 Conditional BRANCH

* Branch is taken = PC is changed to new target addr
* PC not change = branch instruction sequenced
* branch depend on 1 or more bits of SREG in processor
  * SREG reflect result of past instru
  * only some instru modify SREG, other not

## 9.2 jump and conditional branch instructions

* JUMP instru
  * RJMP ADDR
    * ADDR limit within 2048 words of instru itself
    * limit 2^12=4096 displacement bits
    * PC relative addressing
      * assembler determine disp to target of jump from cur addr
      * singed number code into instru
      * exec RJMP, add +disp from instru into cur PC value 
      * -> new PC value, target
  * IJMP  ; Z
  * EIJMP ; EIND:Z
  * JMP   ; 4M words address-able, 2 w/c, 2^22=4Million word address
* Exec conditional branch no change any SREG flag
  * can have a sequence of branch instru rely on single SREG setting
* address mode, if true, dest address = PC - relative
  * range [-64,63] words from instru follow conditional branch
  * disp is calculated by assembler and store as 7-bit 2's complement int
* AVR only have two cond branch instru base on SREG
  * BRBC if SREG bit clear
  * BRBS if SREG bit set
  * 0-C 1-Z 2-N 3-V 4-S 5-H 6-T 7-I
  ```
  ; SREG - Status Register
  .equ  SREG_C  = 0 ; Carry Flag
  .equ  SREG_Z  = 1 ; Zero Flag
  .equ  SREG_N  = 2 ; Negative Flag
  .equ  SREG_V  = 3 ; Two's Complement Overflow Flag
  .equ  SREG_S  = 4 ; Sign Bit
  .equ  SREG_H  = 5 ; Half Carry Flag
  .equ  SREG_T  = 6 ; Bit Copy Storage
  .equ  SREG_I  = 7 ; Global Interrupt Enable
  ```

* Table 9.1 AVR Conditional Branch Instructions Based on the SREG Flags
  - CP Rd, Rr = Rd - Rr = R_dst - R_src
  - alias

Syntax   | Operands     | Action | Description
--       | --           | --     | --
BRBC s,k | bit-no, addr | PC<-SREG(s)?<br>PC+1:PC+1+k | branch if SREG-s 0
BRBS s,k | bit-no, addr | PC<-SREG(s)?<br>PC+1:PC+1+k | branch if SREG-s 1
BRCC/S k | addr         | brbc/s 0,k | if carry clear/set
BRSH/LO k| addr         | brbc/s 0,k | if same_or_higher/lower (uint)
BRNE/EQ k| addr         | brbc/s 1,k | if not_equal/equal
BRPL/MI k| addr         | brbc/s 2,k | if plus/minus
BRVC/S k | addr         | brbc/s 3,k | if oVerflow clear/set
BRGE/LT k| addr         | brbc/s 4,k | if greater_or_equal/less (signed)
BRHC/S k | addr         | brbc/s 5,k | if half carry clear/set
BRTC/S k | addr         | brbc/s 6,k | if T flag is clear/set
BRID/E k | addr         | brbc/s 7,k | if global interrupt disabled/enabled

### 9.2.1 Comparisons

* compare instru to set SREG before cond branch
* no change GPR
* only affect SREG
* arithmetic and logical instru also affect SREG
* cmp instru must do before cond branch
* tst not always necessary

* Table 9.2 Compare and TST Instructions that Ofen Precede a Conditional Branch

Syntax    | Operands     | Action         | Description
--        | --           | --             | --
CP Rd,Rr  | dst, src     | SUB Rd - Rr    | Compare Rd to Rr
CPC Rd,Rr | dst, src     | SBC Rd - Rr -C | with carry
CPI Rd,Rr | Rh[16,31], k8| SBI Rd - k     | Rd with immediate constant
TST Rd    |              |                | test for zero or minus

* TST
  - Z -> NREQ/NE
  - N -> BRMI/PL
* INC/DEC not affect SREG_C
  * BRCC/S
  * BRSH/LO
* BRSH/LO unsinged numbers
* BRGE/LT signed numbers

* logic instru -> SREG_Z BREQ/NE
* shift/rotate -> SREG_C BRCC/S
  * multiply/divided by 2 -> S/V/N

### 9.2.2 Conditional Skip Instru

* If condition flase: PC = PC + 1 (sequence, no skip)
* If condition true:  PC = PC + 2/3 (skip)
 
  - Table 9.3 Conditional Skip Instructions
  - None affect SREG/GPR

Syntax     | Operands     | Action                        | Description
--         | --           | --                            | --
CPSE Rd, Rr | Rd, Rr       | PC<-(Rd=Rr)?<br>PC+2(3):PC+1  | if = skip
SBIC A, b   | SFRL/REG_IO[0,31]<br>bit no.[0,7] | PC<-(SREG(b)=1)?<br>PC+2(3):PC+1 | if SFRL/SREG_IO bit-no. clr
SBIS A, b   | SFRL/REG_IO[0,31]<br>bit no.[0,7] | PC<-(SREG(b)=1)?<br>PC+2(3):PC+1 | if SFRL/SREG_IO bit-no. set
SBRC A, b   | GPR[0,31]<br>bit no.[0,7] | PC<-(Rr(b)=1)?<br>PC+2(3):PC+1 | if GPR bit-no. clr
SBRS A, b   | GPR[0,31]<br>bit no.[0,7] | PC<-(Rr(b)=1)?<br>PC+2(3):PC+1 | if GPR bit-no. set

## 9.3 Selection

* common selection struction
  * statement
    * if
    * if/else
    * switch
    * select case
  * 1 or 2 expr -> evaluate via logic condition
    * restricted version of if/else structure
    * condense to single expr evaluation semantic

### 9.3.0 IF-SKIP
* IF
  * true, exec then-part (>= one instru)
  * flase, skip
  * only one entry point (when condition evaluated)
  * one exit (after then-part)
  * asm
    * reverse/negate the condition
    * action skip/branch around then-part when [original cond] = false

  - TABLE 9.4 Translating IF Structures to Assembly Language
  - var a/b = unsinged byte GPR
  - var m/n = signed   byte GPR
  
type| High Level        | Assembly Equivalent
----| ------------------| --
uint| if (a>=b)<br>b++; | CP a,b ;skip if a<b <br>BRLO skip<br>INC b<br>skip:
uint| if (a>b)<br>a--;  | CP b,a ;skip if a<=b,b>=a <br>BRSH skip<br>DEC a<br>skip:
sint| if (m>=n)<br>m-=n;| CP m,n ;skip if m<n<br>BRLT skip<br>sub m,n<br>skip:
sint| if (m>n)<br>m-=n; | CP n,m ;skip if m<=n,n>=m<br>BRSH skip<br>sub m,n<br>skip:
sint| if (m==0)<br>n=0; | TST m  ;skip if m!=0<br>BRNE skip<br>CLR n
k-int| if (a>9)<br>a=0; | CPI a,10 ;skip if a<=9,a<10<br>BRLO skip<br>CLR a<br>skip:
sint| if (m==n)<br>a=255;| CPSE m,n ;if m==n,skip next<br>RJMP skip ;skip  m!=n<br>LDI a,255<br>skip:
sint| if (m!=n)<br>a=255;| CPSE m,n ;skip if m==n<br>LDI a,255
sint| if (m!=n+1)<br>a=255;| PUSH n<br>INC n ;tmp expr val (n+1)<br>CPSE m,n ;skip if m==n+1<br>LDI a,255<br>POP n ; restore n

### 9.3.1 IF-ELSE


