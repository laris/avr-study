/* enhance 8-bit hardware timer to 16-bit with software implement
 * limitation:
 * 1. 8-bit only work with normal and ctc mode
 * 2. 16-bit only work with ctc mode
 */

/*
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"
*/
#define rhtmp1      R16
#define rhtmp2      R17
#define REG_TCCTRL  R18 // TC status reg

#define REG_GTCCR   R19 // overlap SFR
#define REG_TIMSK   R20 // overlap SFR
#define REG_TIFR    R21 // overlap SFR

#define REG_TCNT0   R22 // - TC0
#define REG_OCR0A   R23 // TC0
#define REG_OCR0B   R24 // TC0
#define REG_TCCR0A  R25 // TC0
#define REG_TCCR0B  R26 // TC0

#define REG_TCNT1   R22 // - TC1
#define REG_OCR1A   R23 // TC1
#define REG_OCR1B   R24 // TC1
#define REG_OCR1C   R25 // TC1
#define REG_PLLCSR  R26 // TC1
#define REG_TCCR1   R27 // TC1

#define TCNT0H      R28 // TC0
#define OCR0AH      R29 // TC0
#define TCNT1H      R30 // TC1
#define OCR1CH      R31 // TC1
;-------------------------------------------------------------------------------
; For clarity, what i'm currently doing right now is this:
; 16 bit count is 548 (HIGH_TOPCOUNT = 2, LOW_TOPCOUNT = 36)
; Initialization:
; CLEAR HIGH_COUNTER
; OCRnA = LOW_TOPCOUNT
; TIMER MODE = NORMAL
; TIMER COMPARE MATCH INTERRUPT ENABLED
; START TIMER
; Compare Match ISR:
;   DISABLE CTC MODE (IF ENABLED)
;   CHECK TIMER OVERFLOW_FLAG. 
;   IF OVF SET
;     INCREMENT HIGH_COUNTER
;     CLR OVF
;   ENDIF
;   IF HIGH_COUNTER = (HIGH_TOPCOUNT - 1)
;       SET TIMER MODE = CTC
;   ELSE IF HIGH_COUNTER >= (HIGH_TOPCOUNT) THEN
;     {
;       CLEAR HIGH_COUNTER
;       DO STUFF
;     }
;   ELSE SKIP ISR
;-------------------------------------------------------------------------------
/*****************************************************************************/
func_tnx5_tc0_init_reg:
    _mcp_mask REG_GTCCR,GTCCR,0b10000001,rhtmp1,rhtmp2
    _mcp_mask REG_TIMSK,TIMSK,0b00011010,rhtmp1,rhtmp2
    _mcp_mask REG_TIFR,TIFR,  0b00011010,rhtmp1,rhtmp2   // overlap done
    _mcp      REG_TCNT0,TCNT0
    _mcp      REG_OCR0A,OCR0A
    _mcp      REG_OCR0B,OCR0B
    _mcp_mask REG_TCCR0A,TCCR0A,0b11110011,rhtmp1,rhtmp2 // TCCR0A
    _mcp_mask REG_TCCR0B,TCCR0B,0b00001000,rhtmp1,rhtmp2 // TCCR0B CS no write
    _mcp_mask REG_TCCR0B,REG_TCCTRL,0b00000111,rhtmp1,rhtmp2 // backup CS0[2,1,0]
    RET
func_tnx5_tc0_start_reg:
    _mcp_mask REG_TCCTRL,TCCR0B,0b00000111,rhtmp1,rhtmp2
    RET
func_tnx5_tc0_stop_reg:
    _mem_andi REG_TCCR0B,0b11111000               // clr CS0[2,1,0]
    RET
func_tnx5_tc0_do_foc0a:
    _mem_ori  REG_TCCR0B,0b10000000               // write 1 FOC
    RET
func_tnx5_tc0_do_foc0b:
    _mem_ori  REG_TCCR0B,0b01000000               // write 1 FOC
    RET
/*****************************************************************************/
func_tnx5_tc1_init_reg:
    _mcp_mask REG_GTCCR,GTCCR,0b01110010,rhtmp1,rhtmp2
    _mcp_mask REG_TIMSK,TIMSK,0b01100100,rhtmp1,rhtmp2
    _mcp_mask REG_TIFR,TIFR,  0b01100100,rhtmp1,rhtmp2   // overlap done
    _mcp      REG_TCNT1,TCNT1
    _mcp      REG_OCR1A,OCR1A
    _mcp      REG_OCR1B,OCR1B
    _mcp      REG_OCR1C,OCR1C
    _mcp      REG_PLLCSR,PLLCSR
    _mcp_mask REG_TCCR1,TCCR1,0b11110000,rhtmp1,rhtmp2   // TCCR1 CS no write
    SWAP      REG_TCCR1                          // save CS1[3,0] to upper nibble
    _mcp_mask REG_TCCR1,REG_TCCTRL,0b11110000,rhtmp1,rhtmp2 // backup CS0[2,1,0]
    //SWAP      REG_TCCR1                          // recover
    RET
func_tnx5_tc1_start_reg:
    PUSH      REG_TCCTRL
    SWAP      REG_TCCTRL
    _mcp_mask REG_TCCTRL,TCCR1,0b00001111,rhtmp1,rhtmp2
    POP       REG_TCCTRL
    RET
func_tnx5_tc1_stop_reg:
    _mem_andi REG_TCCR1,0b11110000               // clr CS1[3,2,1,0]
    RET
func_tnx5_tc1_do_foc1a:
    _mem_ori  REG_GTCCR,0b00000100               // write 1 FOC
    RET
func_tnx5_tc1_do_foc1b:
    _mem_ori  REG_GTCCR,0b00001000               // write 1 FOC
    RET
/*****************************************************************************/
_isr_OC0A_16bTC:
  _jp_isr_OC0A_16bTC_chk_HTOP_Z:
    TST OCR0AH
    _if_cond nz, _jp_isr_OC0A_16bTC_OC0A_DO // if not zero, do below, fail jp DO
  _jp_isr_OC0A_16bTC_off_CTC:
    _mem_andi TCCR0A,0b11110000,rhtmp1  // off CTC
  _jp_isr_OC0A_16bTC_chk_ovf:
    _if_bit TIFR,TOV0,set,_jp_isr_OC0A_16bTC_chk_ovf_clr
  _jp_isr_OC0A_16bTC_chk_ovf_set: //if 256, clr flg, high cnt++, else skip
      _bit_wr TIFR,TOV0,1
      INC     TCNT0H
  _jp_isr_OC0A_16bTC_chk_ovf_clr:
  _jp_isr_OC0A_16bTC_chk_final_round:
      _mcp    TCNT0H,rhtmp1
      INC     rhtmp1
      _if <CP rhtmp1,OCR0AH>, eq, elif
  _jp_isr_OC0A_16bTC_on_CTC:
          _mem_ori TCCR0A,0b00000010,rhtmp1  // on CTC
      _Ethen
      _elif <CP TCNT0H,OCR0AH>, sh, Eif
  _jp_isr_OC0A_16bTC_reset:
          CLR TCNT0H
  _jp_isr_OC0A_16bTC_OC0A_DO:
          //RCALL _call_loop_cnt_nx_4ms9
  _jp_isr_OC0A_16bTC_OC0A_DO_ICALL:
          PUSH  ZL
          PUSH  ZH
          CLR   ZH
          LDI   ZL, pm_lo8(_SW_INT_VECTOR_JP1)
          #LDI   ZL, _VECTORS_SIZE/2
          ICALL 
          POP   ZH
          POP   ZL
  _jp_isr_OC0A_16bTC_OC0A_DO_ICALL_END:
          _if <TST OCR0AH>, z
  _jp_isr_OC0A_16bTC_on_CTC_8bit:
              _mem_ori TCCR0A,0b00000010,rhtmp1  // on CTC
          _Eif
      _Eif
  _jp_isr_OC0A_16bTC_exit:
  RETI
/*****************************************************************************/
_isr_OC1C_16bTC:
  _jp_isr_OC1C_16bTC_chk_HTOP_Z:
    TST OCR1CH
    _if_cond nz, _jp_isr_OC1C_16bTC_OC1C_DO // if not zero, do below, fail jp DO
  _jp_isr_OC1C_16bTC_off_CTC:
    _mem_andi TCCR1,0b01111111,rhtmp1  // off CTC1
  _jp_isr_OC1C_16bTC_chk_ovf:
    _if_bit TIFR,TOV1,set,_jp_isr_OC1C_16bTC_chk_ovf_clr
  _jp_isr_OC1C_16bTC_chk_ovf_set: //if 256, clr flg, high cnt++, else skip
      _bit_wr TIFR,TOV1,1
      INC     TCNT1H
  _jp_isr_OC1C_16bTC_chk_ovf_clr:
  _jp_isr_OC1C_16bTC_chk_final_round:
      _mcp    TCNT1H,rhtmp1
      INC     rhtmp1
      _if <CP rhtmp1,OCR1CH>, eq, elif
  _jp_isr_OC1C_16bTC_on_CTC:
          _mem_ori TCCR1,0b10000000,rhtmp1  // on CTC
      _Ethen
      _elif <CP TCNT1H,OCR1CH>, sh, Eif
  _jp_isr_OC1C_16bTC_reset:
            CLR TCNT1H
  _jp_isr_OC1C_16bTC_OC1C_DO:
          LDI   ZL, pm_lo8(_SW_INT_VECTOR_JP1)
          //RCALL
            _if <TST OCR1CH>, z
  _jp_isr_OC1C_16bTC_on_CTC_8bit:
              //_mem_ori TCCR1,0b10000000,rhtmp1  // on CTC
              _mem_ori TCCR1,<1!<!<CTC1>,rhtmp1  // on CTC
            _Eif
      _Eif
  _jp_isr_OC1C_16bTC_exit:
  RETI
/*****************************************************************************/