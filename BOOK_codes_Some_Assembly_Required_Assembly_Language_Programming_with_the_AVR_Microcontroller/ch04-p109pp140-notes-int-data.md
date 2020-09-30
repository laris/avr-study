
* raw data
  * bit
  * byte
  * word
  * etc...

* Native data type = raw logical/binary data
  * 1 = true
  * 0 = false
* all computation are carried out at raw logical(binary) data

## 4.1 unsigned integer data
* store in fixed-size container
  * byte
  * word
* 8-bit uint and 32-bit uint but similar data types
* expressing
  * base 2
  * append leading 0
  * store binary digits as bits
* 8-bit container 
  * 2^8 = 256 distinct values
  * range DEC[0,255]
* n-bit
  * 2^n distinct values

* conversion diff-sized uint
  * add lead-0
  * remove lead-bits
    * nonzero bits removed -> fit value -> smaller container
    * truncation = value change 
    * truncated value mathematically = original value modulo some power of 2
    * uint16 48075 -> uint8 203
    * 48075 % 256 = 203

  * fixed-size container
    * adv vs dis-adv
      * convenient computationally
      * overflow
      * limit restricted range
  * variable size number
    * complex to represent
    * less efficient computationally
    * have place in particular situation
  * Process native support fixed-size but not for variable

* ALU circuitry perform unsigned
  * addition
  * subtraction
  * multiplication
  * division

  * add/sub 
    * usually produce a result of same size as operands
    * result overflow
    * produce arithmetic result modulo a power of 2 via truncation
      * indicate overflow

  * unsigned arithmetic, lager - small num, like 1-2
    * operation leagal as ALU concerned
    * result, simulating a borrow from next higer bit possible (outside of container)
    * result is correct = modulo a power of two
    * simulate borrow is flagged as unsigned overflow

* Encode number
  * encode number in supported number code as numberic data type
  * place into mem or reg

* assembler, data definition directive
  * generate uint codes 
    * any number  by
    * using literal or expr as the operand

  * value of expr
    * evaluated using 64-bits, 
    * encode as desired size uint

  * assembler cannot 
    * gen constant data to dseg, only flash/eeprom

  * literal number
    * 0b binary
    * o207 oct
    * 0x or $ hex
      * easy way to express in hex
    * '9' string

  * assembler will gen hex format
    * display byte via word as unit
    * listing file

  * expr
    * high (45239)
  * assembler translate the result of any expr 
    * represent non-negative num
    * into unsigned format
    * incorporated in assembly process
  * notation

* 16-bit addition/SUB with k instr
  * ADIW Rd+1:Rd, k
  * SBIW Rd+1:Rd, k
  * Rp = [24,30]
  * immediate k  = [0,63]
  * high byte -> odd (higher) reg
  * low  byte -> even (lower) reg
  * little-endian
    * high byte in high mem
  * ov
    * illegal substraction, unsigned answer = negative
    * result modulo 256 or 65535
    * carry flag set

* 16-bit add/sub instr with regs
  * ADD ADC
  * SUB SBC
  * no reason that two bytes of each word be in adjacent regs
  * ADC, Z flag only reflect the 2nd addition 
  * SBC, Z flag always correctly represent the status of 16-bit result
    * if high byte = zero, Z =1
    * if low  byte != zero, Z still =1

