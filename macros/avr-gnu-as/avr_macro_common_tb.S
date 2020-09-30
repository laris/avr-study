##define __AVR_ATtiny85__
#define __AVR_ATmega328P__
/* default offset 0x20 */
##define __SFR_OFFSET  0x20
#include <avr/io.h>
##define __DEBUG_GET_PRT_TYPE__ 1
##define __DEBUG_GET_AADDR__ 1
#define __DEBUG_COMMON__
#include "avr_macro_common.S"

/*****************************************************************************/
/*
Assembler Special Characters / Syntax
ERR, Inline comment char:'@'
ERR, Line comment char:'#'
ERR, Statement separator:';'
ERR, Immediate operand prefix:'#' or'$'
*/
#LDI R16,0xFF ;comment
#LDI R16,007

#_str_concat NO,P
#_strc NO,P
#.altmacro
#_strc L,PM
#_strc_vararg 1

;LPM
;__id_test = 100
;_gen_jp5a __jp_, %__id_test, _, test
;NOP
;LPM
;NOP

/*****************************************************************************/
.macro  _exec_opcode argc, opcode, arg1, arg2
        .if (\argc <= 1)
            \opcode \arg1
        .elseif (\argc == 2)
            \opcode \arg1, \arg2
        .endif
.endm
.macro _exec_opcode_0a opcode
        \opcode
.endm
.macro _exec_opcode_1a opcode, arg1
        \opcode \arg1
.endm
.macro _exec_opcode_2a opcode, arg1, arg2
        \opcode \arg1, \arg2
.endm
.macro  _exec_opcode_tb
_exec_opcode 0, nop
_exec_opcode 1, inc, r0
_exec_opcode 2, ldi, r16, 0xff
_exec_opcode 2, std, Z+1, R0
.endm
/*****************************************************************************/
.macro _arg_test arg1:req, arg2=0, arg3, arg4=1, arg5:vararg
        .print "_arg1= arg1, _arg2= arg2, _arg3= arg3, _arg4= arg4, _arg5= arg5"
        .ifc \arg2,_
          .print "arg2 _"
        .endif
.endm _if_tb
#_arg_test a1
#_arg_test a1, arg2=1, arg5=abc
#_arg_test a1, 2,3,4,5,abc
## _arg1= a1, _arg2= 2, _arg3= 3, _arg4= 4, _arg5= 5,abc
# left blank can set to deflt
#_arg_test a1, , ,4,5,abc
##_arg1= a1, _arg2= 0, _arg3= , _arg4= 4, _arg5= 5,abc
#_arg_test a1,_ , ,4,5,abc
# so, only one way to pass args to macro, according positions

.macro _get_bin_tb
_get_bin 1
_get_bin true
_get_bin yes
_get_bin set
_get_bin 0
_get_bin False
_get_bin NO
_get_bin clear
_get_bin clr
_get_bin CLear
.endm
#_get_bin_tb