/*
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"
_jp_init_sp:
        _init_sp
_main:  
        //_ldi16hl 17,delayCNTH_reg,delayCNTL_reg //2c
        _ldi16hl 38,r25,r24 //2c
        RCALL funcDelayNCycle16_reg           // RCALL 3/4 overhead
        NOP
        NOP
        NOP
        _ldi16hl 0xFFFF ,r25,r24 //2c
        RCALL funcDelayNCycle16_reg           // RCALL 3/4 overhead
        NOP
_end:
        rjmp _end
*/
/*****************************************************************************/
// 22c overhead +[4x4=16,               4xD16378/0x3FFA]+[0,1,2,3]
// 22c           +     16+[<0>,1,2,3],  22c +D65512/0xFFE8+[0,<1>,2,3]
// =[38,             D65535/0xFFFF]
#define delayCNTL_reg       R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayCNTH_reg       R25
#define delayNx4cycRmd_reg  R26 // Remainder
//#define delayCNTL_ram_addr_sram 0x100
//#define delayCNTH_ram_addr_sram 0x101
#define funcDelayNCycle16_OVERHEAD_reg  22
funcDelayNCycle16_reg:  // 48-bytes 24-words
/*****************************************************************************/
  jp_call_method_gpr_reg:
        //_ldi16hl 52,delayCNTH_reg,delayCNTL_reg //2c
        //RCALL funcDelayNCycle16_reg           // RCALL 3/4 overhead
  jp_call_method_ram_reg:
        //_ldi16hl 52,delayCNTH_reg,delayCNTL_reg //2c
        //_mcp delayCNTH_reg, delayCNTH_ram_addr_reg //2c
        //_mcp delayCNTL_reg, delayCNTL_ram_addr_reg //2c
  jp_call_method_sp_reg:
        //_pushki16hl 52                            //6c
        //RCALL funcDelayNCycle16_reg           // RCALL 3/4 overhead
        //_pop16lh r16,r16                          //4c
  jp_get_arg_cnt_from_sp_reg: //reg
        //_mcp  SPH,ZH                        // 1c
        //_mcp  SPL,ZL                        // 1c
        //_mcp_p Z,delayCNTL_reg,d,3              // 2c
        //_mcp_p Z,delayCNTH_reg,d,4              // 2c
  jp_get_arg_cnt_from_ram_reg:
        //_mcp delayCNTL_ram_addr_reg,delayCNTL_reg   // 2c
        //_mcp delayCNTH_ram_addr_reg,delayCNTH_reg   // 2c
  jp_get_arg_cnt_from_gpr_reg:
  jp_calc_overhead_start_reg:
        SBIW delayCNTL_reg,funcDelayNCycle16_OVERHEAD_reg // 2c
  jp_calc_overhead_done_reg:
  jp_divide_cnt_by_4_reg:
  jp_divide_cnt_by_2_1_reg:
        LSR   delayCNTH_reg            // 1c /2 H
        ROR   delayCNTL_reg            // 1c /2 L
  jp_save_remainder_bit0_reg:
        CLR   delayNx4cycRmd_reg       // 1c save remainder, clr 
        ROR   delayNx4cycRmd_reg       // 1c shift c/lsb to msb
  jp_divide_cnt_by_2_2_reg:
        LSR   delayCNTH_reg            // 1c /2 H
        ROR   delayCNTL_reg            // 1c /2 L = counter/4, now CNTH = 0
  jp_save_remainder_bit1_reg:
        //ROR   delayNx4cycRmd_reg       // 1c shift c/lsb-b1 to msb
  jp_recover_remainder_reg:
        //ROL   delayNx4cycRmd_reg       // 1c recover remainder b1->c
        ROL   delayNx4cycRmd_reg       // 1c recover remainder c->b1, b0->c
        ROL   delayNx4cycRmd_reg       // 1c recover remainder c->b0
  jp_divide_done_reg: // now quotient = CNTL:CNTH, remainder = rRmd
        //CLZ                             // 1c, avoid remainder=0
  jp_funcDelayNCycle16_Loop_reg:
        SBIW  delayCNTL_reg,1           // 2c
        BRNE  .-4                   // f1c/t2c
  jp_funcDelayNCycle16_add_1_2_3_cyc_reg:
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
