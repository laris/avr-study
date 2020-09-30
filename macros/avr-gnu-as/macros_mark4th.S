; macros.s
; -----------------------------------------------------------------------

 #define tl  r20            // top of stack
 #define th  r21
 #define wl  r22            // working register
 #define wh  r23
 #define ipl r24            // interpretive pointer
 #define iph r25

 #define _IO _SFR_IO_ADDR   // i dont want _EXTRALONGBULLSHITMACRONAMES

; -----------------------------------------------------------------------

; x = parameter stack pointer
; y = return stack pointer
; z = gp pointer

; -----------------------------------------------------------------------

  .set lex, 0               ; marks next word as immediate, alias etc
  .set _thread, 0           ; link to previous word in current vocabulary

  .set forth_link, 0        ; links to previous word in each vocab
  .set comp_link, 0
  .set root_link, 0
  .set e_link, 0            ; chain of eeprom variables

  .set _voclink, 0          ; assembly time voc linkage

; -----------------------------------------------------------------------

  .set FORTH, 1
  .set COMPILER, 2
  .set ROOT, 3

; -----------------------------------------------------------------------

  LEXMASK = 0x3f
  IMM     = 0x80
  ALIAS   = 0x40
  BODY    = 2

; ------------------------------------------------------------------------
; make next assembled word an immediate word

.macro _imm_
  .set lex, IMM
.endm

; -----------------------------------------------------------------------

.macro next
  jmp _next
.endm

; -----------------------------------------------------------------------

.macro hstring name
  .byte lex + 9f-(.+1)
  .ascii "\name"
9:
  .balign 2
.endm

; -----------------------------------------------------------------------

.macro wrd addr
  .byte lo8(\addr + 0x8000)
  .byte hi8(\addr + 0x8000)
.endm

; -----------------------------------------------------------------------

.macro header name, cfa
  wrd _thread  
  .set _thread, .
  hstring "\name"
  .set lex, 0
  wrd \cfa   ; note: can probably delete this link
  wrd _thread
.endm

; -----------------------------------------------------------------------

.macro xt xx q:vararg
  wrd \xx 
  .ifnb \q
    xt \q
  .endif
.endm

; -----------------------------------------------------------------------

.macro code name, cfa
  header "\name", \cfa
\cfa:
.endm

; -----------------------------------------------------------------------

.macro colon name, cfa
  header "\name", \cfa
\cfa:
  jmp nest
.endm

; -----------------------------------------------------------------------

.macro lit name, value
  xt plit
  .word \value
.endm

; -----------------------------------------------------------------------

.macro alias name, cfa, label
  .set lex, ALIAS
  header "\name", \cfa
  .set \label, \cfa
.endm

; -----------------------------------------------------------------------

.macro constant name, cfa, value
  header "\name", \cfa
\cfa:
  jmp doconstant
  .word \value
.endm

; -----------------------------------------------------------------------

.macro variable name, cfa, value
  header "\name", \cfa
\cfa:
  jmp dovariable
  .word b_\cfa
.section .data
b_\cfa\():
  .word \value
.section .text
.endm

; -----------------------------------------------------------------------

.macro var name, cfa, value
  header "\name", \cfa
\cfa:
  jmp dovar
  .word b_\cfa
.section .data
b_\cfa\():
  .word \value
.section .text
.endm

; -----------------------------------------------------------------------
; assemble a deferred word

.macro defer name, cfa, value
  header "\name", \cfa
\cfa:
  jmp dodefer
  .word b_\cfa              ; point to deferred words ram vector
  .word e_\cfa              ; ram vector is shadowed in eeprom
  .word e_link
  .set e_link, .
.section .data
b_\cfa\():
  .word \value              ; run time ram vector
.section .eeprom            ; above ram vector is initialized at boot
e_\cfa\():                  ; by copying this eeprom data to ram
  .word \value
.section .text
.endm

; -----------------------------------------------------------------------

.macro evar name, cfa, value
  header "\name", \cfa
\cfa:
  jmp dovar
  .word b_\cfa
  .word e_\cfa
  .word e_link
  .set e_link, .
.section .data
b_\cfa:
  .word \value
.section .eeprom
e_\cfa\():
  .word \value
.section .text
.endm

; -----------------------------------------------------------------------

.macro vocab name, cfa, thread
  header "\name", \cfa
\cfa:
  jmp dovoc
  .word e_\cfa
  .word _voclink
  .set voclink, \cfa
.section .eeprom
e_\cfa\():
  .word \thread
  .fill 15, 2, 0
.section .text
.endm

; =======================================================================