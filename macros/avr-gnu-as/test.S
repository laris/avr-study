#.altmacro
.macro  _test_altm arg1
        .print "arg1"
        .print "_$arg1"
        #.print "_\()arg1"
        .print "arg1$_"
.endm
#_test_altm 'a'
.altmacro
/*
.altmacro
use & \() as seperator to seperate argument apart from other connecting string
.print can print input args in string,
       cannot print local defined variable value in string, cannot evaluate the value
       .print only use & as seerator, will warning \()
\@ indicate how many times the macro called
! to escape any char
%expr to evaluate expr and use as string
.noaltmacro
& seperator
.print cannot print input args in string
*/
#.noaltmacro
.macro  _join_args arg1, arg2
        LDI \arg1\arg2, 0xFF
.endm
.macro  _convert_str arg
        __int = 0
        .print "%__int"
.endm
.macro  _test_arg_keyword arg1, arg2
        __local_var1 = 0
        #.print 'a'
        #.print "!a"
        #.print "__local_var1 = %__local_var1, _arg1=arg1,\t _&arg2=arg2"
        __r = r
        __rid = 16
        _join_args r, %__rid
.endm
#_test_arg_keyword a1, a2
#_test_arg_keyword arg1=9,arg2=10
#_test_arg_keyword
.macro  _altm_pass_arg cmd1, cmd2
        cmd1
        \cmd2
#        \cmd1
.endm
#_altm_pass_arg <INC R0>
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
        \opcode \arg1
        #, \arg2
.endm
.macro  _exec_opcode_tb
_exec_opcode 0, nop
_exec_opcode 1, inc, r0
_exec_opcode 2, ldi, r16, 0xff
_exec_opcode 2, std, Z+1, R0
.endm
.macro  _wrapper_opcode arg0, arg:vararg
#.macro  _wrapper_opcode arg0, arg
#.macro  _wrapper_opcode arg
        _exec_opcode_0a arg
        #arg
.endm
_wrapper_opcode 1, <add r0, r1>
#.macro  _call_altm_pass_arg arg1:vararg
.macro  _call_altm_pass_arg arg1
        #_altm_pass_arg <inc r0>, <dec r1>
        #_altm_pass_arg arg1
        _join_arg_sp arg1
.endm
#_call_altm_pass_arg <ldi,r16,0xff>
#<inc r0>