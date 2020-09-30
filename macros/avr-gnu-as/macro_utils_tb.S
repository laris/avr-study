.include "m328p_def.inc"
.include "macro_utils.inc"
/******************************************************************************/
# _get_bin
#_get_bin yes
#_get_bin false
/******************************************************************************/
# _get_symbol
#_get_symbol SREG_T, SREG_T, SREG_T, T
#_get_symbol T, SREG_T, SREG_T, T
#_get_symbol Tx, SREG_T, SREG_T, T
/******************************************************************************/
/******************************************************************************/
#_get_op tb
#_get_op 0
#_get_op 0x0
#_get_op r0
#_get_op R0

#_get_op 10
#_get_op r10
#_get_op R10
#_get_op 0xa

#_get_op 16
#_get_op 0x10
#_get_op r16
#_get_op R16

#_get_op 25
#_get_op r25
#_get_op R25
#_get_op xh

#_get_op 26
#_get_op 0x1a
#_get_op r26
#_get_op R26
#_get_op xl
#_get_op XL

#_get_op 0x19
#_get_op 0x1a
#_get_op 0x1b
#_get_op 0x1f
#_get_op 0x20
#_get_op 0x3F
#_get_op 0x5F
#_get_op SREG
#_get_op SPH
#_get_op PINB
/******************************************************************************/
