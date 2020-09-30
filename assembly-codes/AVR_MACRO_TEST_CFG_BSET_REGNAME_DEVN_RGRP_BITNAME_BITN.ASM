.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IO.INC"
;-------------------------------------------------------------------------------
; test
        ; TCCR_0_A -> WGM_0_0
        ;CFG_BSET_REGNAME_DEVN_RGRP_BITNAME_BITN      TCCR,0,A,WGM,0
        ; TCCR_0_A -> COM_0_A_0
        ;CFG_BSET_REGNAME_DEVN_RGRP_BITNAME_BGRP_BITN TCCR,0,A,COM,A,0
