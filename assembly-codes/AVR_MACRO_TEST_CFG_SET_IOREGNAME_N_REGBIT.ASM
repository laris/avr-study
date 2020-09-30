.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IO.INC"
;-------------------------------------------------------------------------------
; PASS! test CFG_SET_IOREGNAME_1_REGBIT with one BIT list
;        CFG_SET_IOREGNAME_1_REGBIT TCCR0B,FOC0A
;        CFG_WR_IOREGNAME_TRH TCCR0B,R16
;-------------------------------------------------------------------------------
; PASS! test CFG_SET_IOREGNAME_N_REGBIT with 2 BIT list 
;        CFG_SET_IOREGNAME_N_REGBIT TCCR0B,2,FOC0A,FOC0B
;        CFG_WR_IOREGNAME_TRH TCCR0B,R16
;-------------------------------------------------------------------------------
