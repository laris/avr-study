* Native data type = raw logical/binary data
  * 1 = true
  * 0 = false
## 4.1 unsigned integer data
* store in fixed-size container
  * byte
  * word
* 8-bit uint and 32-bit uint
* expressing
  * base 2
  * append leading 0
  * store binary digits as bits
* 8-bit container 
  * 256 distinct value
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
* ALU for add/sub
  * overfolow
  * produce arithmetic result modulo a power of 2 via truck
  * indicate overflow
  * both for add/sub

* Encode by assembler
  * encode number in supported number code as numberic data type
  * place into mem or reg
  * assembler accept any number by ussing literal or expr as the operand for data define directive, generate uint code
  * value of expr is evaluated using 64-bits, encode as desired uint size
  * assembler cannot gen constant data to dseg, only flash/eeprom
  * easy way to express in hex
  * verify, assemble .dw code, check lst file show word in hex
  * 