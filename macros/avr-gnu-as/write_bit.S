  .text
  .global main

  .equ valueLow,  16  ;r16
  .equ valueHigh, 17  ;r17
  .equ TIFR,     0x38
  .equ OCF1B,    3
  .equ OCR1AL,   0x2A
  .equ OCR1AH,   0x2B

.macro   writebit1
   in   valueLow,TIFR
   andi   valueLow,(1<<OCF1B)
   breq   .-6             ; loop until OCF1B set
   out   TIFR,valueLow
   out   OCR1AH,r0   
   out   OCR1AL,valueHigh
.endm

.macro   writebit2
1:
   in   valueLow,TIFR
   andi   valueLow,(1<<OCF1B)
   breq  1b             ; loop until OCF1B set
   out   TIFR,valueLow
   out   OCR1AH,r0   
   out   OCR1AL,valueHigh
.endm

.macro   writebit3 label
.Lwritebit3\label:
   in   valueLow,TIFR
   andi   valueLow,(1<<OCF1B)
   breq  .Lwritebit3\label             ; loop until OCF1B set
   out   TIFR,valueLow
   out   OCR1AH,r0   
   out   OCR1AL,valueHigh
.endm

.macro   writebit4
.Lwritebit4\@:
   in   valueLow,TIFR
   andi   valueLow,(1<<OCF1B)
   breq  .Lwritebit4\@             ; loop until OCF1B set
   out   TIFR,valueLow
   out   OCR1AH,r0   
   out   OCR1AL,valueHigh
.endm


.altmacro
.macro   writebit5
LOCAL label
label:
   in   valueLow,TIFR
   andi   valueLow,(1<<OCF1B)
   breq  label             ; loop until OCF1B set
   out   TIFR,valueLow
   out   OCR1AH,r0   
   out   OCR1AL,valueHigh
.endm

main:

  writebit1
  writebit1

  writebit2
  writebit2

  writebit3 1
  writebit3 2

  writebit4
  writebit4

  writebit5
  writebit5

  .end