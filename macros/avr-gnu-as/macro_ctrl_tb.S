.include "macro_all.inc"
/******************************************************************************
Testbench
*******************************************************************************/
.macro _if_tb
nop
_if <cp r0,r1>, eq /* [blank=endif/default] */
#    _then /* optional */
    inc r0
_Ethen Eif/* [blank=jmp to Eif,Eif=skip jmp endif] */
#_Eif
_Eif
nop
.endm
#_if_tb

.macro _if_else_tb
nop
_if <cp r0,r1>, <eq>, else /* [endif=default,else,elif] */
    #_then     /* optional */
    inc r0
_Ethen /* must when next selection exist, aim to jmp to last _Eif*/
_else         /* must when _if fail to else, because need jmp point label */
    lpm
_Eelse
_Eif        /* must */
nop
.endm
#_if_else_tb

.macro _if_elif_tb
nop
_if <cpi r16,0>, <eq>, elif /* [endif=default,else,elif] */
    #_then
    lpm
_Ethen  /* must when next selection exist, aim to jmp to last _Eif*/
_elif <cpi r16,1>, <eq>, elif /* _elif [endelif=default, else, (last) endif] */
    lpm
_Eelif
_elif <cpi r16,1>, <eq>, Eif
    lpm
#_Eelif
_Eelif Eif /* _Eelif [blank=(last) endif=default, endif=skip jmp] */
_Eif
nop
.endm
#_if_elif_tb

.macro _if_elif_else_tb
nop
_if <cpi r16,0>, <eq>, elif
    LPM
_Ethen
_elif <cpi r16,1>, <eq>, elif
    INC R0
_Eelif
_elif <cpi r16,1>, <eq>, else
    LPM
_Eelif
_else
    LPM
_Eif
nop
.endm
#_if_elif_else_tb

.macro _if_if_Eif_Eif_tb
nop
_if <cpi r16,0>, <eq>
    inc r0
    _if <cp r20,r21>, <eq>
      dec r1
      _if <cp r20,r21>, <eq>
        COM R2
      _Eif
      DEC R3
    _Eif
    LSR R4
_Eif
nop
.endm
#_if_if_Eif_Eif_tb

.macro _if_if_else_tb
nop
_if <cpi r16,0>, <eq>, else
        inc r0
    _if <cp r20,r21>, <eq>
        #_then
        dec r1
        #_Ethen endif
    _Eif
_Ethen
_else
    dec r3
_Eif
nop
.endm
#_if_if_else_tb

.macro _if_if_elif_else_tb
nop
_if <cpi r16,0>, <eq>, elif
        inc r0
    _if <cp r20,r21>, <eq>
        LPM
    _Eif
#  _Ethen
_elif <cpi r16,1>, <eq>, elif
    dec r1
    _if <cp r20,r21>, <eq>
        DEC R1
    _Eif
_Eelif
_elif <cpi r16,1>, <eq>, else
    dec r2
_Eelif
_else
    dec r3
_Eif
nop
.endm
#_if_if_elif_else_tb

.macro _if_all_tb2
_if <CPI R16,0>, eq, elif
        LSR R1
    _if <CPI R17,2>, eq, else
        LSR R3
    _else
        LSR R4
    _Eif
_Ethen
_elif <CPI R18,5>, eq, elif
        LSR R6
    _if <CPI R19,7>, eq, else
        LSR R8
    _else
        LSR R9
    _Eif
_Eelif
_elif <CPI R20,10>, eq, else
        LSR R11
_Eelif
_else
        LSR R12
    _if <CPI R19,13>, eq, else
        LSR R14
    _else
        LSR R15
    _Eif
_Eif
.endm
#_if_all_tb2
/*****************************************************************************/
.macro _if2_tb
nop
ldi r20,1
ldi r21,1
ldi r22,2
ldi r23,3
ldi r24,0
_if2 <cp r20,r21>, sh, or, <cp r22,r23>, lt
    inc r3
_Eif
nop
_if2 <cp r20,r21>, ge, or, <cp r22,r23>, lo
    inc r3
_Eif
nop
.endm
#_if2_tb

.macro _if2_else_tb
nop
ldi r20,1
ldi r21,1
ldi r22,2
ldi r23,3
ldi r24,0
nop
_if2 <cp r20,r21>, sh, or, <cp r22,r23>, lt
    inc r3
_Eif
nop
_if2 <cp r20,r21>, sh, or, <cp r22,r23>, lt
    inc r3
_else
    dec r4
_Eif
nop
nop
_if2 <cp r20,r21>, eq, and, <cp r22,r23>, eq
    inc r3
_else
    dec r4
_Eif
nop
.endm
#_if2_else_tb

/*****************************************************************************/
.macro _ifandor2_tb
_if <cp r20,r21>, eq, _JP_OR_DO, ,dj
  _then
  _if <cp r30,r31> eq
    _then
_JP_OR_DO:
    COM R0
  _Eif
_Eif
#_ifand2 <cp r20,r21>, <eq>, <cp r30,r31>, eq
#    inc r0
#_Eifand2
NOP
NOP
NOP
NOP
#_ifand2
_if <cp r20,r21>, eq, ,_
  _then
  _if <cp r30,r31> eq
    _then
    COM R0
  _Eif
_Eif

#_ifor2 <cp r20,r21>, eq, <cp,r30,r31> eq
#    dec r1
#_Eifor2
.endm
#_ifandor2_tb

/*****************************************************************************/
.macro _if_bit_tb
NOP
_if_bit R0,    0, set, .
NOP
_if_bit PORTB, 0, set, .
NOP
_if_bit 0x40,  0, set, .
NOP
_if_bit SREG,  0, set, .
NOP
_if_bit 0x60,  0, set, .
NOP
.endm
#_if_bit_tb
#
.macro _ifb_true_skip_tb
NOP
_ifb_true_skip R0,    0, set
NOP
_ifb_true_skip PORTB, 0, set
NOP
_ifb_true_skip 0x40,  0, set
NOP
_ifb_true_skip SREG,  0, set
NOP
_ifb_true_skip 0x60,  0, set
NOP
.endm
#_ifb_true_skip_tb
#

/*****************************************************************************/
.macro _jtb_tb
NOP
_jtb_gen_tbl 5
NOP
_jtb_case_do 0
        INC R0
_jtb_case_end 0
NOP
_jtb_case_do 1
        DEC R0
_jtb_case_end 1
NOP
#_jtb_case_do deflt # no useful for default
#        CLR R0
#_jtb_case_end
_jtb_gen_batch_cases <nop>, 2,3,4
        NOP
        LDI R16,0 /* jmp to case 0 */
_jtb_switch r16
        NOP
        LDI R16,1 /* jmp to case 0 */
_jtb_switch r16
        NOP
        LDI R16,2 /* jmp to case 0 */
_jtb_switch r16
        NOP
#_jtb_switch 0, 0, cs, <add zh,r0>
#_jtb_switch 0, 0, cs, <add r0,r1>
#_jtb_switch %!<ADI ZL,1!>, cs
        NOP
#_jtb_switch "<CP R0,R1>", eq, 1, 0
        NOP
_jtb_end
NOP
.endm
#_jtb_tb
/*****************************************************************************/

.macro _do_while_tb
DELAYTEST:
  ldi r27, 100
  _do
    ldi r28, 0
    _do
      nop
      nop
      dec r28
      _while nz
    dec r27
    _while nz
  nop
.endm
#_do_while_tb

.macro _loop_while_break_tb
LOOPWBREAKTEST:
  clr r10
  _do
    ld r17, X+
    and r17, r17
    _break e
    inc r10
    _while always
    nop
    nop
.endm
_loop_while_break_tb
