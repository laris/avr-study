EXERCISES

1. Name the three memories found in an AVR processor.
   - LDI, MOV, ADD
2. In which of the three memories are instructions (to be fetched and executed) stored?
   - ROM
3. What is meant by volatile memory? Which of the three AVR memories are volatile?
   - retain info as log as it supply with power. Register, RAM, ?
4. What is the usual length of an AVR machine instruction (in bits)?
   - 1 x word = 16 bits
5. The ATmega16A, a common member of the AVR family, has 16 KBytes of program memory (16,384 bytes). However, it has only 8 K distinct addresses (0-8191). Explain why this is OK.
   - AVR PROM, each instruction 1 x word store in flash memory with 1 x address
   - word addressing mode, 1 x word = 2 bytes -> 1 x address
6. How many general purpose registers are present in the AVR core? What is the size of each register (in bytes).
   - 32 x GPR
   - byte
7. What is the name of the special register that always contains the address of the next instruction to be executed?
   - PC Program Counter
8. If an AVR processor is running with a clock speed of 4 MHz, what is the maximum number of instructions it can execute in a single second?
   - 1/4 x 10^6 = 0.25 x 10^-6 = 250ns, 4 MIPS Million instructions per second
9.  The LDI instruction causes a certain bit pattern to be loaded into a register. How does the processor determine the bit pattern to be loaded? That is, where are the bits found?
    - LDI Rd, K 1110-bbbb-rrrr-bbbb
    - [16,31], [$00,$FF]
10. Why does the LDI instruction only work with half of the available general purpose registers? Which half can it affect?
    - rrrr, 4 x bits, 2^4=16, R[16,31]
11. The ADD instruction can utilize any general purpose register as the destination of the result (sum). What is different about the encoding of the ADD and LDI instructions that makes this possible?
    - ADD Rd, Rr 0000-11rd-dddd-rrrr
    - [0,31], [0,31]
    - r-rrrr/d-dddd, 5 x bits, 2^5=32, R[0,31]
12. Is there any difference between ADD R0, R1 and ADD R1, R0? Explain.
    - Rd save sum
13. How many bits is the opcode for the LDI instruction? For the ADD instruction?
    ref 9, 11
14. How similar are these two instructions: LSL R2 and ADD R2, R2? Explain.
    - 2 x R2
15. When the AVR processor is reset, what address is placed into the PC register? Do all processors work this way? Check on the Intel 8086 and the Motorola PowerPC.
    - $0 (or reset jmp vector addr)
16. The RJMP instruction used to create a very tight infinite loop uses a K value of -1. What would an RJMP with a K value of 0 do?
    - RJMP K 1100-kkkk-kkkk-kkkk
    - [-2048,2048) $800
    - PC = PC + 1 + K
    - RJMP/JMP require additional clock cycle to restock the pipline to ignore the unnecessary fetched instruction at RJMP-next-address when the PC execute at RJMP instruction.
    - PC mean the addr of the current instruction, not content of processsor's PC
    - 0x2 = 0x2 + 1 + K, so K=-1 => 0xFFF 1's
    - PC = PC + 1 + (-1) = 0x2
    - CU decode RJMP opcode 0b1100, setup internal data pathway to add together the number represented in 12-bits, save the result directly back to PC, discard the pipeline prefetched instruction, does nothing for next clock cycle as it wait for the instruction at addr 0x2 to be fetched and moved into the decode reg.
    - K=0, jmp to next
17. Hand assemble the following instructions. Express each machine language instruction in hexadecimal (4 hex digits)
    - a. LDI R20, 45
    - b. LDI R31, $C2
    - c. ADD R18, R19
    - d. ADD R0, R16
    - e. RJMP 27 (Jumps to PC + 1 + 27)
18. Decode the following instructions (shown in hexadecimal notation) to a machine instruction and its operands. Write your answer in a format similar to that of the previous question.
    - a. E43C
    - b. 0E2E
    - c. C002
    - d. ECE0
    - e. 0D00
19. What assembler directive can be used to assemble a machine language instruction if you know the instruction's binary representation?
    - .dw
    - assembly language program used a simple assembly language directive, .dw, to build our program.
20. What is an instruction mnemonic?
    - utilize the capabilities of the assembler by writing our program in machine instruction mnemonics and allow it to assemble the instruction words rather than doing the assembly by hand.
    - Instructions are indicated by a mnemonic, a memory aid, representing the machine instruction to be assembled
21. The AVR assembler stores the intended contents of flash memory in a file with what extension?
    - hex
22. What is the file called that contains a report of the assembly process, including error messages?
    - .lst
23. How is a label defined in an AVR assembly language program?
    - A label is an identifier representing a memory location.
24. When the assembler encounters a machine language mnemonic, it executes it. True or false? F
25. Illustrate the three ways a comment can be placed in an assembly language program.
    - ; comment
    - //
    - /*...*/
26. What directive indicates that the following statements are to be assembled into flash storage?
    - .dw
27. What directive is used to associate statements with SRAM?
    - .byte
28. Why is it important for address zero of flash to contain a valid (and meaningful) machine instruction?
    - PC execute 0 after reset
29. What directive is used to bind a meaningful symbolic name to a general purpose register? Why is this done?
    - .def
    - serves to give meaningful names (symbols) to hardware resources used by the program, making the meaning a little more clear. Registers R17 and R16 are renamed as temp and count to reflect their usage throughout this program.
30. What information is found in the map file?
    - contains a report generated from the symbol table which is an internal table created by the assembler.
    - As the assembler carries out its task, it builds a table of all the symbols defined in the program and the values assigned to them.
31. If the AVR processor uses a clock speed of 1 MHz, how many seconds will it take to execute a loop that counts from 0 to 255, assuming each loop iteration takes three clock cycles?
    - 1 MHz => 1^6Hz => 1us per cycle
    - 3 x 256 = 768 cycle = 768 us
32. If the AVR processor uses a clock speed of 2 MHz, how many seconds will it take to execute a loop that counts from 0 to 65535, assuming each loop iteration takes four clock cycles?
    - 4 x 65536 x 1/2 x 10^-6 s
33. If the AVR processor uses a clock speed of 1 MHz, how many loop iterations will be required to cause a 1 millisecond delay if each loop repetition requires five clock cycles? Can this be
accomplished with a 1-byte loop counter variable? Explain.
  - .

PROGRAMMING EXERCISES
1. Setup the STK-500 to run the sample program in Figure 2.11. Use a clock speed slow enough so you can observe each value. When the program starts, a zero is asserted on the output port. Are the LEDs on or off when a zero bit is asserted on the port? What is the next byte value to be asserted? What LEDs are on?
2. Again, referring to the sample program in Figure 2.11, if you add a second inc count instruction immediately after the existing increment, how would that change the program's behavior? Try it out in the simulator.
3. Write an assembly language program to add three numbers together. The program must begin by loading the three numbers into three distinct registers. The sum is to be calculated in register zero using ADD instructions. You might need to investigate the CLR instruction. Assemble and test your program in the simulator. Try several different numbers, keeping in mind that the sum cannot exceed 255 (if it is to be correct).
4. Determine the machine language version of the following program. Use the assembler to help.
ldi r16, 15
add r16, r16
rjmp PC-1

Rewrite the program without using any instruction mnemonics (use only .dw). Assemble it. Verify that both programs do exactly the same thing when tested in the simulator. List the successive and distinct values of register 16 as the program executes. Which instruction is the target of the RJMP instruction? Rewrite the original version of the program to use a label (in place of PC-1). Verify that the assembled program has exactly the same machine language words