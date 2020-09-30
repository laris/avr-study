; https://www.avrfreaks.net/forum/avr-spm-instruction-usage-peculiar-behavior-soliciting-input?page=all

.nolist
;.include "usb1286def.inc"  ;128K FLASH, 8K DATA, 4K EEP, USB
.include "m32u4def.inc"    ;32K FLASH, 2.5K DATA, 1K EEP, USB

#define    TEST  1

;-the routine writes one page of data from RAM to Flash
; the first data location in RAM is pointed to by the Y pointer
; the first data location in Flash is pointed to by the Z-pointer
;-error handling is not included
;-the routine must be placed inside the Boot space
; (at least the Do_spm sub routine). Only code inside NRWW section can
; be read during Self-Programming (Page Erase and Page Write).
;-registers used: r0, r1, temp1 (r16), temp2 (r17), looplo (r24),
; loophi (r25), spmcrval (r20)
; storing and restoring of registers is not included in the routine
; register usage can be optimized at the expense of code size
;-It is assumed that either the interrupt table is moved to the Boot
; loader section or that the interrupts are disabled.

.include "Macro_Misc.inc"
.include "Macro_IO.inc"

.list

;define register names
  .def  temp1    = r16
  .def  temp2    = r17
  .def  spmcrval  = r20
  .def  looplo    = r24
  .def  loophi    = r25

  .equ   PAGESIZEB   = PAGESIZE*2  ;PAGESIZEB is page size in BYTES, not words

;===============================================================================
  .dseg
  .org  SRAM_START

BUFFER:
  .byte  256              ;reserve 256 bytes for buffer  

;===============================================================================
  .cseg
  .org 0  ;start of FLASH memory

  jmp    RESET        ; Reset vector

;===============================================================================
  .org PC + (PAGESIZE - (PC % PAGESIZE)); set program counter for page alignment
SPM_DATA:    ; reserve FLASH memory for data storage (MCU specific)
#if TEST
  .dd  0x03020100,0x07060504,0x0B0A0908,0x0F0E0D0C
  .dd  0x13121110,0x17161514,0x1B1A1918,0x1F1E1D1C
  .dd  0x23222120,0x27262524,0x2B2A2928,0x2F2E2D2C
  .dd  0x33323130,0x37363534,0x3B3A3938,0x3F3E3D3C
#else
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ;minimum of 32 words reserved
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ; for all parts.  For 8KB parts
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ;  PAGESIZE=32 or 64 bytes.
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
#endif
.if 2*FLASHEND > 8192  ; for 16KB+ parts 
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ;additional reserved words 
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ; for 16+KB parts as
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ; or PAGESIZE=64 (128 bytes)
  .dd  0x00000000,0x00000000,0x00000000,0x00000000  ; or PAGESIZE=128 (256 bytes)
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
  .dd  0x00000000,0x00000000,0x00000000,0x00000000
.endif

;===============================================================================
.org SMALLBOOTSTART
;.org PC + (PAGESIZE - (PC % PAGESIZE)); set program counter for page alignment
Reset:
  ldi    temp1,LOW(RAMEND)  ;init SP
  out    SPL,temp1
  ldi    temp1,HIGH(RAMEND)
  out    SPH,temp1

  ;SetZPtr  2*SPM_DATA      ;point Z to SPM_DATA
  ldi    ZL,low(2*SPM_DATA)    ;point Z to data in RAM buffer
  ldi    ZH,high(2*SPM_DATA)
  rcall  Erase_Page        ;erase the page

  rcall  Page_Fill        ;fill page from RAM buffer

;  ldi    ZL,low(2*SPM_DATA)    ;point Z to data in RAM buffer
;  ldi    ZH,high(2*SPM_DATA)
;  rcall  Erase_Page        ;erase the page

  rcall  Write_page        ;write the buffer to FLASH

  rcall  Readback        ;read back the data

Loop:  ;end of the line
  nop
  rjmp  Loop

;-------------------------------------------------------------
Error:
  nop
  rjmp  Error

;===============================================================================
Page_Fill:
  ; Page Erase
;  ldi   spmcrval, (1<<PGERS) | (1<<SPMEN)
;  call   Do_spm
  ; re-enable the RWW section
;  ldi   spmcrval, (1<<RWWSRE) | (1<<SPMEN)
;  call   Do_spm

  ; transfer data from RAM to Flash page buffer
  ldi   looplo, low(PAGESIZEB)  ;init loop variable
  ldi   loophi, high(PAGESIZEB)   ;not required for PAGESIZEB<=256

  ;SetYPtr  BUFFER        ;point Y to data in RAM buffer
  ldi  YL,low(BUFFER)        ;point Y to data in RAM buffer
  ldi  YH,high(BUFFER)

  ;SetZPtr  0          ;point Z to start of temporary buffer
  clr    ZH            ;point Z to start of temporary buffer
  clr    ZL

Page_Fill1:
  ld    r0, Y+
  ld    r1, Y+
  ldi   spmcrval, (1<<SPMEN)
  call   Do_spm
  adiw   ZH:ZL, 2
  sbiw   loophi:looplo, 2    ;use subi for PAGESIZEB<=256
  brne   Page_Fill1

  ; execute Page Write
;  subi   ZL, low(PAGESIZEB)      ;restore pointer
;  sbci   ZH, high(PAGESIZEB)    ;not required for PAGESIZEB<=256

  ret

;===============================================================================
Write_Page:
  ;SetZPtr  2*SPM_DATA      ;point Z to SPM_DATA
  ldi    ZL,low(2*SPM_DATA)      ;point Y to data in RAM buffer
  ldi    ZH,high(2*SPM_DATA)

  ldi   spmcrval, (1<<PGWRT) | (1<<SPMEN)
  call   Do_spm

  ; re-enable the RWW section
  ldi   spmcrval, (1<<RWWSRE) | (1<<SPMEN)
  call   Do_spm

  ret

;===============================================================================
Readback:
  ; read back and check, optional
  ldi   looplo, low(PAGESIZEB)    ;init loop variable
  ldi   loophi, high(PAGESIZEB)   ;not required for PAGESIZEB<=256

  subi   YL, low(PAGESIZEB)      ;restore pointer
  sbci   YH, high(PAGESIZEB)

Readback1:
.ifdef RAMPZ
  elpm   r0, Z+
.else
  lpm   r0, Z+
.endif
  ld    r1, Y+
  cpse   r0, r1
  jmp   Error
  sbiw   loophi:looplo, 1    ;use subi for PAGESIZEB<=256
  brne   Readback1

  ret

;===============================================================================
Erase_Page:
  ; Page Erase
  ldi   spmcrval, (1<<PGERS) | (1<<SPMEN)
  call   Do_spm
  ; re-enable the RWW section
  ldi   spmcrval, (1<<RWWSRE) | (1<<SPMEN)
  call   Do_spm

  ret

;===============================================================================
;!!!!!  Why is this here?  It is never used!
; return to RWW section
; verify that RWW section is safe to read
;Return:
;  in    temp1, SPMCSR
;  sbrs   temp1, RWWSB
; If RWWSB is set, the RWW section is not ready yet
;  ret

; re-enable the RWW section
;  ldi   spmcrval, (1<<RWWSRE) | (1<<SPMEN)
;  call   Do_spm
;  rjmp   Return
;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

;-------------------------------------------------------------
Do_spm:      ; check for previous SPM complete
Wait_spm:
  in    temp1, SPMCSR
  sbrc   temp1, SPMEN
  rjmp   Wait_spm
; input: spmcrval determines SPM action
; disable interrupts if enabled, store status
  in    temp2, SREG
  cli
; check that no EEPROM write access is present

Wait_ee:
  sbic   EECR, EEPE
  rjmp   Wait_ee
; SPM timed sequence
  out   SPMCSR, spmcrval
  spm
; restore SREG (to enable interrupts if originally enabled)
  out   SREG, temp2
  ret


