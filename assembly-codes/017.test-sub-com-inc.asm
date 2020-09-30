.INCLUDE "M328PDEF.INC"
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
.INCLUDE "AVR_MACRO_MISC1.INC"

LDI R16, 111
LDI R17, 222
SUB R16,R17
NEG R16

NOP

LDI R16, 111
LDI R17, 222
SUB R16,R17
COM R16
INC R16

NOP
;-------------------------------------------------------------------------------
LDI R16, 222
LDI R17, 77
SUB R16,R17
NEG R16

NOP

LDI R16, 222
LDI R17, 77
SUB R16,R17
COM R16
INC R16

NOP
;-------------------------------------------------------------------------------
LDI R16, 1
LDI R17, 129
SUB R16,R17
NEG R16

NOP

LDI R16, 1
LDI R17, 129
SUB R16,R17
COM R16
INC R16

NOP
;-------------------------------------------------------------------------------
LDI R16, 1
LDI R17, 130
SUB R16,R17
NEG R16

NOP

LDI R16, 1
LDI R17, 130
SUB R16,R17
COM R16
INC R16

NOP
;-------------------------------------------------------------------------------
LDI R16, 1
LDI R17, 131
SUB R16,R17
NEG R16

NOP

LDI R16, 1
LDI R17, 131
SUB R16,R17
COM R16
INC R16

NOP
;-------------------------------------------------------------------------------
        LDI R16, 0
LOOP1:
        NEG R16
        NEG R16
        INC R16
        RJMP LOOP1



