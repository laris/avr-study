;.INCLUDE "avr_macro.inc"
#define _GPR0   R  ; string
.MACRO  _byte_com
        .IF   @0 == _GPR0
          .message "good"
        .ENDIF
.ENDM

.CSEG
.ORG  $0

;        _byte_com
;        _byte_com PORTB,R16