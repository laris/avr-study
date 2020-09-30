.include "m328p_def.inc"
.include "macro_utils.inc"
.include "macro_dtrans.inc"
/******************************************************************************/
# _mcp
#_mcp 31,1
#_mcp 0,r1
#_mcp 0,0x1F
#_mcp 1,DDRB
#.irp dst r1, R15, 16, 0x1F, DDRB, SREG, TCNT1H, 0x100 ,RAMEND
#  _mcp 0, \dst
#.endr
#.irp dst r1, 0x1F, DDRB, SREG, TCNT1H, 0x100, RAMEND
#  _mcp DDRB, \dst
#.endr
#.irp dst r1, 0x1F, DDRB, SREG, TCNT1H, 0x100, RAMEND
#  _mcp SREG, \dst
#.endr
#.irp dst r1, 0x1F, DDRB, SREG, TCNT1H, 0x100, RAMEND
#  _mcp TCNT1H, \dst
#.endr
#.irp dst r1, 0x1F, DDRB, SREG, TCNT1H, 0x100, RAMEND
#  _mcp 0x100, \dst
#.endr
/******************************************************************************/
# _mcpp _src:req, _sidp, _sdp, _rtmp=r24, _dst:req, _didp, _ddp
#_mcpp 0      ,,,r25,x
#_mcpp r1     ,,,r25,Y
#_mcpp r26    ,,,r25,Z

#_mcpp ZH     ,,,r25,x,-
#_mcpp xl     ,,,r25,Y,+
#_mcpp Yh     ,,,r25,Z,d,1

#_mcpp PORTB  ,,,r25,x
#_mcpp SREG   ,,,r25,Y
#_mcpp RAMEND ,,,r25,Z

#_mcpp PORTB  ,,,r25,x,-
#_mcpp SREG   ,,,r25,Y,+
#_mcpp RAMEND ,,,r25,Z,d,2

#_mcpp x, , ,    r24, 0x2     , , 
#_mcpp y, , ,    r23, r15     , ,
#_mcpp Z, , ,    r22, r20     , ,

#_mcpp x,-, ,    r24, 0x2     , , 
#_mcpp y,+, ,    r23, r15     , ,
#_mcpp Z,d,9,    r22, r20     , ,

#_mcpp x,-, ,    r24, PORTB   , , 
#_mcpp y,+, ,    r23, SREG    , ,
#_mcpp Z,d,9,    r22, RAMEND  , ,

#_mcpp x, , ,    r24, y, ,
#_mcpp y, , ,    r23, Z,-,
#_mcpp Z, , ,    r22, x,+,
#_mcpp x, , ,    r22, z,d,63

#_mcpp x, , ,       , y,-,
#_mcpp y,-, ,       , Z,+,
#_mcpp Z,+, ,       , y,d,12
#_mcpp y,d,2,       , x, ,
#_mcpp y,d,2,       , z,d,2
/******************************************************************************/
#_mwri 0xff, 0
#_mwri 0xff, r0
#_mwri 0xff, r16
#_mwri 0xff, PORTB
#_mwri 0xff, SREG
#_mwri 0xff, SRAM_START
#_mwri hi8(0x1234), SREG
#_mwri 12+3, SRAM_START
#.equ __ABEFD, 0xFF
#_mwri __ABEFD, SRAM_START
/******************************************************************************/
# _bcp
#_bcp yes    , , no
#_bcp no     , , no
#_bcp T      , , no
#_bcp t      , , no
#_bcp SREG_T , , no
#_bcp 0      , , no
#_bcp 0x10   , , no
#_bcp PORTB  , , no
#_bcp RAMEND , , no

#_bcp yes    , , no
#_bcp no     , , no
#_bcp T      , , no
#_bcp t      , , no
#_bcp SREG_T , , no
#_bcp 0      , , no
#_bcp 0x10   , , no
#_bcp PORTB  , , no
#_bcp RAMEND , , no

#_bcp zero,,T
#_bcp clr,,SREG_T
#_bcp false,,t
#
#_bcp one,,T
#_bcp Set,,SREG_T
#_bcp TRUE,,t

# K -> byte-bit
#_bcp 1,,  r0        , 0
#_bcp one  ,,  r0        , 0
#_bcp Set  ,,  16        , 1
#_bcp TRUE ,,  PORTB     , PB2
#_bcp zero ,,  SREG      , SREG_T
#_bcp 1    ,,  SREG      , SREG_Z
#_bcp on   ,,  TCNT1H    , 7
#_bcp on   ,,  SRAM_START, 6
#_bcp Clr  ,,  RAMEND    , 5

# T-> byte-bit
#_bcp T     ,,  r0        , 0
#_bcp T     ,,  16        , 1
#_bcp T     ,,  PORTB     , PB2
#_bcp T     ,,  SREG      , SREG_T
#_bcp T     ,,  SREG      , SREG_Z
#_bcp T     ,,  TCNT1H    , 7
#_bcp T     ,,  SRAM_START, 6
#_bcp T     ,,  RAMEND    , 5

# byte-bit -> T
#_bcp r0        , 0        , t
#_bcp 16        , 1        , t
#_bcp PORTB     , PB2      , t
#_bcp SREG      , SREG_T   , t
#_bcp SREG      , SREG_Z   , t
#_bcp TCNT1H    , 7        , t
#_bcp SRAM_START, 6        , t
#_bcp RAMEND    , 5        , t

# byte-bit -> byte-bit
#_bcp r0        , 0        , 16        , 1     
#_bcp 16        , 1        , PORTB     , PB2   
#_bcp PORTB     , PB2      , SREG      , SREG_T
#_bcp SREG      , SREG_T   , SREG      , SREG_Z
#_bcp SREG      , SREG_Z   , TCNT1H    , 7     
#_bcp TCNT1H    , 7        , SRAM_START, 6     
#_bcp SRAM_START, 6        , RAMEND    , 5     
#_bcp RAMEND    , 5        , 0					, 0
/******************************************************************************/
#_pmrd_gpr
#_pmrd_gpr r16
#_pmrd_gpr r16,+
#_pmrd
#_pmrd r17
#_pmrd r18,+
#_pmrd PORTB
#_pmrd SREG
#_pmrd RAMEND
/******************************************************************************/
