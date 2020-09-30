/*----------------------------------------------------------------------------*/
.macro  __altmacro_pass_arg reg,input
       #LDI 16, 1<<TOV0|1<<TOV1
        LDI \reg,\input
.endm
/*----------------------------------------------------------------------------*/
.macro  __altmacro_pass_arg_tb
__altmacro_pass_arg r24,<1!<!<PB0>
__altmacro_pass_arg r24,<1!<!<PB0|1!<!<PB1>
__altmacro_pass_arg r24, 1&2
.endm
#__altmacro_pass_arg_tb
