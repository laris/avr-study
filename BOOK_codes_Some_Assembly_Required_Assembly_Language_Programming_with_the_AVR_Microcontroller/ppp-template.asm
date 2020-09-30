;*******************************************************************************
#define TN85
#define M328P
#if defined(M328P)
  .INCLUDE "M328PDEF.INC"
#elif defined(TN85)
  .INCLUDE "TN85DEF.INC"
#endif
;-------------------------------------------------------------------------------
.INCLUDE "AVR_MACRO.INC"
.INCLUDE "AVR_MACRO_IOREG.INC"
.INCLUDE "AVR_MACRO_MISC1.INC"
;*******************************************************************************
