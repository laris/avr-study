
1.  List the five functional units of a typical computer system.
    - CU  Control Unit
    - ALU Arithmetic Logic Unit
    - CPU Central Processing Unit = CU + ALU
    - Memory main + secondary
      - RAM Radom Access 
      - ROM Read Only
      - EEPROM Electrically Erasable Programmable Read Only Mem
      - Tape Drive, CDROM, Hard Disk Drive
    - IO
2.  Which functional unit of a computer system oversees the fetch–execute cycle?
    - CU
3. Is main memory (primary storage) typically RAM or ROM? Why?
    - Fast
4. What is the fundamental distinction between the instructions in CISC and RISC architectures?
    - CISC: complexity, range from very simple to very complex instruction, RISC: easy to implement in hardware and executed very quickly
5. How does a microcontroller differ from a microprocessor.
   MC include MP and integrate specific IO dev and functionality on SOC, RAM/ROM/serial/parallel IO/Timer/etc.
6. What is the major distinction between the von Neumann and Harvard architectures?
    - Von Neumann: code/data via same addr/data buses
    - Harvard: seperate bus and allow parallel access to code and data to increase throughput, can fetch and decode the next instru same time current instru is read/write data in mem
7. Is a CDROM drive a random access or serial secondary storage device? Explain your answer. You may need to read about how these drives operate.
    - CDROM serial secondary storage dev because the data is volatile and cannot be manipulated by program
8. Is a mouse a serial or parallel input device? You may need to do some research to learn how a mouse communicates with a computer.
    mouse = serial, PS/2, GND/VCC/CLK/DATA, frame format: |1-start-bit-0|2-9/8/LSB|10-stop-1|11-ACK-host|
9.  Is a printer attached to a PC, a serial or parallel output device? Are all printers the same with respect to this classification? Explain.
    - 25P-LPT/A/B parallel, DB9, USB serial
10. Is 10 a numeral or a number? Could this represent the number of eyes most animals have? Could this represent the number of eyes found in a jumping spider (family Salticidae)? Explain.
    - binary 10 = 2, decimal 10, hex 0x10 = 15, with A, octal 10 = 8
11. How many bits in a byte? In a word? How many nybbles in a doubleword? How many bits in a quadword?
    - 2 x nybble  = 1 byte, 
    - 8 x bits    = 1 byte, 
    - 2 x bytes   = 1 word, 
    - 4 x bytes   = 1 dword,
    - 8 x bytes   = 1 quadword
12. Why is hexadecimal notation superior to octal when representing nybbles?
    - each hex digit represents a nybble and each nybble is a coefficient in base 16 polynomial
13. In what base is each expression equal to the number of bottles of root beer on the wall (one hundred)?
    - 1∗10000 + 2∗100 + 1
    - 9∗10 + 1
    - 2∗100 + 4∗10 + 4
    - 4∗100
    - 1∗100 + 4∗10 + 4
14. In the numeral 0x4C32CB, what digit is in position 4? What digit has a place value of 4096? If this numeral is translated to binary, how many 1’s will be in the equivalent numeral?
    - 0x4C-32-CB 0b0100_1100-0011_0010-1100_1011
15. Convert the following base 10 numerals to base two and base 16 using repeated division: 256, 32767, and 51983.
    - D256    = 0x100
    - D32767  = 0x7FFF
    - D51983  = 0xCB0F
16. Convert the following base five numerals to base 10 using polynomial evaluation: 10, 44, and 243.
    - 10 = 1 x 10^1
    - 44 = 4 x 10^1 + 4
17. Convert these hexadecimal values to binary in the simplest way possible: 100, 2F, and AC.
    - 0x100 = 0x1-0000-0000
    - 0x2F  = 0x0010-1111
    - 0xAC  = 0x1010-1100
18. Convert these octal values to binary and then hexadecimal without first converting to base 10: 377, 1037, and 4501.
    -
19. Using Horner’s Rule, show the step-by-step progress of converting the base seven numeral 62034 to base 10. Begin with zero; multiply by 7, add the next digit, and repeat.
    -
20. Convert these base 15 numerals to base 10 using Horner’s Rule. Process the digits from left to right. 9A2, 1000, and C07E.
    -
21. Using Horner’s Rule, show the steps to convert 734 (base 10) to base four. You will need to show all of your work in base four. Remember to convert the digits to base four before adding. You will need to know that 10, in base four, is written 22.
    -
22. Determine the result of performing a bitwise AND operation on this pair of nybbles: 0b1011 and 0b0100.
    - 0b  1011
    - 0b  0100
    - AND ----
    -     0000
23. Determine the result of performing a bitwise OR operation on this pair of nybbles: 0b1001 and 0b1100.
    - 0b  1001
    - 0b  1100
    - OR  ----
    -     1101
24. Determine the result of performing a bitwise XOR operation on this pair of nybbles: 0b1001 and 0b1101.
    - 0b  1001
    - 0b  1101
    - XOR ----
    -     0100
25. What mask and operation would be used to zero the upper nybble in a byte? To clear the lower nybble of a byte?
    - zero/clear = AND 0
26. Give the mask and operation needed to round odd numbers (represented by the binary value in a byte) down to the next even number (and not alter even numbers). How could you round up to the nearest odd number?
    - 
27. Shift each byte to the left and tell the result in hexadecimal: $3C, $E9, $FF.
    - 
28. What happens if you shift a byte to the right, and then shift it back to the left?
    - 
29. Devise a way to multiply a byte by 6 using only shifts and one addition.
    - 