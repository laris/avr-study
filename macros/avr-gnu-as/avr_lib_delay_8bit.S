/*
##define __AVR_ATtiny85__
#define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"
*/
/*****************************************************************************/
// 19c overhead +[4x4=16,               4x58]+[0,1,2,3]
// 19c          +     16+[<0>,1,2,3], 19c +232+[0,1,2,<3>]
// =[35,             D255/0xFF]
/*
.macro  _init_delay_8bit_cycle _reg_cnt:req, _reg_rmd:req
#define delayCNT_reg        \_reg_cnt //R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayNx4cycRmd_reg  \_reg_rmd  //R25 // Remainder
//#define delayCNT_ram_addr_sram 0x100
#define funcDelayNCycle8_OVERHEAD_reg  19
.endm
*/
funcDelayNCycle8_reg:  // 36-byte 13-word
/*****************************************************************************/
  jp_call_method_gpr_reg:
        //LDI delayCNT_reg,255 //2c
        //RCALL funcDelayNCycle8_reg           // RCALL 3/4 overhead
  jp_call_method_ram_reg:
        //_ldi16hl 52,delayCNTH_reg,delayCNT_reg //2c
        //_mcp delayCNT_reg, delayCNT_ram_addr_reg //2c
  jp_call_method_sp_reg:
        //_pushki16hl 52                            //6c
        //RCALL funcDelayNCycle8_reg           // RCALL 3/4 overhead
        //_pop16lh r16,r16                          //4c
  jp_get_arg_cnt_from_sp_reg: //reg
        //_mcp  SPH,ZH                        // 1c
        //_mcp  SPL,ZL                        // 1c
        //_mcp_p Z,delayCNT_reg,d,3              // 2c
  jp_get_arg_cnt_from_ram_reg:
        //_mcp delayCNT_ram_addr_reg,delayCNT_reg   // 2c
  jp_get_arg_cnt_from_gpr_reg:
  jp_calc_overhead_start_reg:
      //SBIW delayCNT_reg,funcDelayNCycle8_OVERHEAD_reg // 2c
        SUBI delayCNT_reg,funcDelayNCycle8_OVERHEAD_reg // 1c
  jp_calc_overhead_done_reg:
  jp_divide_cnt_by_4_reg:
  jp_divide_cnt_by_2_1_reg:
        LSR   delayCNT_reg            // 1c /2 H
  jp_save_remainder_bit0_reg:
        CLR   delayNx4cycRmd_reg       // 1c save remainder, clr 
        ROR   delayNx4cycRmd_reg       // 1c shift c/lsb to msb
  jp_divide_cnt_by_2_2_reg:
        LSR   delayCNT_reg            // 1c /2 L = counter/4
  jp_save_remainder_bit1_reg:
        //ROR   delayNx4cycRmd_reg       // 1c shift c/lsb-b1 to msb
  jp_recover_remainder_reg:
        //ROL   delayNx4cycRmd_reg       // 1c recover remainder b1->c
        ROL   delayNx4cycRmd_reg       // 1c recover remainder c->b1, b0->c
        ROL   delayNx4cycRmd_reg       // 1c recover remainder c->b0
  jp_divide_done_reg: // now quotient = CNTL:CNTH, remainder = rRmd
        //CLZ                             // 1c, avoid remainder=0
  jp_funcDelayNCycle8_Loop_reg:
        DEC   delayCNT_reg             // 1c
        NOP                             // 1c
        BRNE  .-6                       // f1c/t2c
  jp_funcDelayNCycle8_add_1_2_3_cyc_reg:
                    // fix 6c overhead for branch, [6,7,8]=6+[0,1,2] overhead
  _if <LSR delayNx4cycRmd_reg>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
  _Eif              // true 1+1+2=4c=3+1c, false 1+2 = 3c
  _if <LSR delayNx4cycRmd_reg>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
      NOP           // 1c
  _Eif              // true 1+1+2+1=5c=3+2c, false 1+2=3c
      RET                           // 4c/5c overhead
/*****************************************************************************/
