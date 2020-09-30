/*
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#include <avr/io.h>
##include <avr/interrupt.h>
#include "avr_macro.S"
*/

#define rhtmp        R16

#define pinLED       PB1
#define iLEDInit     0

#define rCntOnOff    R30
#define rCntChange   r31

#define iCntOnOff    3
#define iCntPulse    1
#define iCntChange   (2*iCntPulse)-1
/******************************************************************************
# CLOCK  |¯¦_|¯   |¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_|¯¦_
#        |¯¦_._
# unit    1 2 3 
# pulse   1
# change ._^_._ ISR_OCIE
#        |¯¦_._
******************************************************************************/
funcBlinkLEDinit:
        _bit_wr   DDRB, pinLED,1,rhtmp
        _bit_wr   PORTB,pinLED,1,rhtmp
        RET
funcBlinkLEDreset:
        _bit_wr   PORTB,pinLED,1,rhtmp
        RET
funcBlinkLEDinitConfig:
        _mwrki    iCntOnOff, rCntOnOff
        _mwrki    iCntChange,rCntChange
        RET
funcBlinkLED_coding:
    _jp_funcBlinkLED_coding_dec_unit_cnt:
    DEC rCntOnOff
    _jp_funcBlinkLED_coding_chk_flip:
    _if <TST rCntChange>, nz, elif
    _jp_funcBlinkLED_coding_do_flip:
        _bit_not  PORTB,pinLED,rhtmp
        DEC       rCntChange
    _jp_funcBlinkLED_coding_chk_reset:
    _elif <TST rCntOnOff>, eq, Eif
    _jp_funcBlinkLED_coding_do_reset:
        #_bit_not  PORTB,pinLED,rhtmp
        RCALL     funcBlinkLEDreset
        _mwrk     iCntOnOff,rCntOnOff
        _mwrk     iCntChange,rCntChange
    _Eif
    RET
