.macro  _mcp_tb
#if defined (__AVR_ATmega328P__)
        .irp src r0,PORTB,TCNT1L,0x100,(RAMEND+1)
          .irp dst r1,DDRB,TCNT1H,0x101,(RAMEND+2)
            _mcp \src, \dst, r18
          .endr
        .endr
#elif defined (__AVR_ATtiny85__)
        .irp src r0,PORTB,TCNT0,0x60,(RAMEND+1)
          .irp dst r1,DDRB,TCNT1,0x61,(RAMEND+2)
            _mcp \src, \dst, r18
          .endr
        .endr
#endif
.endm
/*
#_mcp_tb
#_mcp R0,R0
_mcp R0,R16
_mcp r0,R31
_mcp r0,Xl
_mcp r0,PORTB
_mcp r0,SREG
_mcp r0,RAMSTART
*/
.macro _mld_tb
  #_mld r0,x,0
  _mld r0,x,0
  _mld r0,x,-
  _mld r0,x,+
  _mld r0,y,d,1
  _mld r0,z,d,63
.endm
#_mld_tb
.macro _mst_tb
  _mst x,r1,0
  _mst x,r2,-
  _mst x,r3,+
  _mst y,r4,d,1
  _mst z,r5,d,63
.endm
#_mst_tb
.macro _mcp_p_tb
  .irp _src r0,x,X,y,Y,z,Z,r18
    .irp _dst r0,r18,x,X,y,Y,z,Z
      _mcp_p \_src, \_dst
    .endr
  .endr
.endm
#_get_aaddr x
#_mcp_p_tb
#_mcp_p x,r0
#_mcp_p r0,x
#_mcp_p y,x
.macro _mcp_p_tb2
  .irp _src r0,PORTB,SREG,0x60,x
    .irp _dst r0,PORTB,SREG,0x60,x
      _mcp_p \_src, \_dst
    .endr
  .endr
.endm
#_mcp_p_tb2
.macro _mcp_p_tb3
_mcp_p x,r0
_mcp_p r31,y
_mcp_p z,r1,+
_mcp_p r2,x,-
_mcp_p z,0x60,+
_mcp_p PORTB,y,-
_mcp_p TCNT0,z,d,2
_mcp_p OCR0A,y,d,1
.endm
#_mcp_p_tb3
.macro _mwrk_tb
#if defined (__AVR_ATmega328P__)
        .irp src -128,0,127,255
          .irp dst r1,DDRB,TCNT1H,0x101,(RAMEND+2)
            _mwrk \src, \dst, r18
          .endr
        .endr
#elif defined (__AVR_ATtiny85__)
        .irp src -128,0,127,255
          .irp dst r1,DDRB,TCNT1,0x61,(RAMEND+2)
            _mcp \src, \dst, r18
          .endr
        .endr
#endif
.endm
#_mwrk_tb
.macro _mwrk_p_tb
_mwrk_p -129,x
_mwrk_p 0,a
_mwrk_p 0,x
_mwrk_p 0xFF,y
_mwrk_p 0xAA,z,-
_mwrk_p 0x55,x,+
_mwrk_p 0xAA,z,d,1
_mwrk_p 0x55,y,d,63
_mwrk_p 0x55,z,d,0x1F,r20
.endm
#_mwrk_p_tb
.macro _pmrd_gpr_tb
  _pmrd_gpr
  _pmrd_gpr r0
  _pmrd_gpr r0,+
.endm
#_pmrd_gpr_tb
.macro _pmcp_tb1
  _pmcp
  .irp  xaddr R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10,R11,R12,R13,R14,R15,\
              R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,R26,R27,R28,R29,R30,R31,\
              r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10,r11,r12,r13,r14,r15,\
              r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,\
              xl, xL, Xl, XL,\
              xh, xH, Xh, XH,\
              yl, yL, Yl, YL,\
              yh, yH, Yh, YH
        _pmcp \xaddr
  .endr
.endm
.macro _pmcp_tb2
  _pmcp
  .irp  xaddr 0x20,0x3f,PORTB,DDRB,\
              0x40,0x5f,\
              0x60,0x25f,0x260
        _pmcp \xaddr
  .endr
  .irp  xaddr 0x20,0x3f,PORTB,DDRB,\
              0x40,0x5f,\
              0x60,0x25f,0x260
        _pmcp \xaddr, +
  .endr
  .irp  xaddr 0x20,0x3f,PORTB,DDRB,\
              0x40,0x5f,\
              0x60,0x25f,0x260
        _pmcp \xaddr, +, r26
  .endr
.endm
#_pmcp_tb2



