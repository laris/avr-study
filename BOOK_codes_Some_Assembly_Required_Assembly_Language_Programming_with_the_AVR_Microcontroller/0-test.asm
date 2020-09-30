;.EQU  ODD_DIV_RESULT = 3/2
;.DB   ODD_DIV_RESULT

;.EQU  Result = 0xbb-0xaa 
;      LDI   R16,Result
.CSEG
.ORG  $0
;      CLC
;      LDI R16,1
    CPI   R20, 'e'
    BREQ  FOUND
    FOUND: 
          RJMP PC