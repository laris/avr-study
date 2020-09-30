
* https://stackoverflow.com/questions/53340885/avr-assembly-load-a-16-bit-number-into-two-8-bit-registers

* https://ucexperiment.wordpress.com/category/avr-inline-assenbly/
* https://ucexperiment.wordpress.com/2016/03/10/arduino-inline-assembly-tutorial-4/

* https://www.nongnu.org/avr-libc/user-manual/inline_asm.html


## asm call convention
* https://stackoverflow.com/questions/20530052/asm-call-conventions
* http://www.agner.org/optimize/calling_conventions.pdf
* https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Conventions
* https://en.wikibooks.org/wiki/X86_Disassembly/Calling_Convention_Examples
* https://en.wikibooks.org/wiki/X86_Assembly/GAS_Syntax

## avr-as avr-ld miss ld rjmp issue
* https://stackoverflow.com/questions/1761197/how-can-i-jump-relative-to-the-pc-using-the-gnu-assembler-for-avr
* gcrt https://www.avrfreaks.net/forum/gcc-crt0s-linker-script
* https://www.avrfreaks.net/forum/avr-and-bss-section?skey=avr-ld
* https://www.avrfreaks.net/forum/gnu-assemblerproblem-addressing?skey=avr-ld
* https://www.avrfreaks.net/forum/asm-strange-brne-behavior?skey=avr-ld
* https://www.avrfreaks.net/forum/how-build-pure-asm-source-using-avr-toolchain?skey=avr-ld
* https://www.avrfreaks.net/forum/relative-jump-absolute-address?skey=avr-ld
* https://www.avrfreaks.net/forum/programming-and-customizing-avr-gadre?skey=avr-as%20PC
  - The set of GCC based tutorials in the Tutorial Forum here is one of the best guides you are going to find about using avr-gcc in fact.
* https://www.avrfreaks.net/forum/avr-ld-bags-jmpcall-opcodes?skey=avr-ld
  - avr-ld -mavr5 --oformat binary -o foo.bin foo.o
  - avr-ld -mavr5 -o foo.elf foo.o
  - avr-objcopy -O binary foo.elf foo.bin
* https://www.avrfreaks.net/forum/how-do-i-tell-avr-ld-put-data-section-sram?skey=avr-ld
* https://www.avrfreaks.net/forum/assembler-compiler-and-linker-questions?skey=avr-ld

## Misc
* https://www.avrfreaks.net/forum/gas-and-macros?skey=gas%20nest
* https://www.avrfreaks.net/forum/dynanamic-memory-allocation-what-malloc-does
