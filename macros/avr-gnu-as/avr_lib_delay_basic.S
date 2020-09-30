##define __AVR_ATtiny85__
#define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"

_jp_init_sp:
        _init_sp
_main:  
#        rjmp _tst_funcDelayNx3Cycle8
#        rjmp _tst_funcDelayNx4Cycle16
#        rjmp _tst_funcDelayNx4Cycle16_nx4
        rjmp _tst_funcDelayNCycle24_stack
_tst_funcDelayNx3Cycle8:
        LDI   R16,254
        push  R16
        rcall funcDelayNx3Cycle8
        pop   r16
        NOP
_tst_funcDelayNx4Cycle16:
        //LDI   R16,0 //min
        LDI   R16,hi8(65532+3)   // max
        PUSH  R16   // H

        //LDI   R16,4 //min
        LDI   R16,lo8(65532+3)   // max
        PUSH  R16   // L
        RCALL funcDelayN4xCycle16
        POP   R16   // 2c
        POP   R16 // 2c
_tst_funcDelayNx4Cycle16_nx4:
        //LDI   R16,0 //min
        LDI   R16,hi8(65532+3)   // max
        PUSH  R16   // H

        //LDI   R16,4 //min
        LDI   R16,lo8(65532+3)   // max
        PUSH  R16   // L
        RCALL funcDelayN4xCycle16_nx4
        POP   R16   // 2c
        POP   R16 // 2c
_tst_funcDelayNCycle24_stack:
        //PUSH delayCNTH
        //PUSH delayCNTM
        //PUSH delayCNTL
        _pushki24hl 52
        RCALL funcDelayNCycle24_stack
        _pop24lh r16,r16,r16
        //POP delayCNTL
        //POP delayCNTM
        //POP delayCNTH
        _pushki24hl 53
        RCALL funcDelayNCycle24_stack
        _pop24lh r16,r16,r16

        _pushki24hl 0x40023
        RCALL funcDelayNCycle24_stack
        _pop24lh r16,r16,r16
_end:
        rjmp  _end
/******************************************************************************/
/* 1.avr-libc
/*     avr-libc-user-manual-2.0.0/delay__basic_8h_source.html
/* 2.Precise delay in asm
/*     https://www.avrfreaks.net/forum/precise-delay-asm
/* 3.delay-assembly-utilities
/*     http://avr-mcu.dxp.pl Radoslaw Kwiecien, 2008
/*     http://en.radzio.dxp.pl/avr-mcu/delay-assembly-utilities.html
/* 4.delay_ms.inc.asm
/*     https://gist.github.com/gamaral/3773527
/* 5.PRECISION DELAY ROUTINES(FUNCTION FORMAT MACROS)                       
/*     AUTHOR: OMID KOMPANI, TEHRAN-IRAN 30/10/05 - 1384/8/8
/*     https://www.avrfreaks.net/sites/default/files/forum_attachments/Macro_Delays.asm
/*     https://www.avrfreaks.net/forum/uart-half-duplex-attiny861
/* 6.JavaScript version, delay loop calculator
/*     Developed originally by Bret Mulvey.
/*     http://www.bretmulvey.com/avrdelay.html
/*     Register enhancement by T. Morland. (ACES '18)
/******************************************************************************/
/* ref: [2]
;************************************************************************
;*                      Delay Macro 8 ... 65543 Cycle                   *
;*              Author: Peter Dannegger                                 *
;************************************************************************
.listmac
;delay 8 ... 65543 cycle
.macro  mdelay
        ldi    r24,  low(@0 - 8)
        ldi    r25, high(@0 - 8)
        sbiw   r24, 4
        brcc   pc - 1
        cpi    r24, 0xFD
        brcs   pc + 4
        breq   pc + 3
        cpi    r24, 0xFF
        breq   pc + 1
.endmacro
        mdelay   8
        mdelay   9
        mdelay  10
        mdelay  11
        mdelay  12
        mdelay  65543
        rjmp    pc
/******************************************************************************/
/******************************************************************************
.macro delay_cycles_2 cycles:req
    .set _cy_, \cycles
    ; up to 10 cycles can be done by 'nop' and 'rjmp .' sequence
    ; in less-or-equal words than loop
    .if _cy_ >= 11
        .set _cy_, _cy_ - 1
        ldi    r24, lo8( _cy_ / 4 )
        ldi    r25, hi8( _cy_ / 4 )
        sbiw    r24, 1
        brne .-4
        .set _cy_, _cy_ % 4
    .endif
    ; less than 11 cycles or remainder from long delay
    .if (_cy_ & 0x01)
        nop
    .endif
    .rept  (_cy_ / 2)
        rjmp .
    .endr
.endm
******************************************************************************/
#define delayNx3cycCNT R16 // GPRH because SBR/ANDI
#define delayNx3cycRmd R1  //Remainder
funcDelayNx3Cycle8: // 30 x bytes
// ref 1.avr-libc and 6.JS ver
// need push cyc num to delayNx3cycCNT before rcall, delayNx3cycRmd[0,31] as temp reg
// total 18c overhead, 18+[4x3=12,252x3=756]+[0,1,2]=[30,776]
// load N = [(TargetCycNum-14)/3 CBR 0x03] ORI [(TargetCycNum-14)%3]
                                                 // RCALL 3/4 overhead
        _mcp  SPH,ZH                             // 1c
        _mcp  SPL,ZL                             // 1c
        _mcp_p Z,delayNx3cycCNT,d,3              // 2c
        _mcp  delayNx3cycCNT,delayNx3cycRmd      // 1c copy for add 1/2c
        CBR   delayNx3cycCNT,0x03                // 1c, init 2+1+1=4c+2 overhead
  jp_funcDelayNx3Cycle8_Loop:           // delayNx3cycCNT x 3 cycles -1(fail)=3x[1,252]-1
        DEC   delayNx3cycCNT                   // 1c
        BRNE  jp_funcDelayNx3Cycle8_Loop // f1c/t2c
  jp_funcDelayNx3Cycle8_add_1_2_cyc:
// fix 6c overhead for branch, [6,7,8]=6+[0,1,2] overhead
  _if <LSR delayNx3cycRmd>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
  _Eif              // true 1+1+2=4c=3+1c, false 1+2 = 3c
  _if <LSR delayNx3cycRmd>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
      NOP           // 1c
  _Eif              // true 1+1+2+1=5c=3+2c, false 1+2=3c
      RET           // 4c/5c overhead
/*****************************************************************************/
#define delayNx4CycCNTL R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayNx4CycCNTH R25
#define delayNx4cycRmd  R0 // Remainder
funcDelayN4xCycle16: // 32 x bytes
// ref 1.avr-libc and 2.Precision delay
// need push n-cycle/4 and <<2 into delayNx4CycCNT before rcall
// 20c overhead, 20+[4x4=16,D65532/0xFFFCx4=D262,128/0x3FFF0]+[0,1,2,3]
// =[36,D262,151/0x40007]
                                    // RCALL 3/4 overhead
      //PUSH  delayNx4CycCNTH             // 2c
      //PUSH  delayNx4CycCNTL             // 2c
      //RCALL funcDelayN4xCycle16
      //POP   delayNx4CycCNTL             // 2c
      //POP   delayNx4CycCNTH             // 2c
        _mcp  SPH,ZH                             // 1c
        _mcp  SPL,ZL                             // 1c
        _mcp_p Z,delayNx4CycCNTL,d,3              // 2c
        _mcp_p Z,delayNx4CycCNTH,d,4              // 2c
        _mcp  delayNx4CycCNTL,delayNx4cycRmd       // 1c
        CBR   delayNx4CycCNTL,0x03        // 1c, clr 0,1-bit, 2+2+1+1=6c overhead
  jp_funcDelayN4xCycle16_Loop:
        SBIW  delayNx4CycCNTL,1           // 2c
        BRNE  .-4                   // f1c/t2c
  jp_funcDelayN4xCycle16_add_1_2_3_cyc:
// fix 6c overhead for branch, [6,7,8]=6+[0,1,2] overhead
  _if <LSR delayNx4cycRmd>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
  _Eif              // true 1+1+2=4c=3+1c, false 1+2 = 3c
  _if <LSR delayNx4cycRmd>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
      NOP           // 1c
  _Eif              // true 1+1+2+1=5c=3+2c, false 1+2=3c
      RET           // 4c/5c overhead
/*****************************************************************************/
funcDelayN4xCycle16_nx4: // 24 x bytes, 13+[8,65543] x cycles = 21 + N[0,65535]
                                    // RCALL 3/4 overhead
      //PUSH  delayNx4CycCNTH             // 2c
      //PUSH  delayNx4CycCNTL             // 2c
      //RCALL funcDelayN4xCycle16_nx4
      //POP   delayNx4CycCNTL             // 2c
      //POP   delayNx4CycCNTH             // 2c
        _mcp  SPH,ZH                             // 1c
        _mcp  SPL,ZL                             // 1c
        _mcp_p Z,delayNx4CycCNTL,d,3              // 2c
        _mcp_p Z,delayNx4CycCNTH,d,4              // 2c
  jp_funcDelayN4xCycle16_nx4_Loop:
        SBIW  delayNx4CycCNTL,1         // 2c
        BRCC  .-4                 // 1/2
        CPI   delayNx4CycCNTL, 0xFD     // 1
        BRCS  .+6                 // 1/2
        BREQ  .+4                 // 1/2
        CPI   delayNx4CycCNTL, 0xFF     // 1
        BREQ  .                   // 2/1
  jp_funcDelayN4xCycle16_nx4_EXIT:
        RET 
/*----------------------------------------------------------------------------
N calc_cyc  cyc
0 34-12=22  =13+8+ 1 = 1+0x4 = 22 + Nx4
1 38-12=26  =13+8+ 5 = 1+1x4
2 42-12=30  =13+8+ 9 = 1+2x4
3 46-12=34  =13+8+ 13= 1+3x4
4 50-12=38  =13+8+ 17= 1+4x4
0x4xF 262174-12 = 262,162 = 22+ 65535x4
/*****************************************************************************/
// funcDelayN4xCycle16, cycles=[36,D262,151/0x40007]
funcDelayNCyc2N4xCyc:
#define delayCNTL R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayCNTM R25
#define delayCNTH R26
#define delayNx4cycRmd  R0 // Remainder
/*****************************************************************************/
        //PUSH delayCNTH
        //PUSH delayCNTM
        //PUSH delayCNTL
        //RCALL funcDelayNCyc2N4xCyc
        //POP delayCNTL
        //POP delayCNTM
        //POP delayCNTH
        _mcp  SPH,ZH                             // 1c
        _mcp  SPL,ZL                             // 1c
        _mcp_p Z,delayCNTL,d,3              // 2c
        _mcp_p Z,delayCNTM,d,4              // 2c
        _mcp_p Z,delayCNTH,d,5              // 2c
        //_mcp  delayCNTL,delayNx4cycRmd       // 1c
        //SBR   delayCNTL,0x03        // 1c, clr 0,1-bit, 2+2+1+1=6c overhead
        SBIW  delayCNTL,20            // 2c
        
        LSR   delayCNTH            // /2 H
        ROR   delayCNTM            // /2 M
        ROR   delayCNTL            // /2 L

        CLR   delayNx4cycRmd       // save remainder, clr 
        ROR   delayNx4cycRmd       // shift c/lsb to msb

        LSR   delayCNTH            // /2 H
        ROR   delayCNTM            // /2 M
        ROR   delayCNTL            // /2 L = counter/4, now CNTH = 0

        ROR   delayNx4cycRmd       // shift c/lsb-b1 to msb

        ROL   delayNx4cycRmd       // recover remainder b1->c
        ROL   delayNx4cycRmd       // recover remainder c->b1, b0->c
        ROL   delayNx4cycRmd       // recover remainder c->b0

        ADD   delayCNTL,delayNx4cycRmd // now get CNTL/M
        // pop ret address, push remainder, cntH/L, push ret addr
        RET
/*****************************************************************************/
funcDelayNCycle24_stack: // 36c overhead+[4x4=16,4xD65535/0xFFFF=D262140/0x3FFFC]+[0,1,2,3]
// 36+16+[0,1,2,3] - 36+D262140+[0,1,2,3]
// =[52,             D262179/0x40023 ]
#define delayCNTL_stack R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayCNTM_stack R25
#define delayCNTH_stack R26
#define delayNx4cycRmd_stack  R27 // Remainder
#define funcDelayNCycle24_OVERHEAD_stack 36
/*****************************************************************************/
  jp_call_method_gpr_stack:
        //_ldi24hl 52,delayCNTH_stack,delayCNTM_stack,delayCNTL_stack //3c
        //RCALL funcDelayNCycle24_stack           // RCALL 3/4 overhead
  jp_call_method_ram_stack:
        //_ldi24hl 52,delayCNTH_stack,delayCNTM_stack,delayCNTL_stack //3c
        //_mcp delayCNTH_stack, delayCNTH_ram_addr_stack
        //_mcp delayCNTM_stack, delayCNTM_ram_addr_stack 
        //_mcp delayCNTL_stack, delayCNTL_ram_addr_stack
  jp_call_method_sp_stack:
        //_pushki24hl 52
        //RCALL funcDelayNCycle24_stack           // RCALL 3/4 overhead
        //_pop24lh r16,r16,r16
  jp_get_arg_cnt_from_sp_stack: //reg, save -8c
        _mcp  SPH,ZH                        // 1c
        _mcp  SPL,ZL                        // 1c
        _mcp_p Z,delayCNTL_stack,d,3              // 2c
        _mcp_p Z,delayCNTM_stack,d,4              // 2c
        _mcp_p Z,delayCNTH_stack,d,5              // 2c
  jp_get_arg_cnt_from_ram_stack:
        //_mcp delayCNTL_ram_addr_stack,delayCNTL_stack   // 2c
        //_mcp delayCNTM_ram_addr_stack,delayCNTM_stack   // 2c
        //_mcp delayCNTH_ram_addr_stack,delayCNTH_stack   // 2c
  jp_get_arg_cnt_from_gpr_stack:
  jp_calc_overhead_start_stack:
        _if <SBIW delayCNTL_stack,36>,cs  // true=1c, false=2c, overhead=
        //BRCS  .+2
        DEC   delayCNTH_stack
        _Eif                        // true=2+1+1=4c, false=2+2=4c
  jp_calc_overhead_done_stack:
  jp_divide_cnt_by_4_stack:
  jp_divide_cnt_by_2_1_stack:
        LSR   delayCNTH_stack            // 1c /2 H
        ROR   delayCNTM_stack            // 1c /2 M
        ROR   delayCNTL_stack            // 1c /2 L
  jp_save_remainder_bit0_stack:
        CLR   delayNx4cycRmd_stack       // 1c save remainder, clr 
        ROR   delayNx4cycRmd_stack       // 1c shift c/lsb to msb
  jp_divide_cnt_by_2_2_stack:
        LSR   delayCNTH_stack            // 1c /2 H
        ROR   delayCNTM_stack            // 1c /2 M
        ROR   delayCNTL_stack            // 1c /2 L = counter/4, now CNTH = 0
  jp_save_remainder_bit1_stack:
        ROR   delayNx4cycRmd_stack       // 1c shift c/lsb-b1 to msb
  jp_recover_remainder_stack:
        ROL   delayNx4cycRmd_stack       // 1c recover remainder b1->c
        ROL   delayNx4cycRmd_stack       // 1c recover remainder c->b1, b0->c
        ROL   delayNx4cycRmd_stack       // 1c recover remainder c->b0
  jp_divide_done_stack: // now quotient = CNTL:CNTM, remainder = rRmd
        //ADD   delayCNTL_stack,delayNx4cycRmd_stack // now get CNTL/M
        // pop ret address, push remainder, cntH/L, push ret addr
        //RET
/*****************************************************************************/
/*****************************************************************************/
//#define delayCNTL_stack R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
//#define delayCNTH_stack R25
//#define delayNx4cycRmd_stack  R0 // Remainder
//funcDelayN4xCycle16: // 32 x bytes
// ref 1.avr-libc and 2.Precision delay
// need push n-cycle/4 and <<2 into delayCNT before rcall
// 20c overhead, 20+[4x4=16,D65532/0xFFFCx4=D262,128/0x3FFF0]+[0,1,2,3]
// =[36,D262,151/0x40007]
                                    // RCALL 3/4 overhead
      //PUSH  delayCNTH_stack             // 2c
      //PUSH  delayCNTL_stack             // 2c
      //RCALL funcDelayN4xCycle16
      //POP   delayCNTL_stack             // 2c
      //POP   delayCNTH_stack             // 2c
      //_mcp  SPH,ZH                // 1c
      //_mcp  SPL,ZL                // 1c
      //_mcp_p Z,delayCNTL_stack,d,3      // 2c
      //_mcp_p Z,delayCNTH_stack,d,4      // 2c
      //_mcp  delayCNTL_stack,delayNx4cycRmd_stack  // 1c
      //CBR   delayCNTL_stack,0x03        // 1c, clr 0,1-bit, 2+2+1+1=6c overhead
  jp_funcDelayNCycle24_Loop_stack:
        SBIW  delayCNTL_stack,1           // 2c
        BRNE  .-4                   // f1c/t2c
  jp_funcDelayNCycle24_add_1_2_3_cyc_stack:
// fix 6c overhead for branch, [6,7,8]=6+[0,1,2] overhead
  _if <LSR delayNx4cycRmd_stack>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
  _Eif              // true 1+1+2=4c=3+1c, false 1+2 = 3c
  _if <LSR delayNx4cycRmd_stack>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
      NOP           // 1c
  _Eif              // true 1+1+2+1=5c=3+2c, false 1+2=3c
      RET                           // 4c/5c overhead
/*****************************************************************************/

/*****************************************************************************/
funcDelayNCycle24_reg: // 36c overhead+[4x4=16,4xD65535/0xFFFF=D262140/0x3FFFC]+[0,1,2,3]
// 36+16+[0,1,2,3] - 36+D262140+[0,1,2,3]
// =[52-8,             D262179/0x40023 -8]
#define delayCNTL_reg R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayCNTM_reg R25
#define delayCNTH_reg R26
#define delayNx4cycRmd_reg  R27 // Remainder
#define funcDelayNCycle24_OVERHEAD_reg 36-8
/*****************************************************************************/
  jp_call_method_gpr_reg:
        //_ldi24hl 52,delayCNTH_reg,delayCNTM_reg,delayCNTL_reg //3c
        //RCALL funcDelayNCycle24_reg           // RCALL 3/4 overhead
  jp_call_method_ram_reg:
        //_ldi24hl 52,delayCNTH_reg,delayCNTM_reg,delayCNTL_reg //3c
        //_mcp delayCNTH_reg, delayCNTH_ram_addr_reg
        //_mcp delayCNTM_reg, delayCNTM_ram_addr_reg 
        //_mcp delayCNTL_reg, delayCNTL_ram_addr_reg
  jp_call_method_sp_reg:
        //_pushki24hl 52
        //RCALL funcDelayNCycle24_reg           // RCALL 3/4 overhead
        //_pop24lh r16,r16,r16
  jp_get_arg_cnt_from_sp_reg: //reg, save -8c
        //_mcp  SPH,ZH                        // 1c
        //_mcp  SPL,ZL                        // 1c
        //_mcp_p Z,delayCNTL_reg,d,3              // 2c
        //_mcp_p Z,delayCNTM_reg,d,4              // 2c
        //_mcp_p Z,delayCNTH_reg,d,5              // 2c
  jp_get_arg_cnt_from_ram_reg:
        //_mcp delayCNTL_ram_addr_reg,delayCNTL_reg   // 2c
        //_mcp delayCNTM_ram_addr_reg,delayCNTM_reg   // 2c
        //_mcp delayCNTH_ram_addr_reg,delayCNTH_reg   // 2c
  jp_get_arg_cnt_from_gpr_reg:
  jp_calc_overhead_start_reg:
        _if <SBIW delayCNTL_reg,36-8>,cs  // true=1c, false=2c, overhead=
        //BRCS  .+2
        DEC   delayCNTH_reg
        _Eif                        // true=2+1+1=4c, false=2+2=4c
  jp_calc_overhead_done_reg:
  jp_divide_cnt_by_4_reg:
  jp_divide_cnt_by_2_1_reg:
        LSR   delayCNTH_reg            // 1c /2 H
        ROR   delayCNTM_reg            // 1c /2 M
        ROR   delayCNTL_reg            // 1c /2 L
  jp_save_remainder_bit0_reg:
        CLR   delayNx4cycRmd_reg       // 1c save remainder, clr 
        ROR   delayNx4cycRmd_reg       // 1c shift c/lsb to msb
  jp_divide_cnt_by_2_2_reg:
        LSR   delayCNTH_reg            // 1c /2 H
        ROR   delayCNTM_reg            // 1c /2 M
        ROR   delayCNTL_reg            // 1c /2 L = counter/4, now CNTH = 0
  jp_save_remainder_bit1_reg:
        ROR   delayNx4cycRmd_reg       // 1c shift c/lsb-b1 to msb
  jp_recover_remainder_reg:
        ROL   delayNx4cycRmd_reg       // 1c recover remainder b1->c
        ROL   delayNx4cycRmd_reg       // 1c recover remainder c->b1, b0->c
        ROL   delayNx4cycRmd_reg       // 1c recover remainder c->b0
  jp_divide_done_reg: // now quotient = CNTL:CNTM, remainder = rRmd
        //ADD   delayCNTL_reg,delayNx4cycRmd_reg // now get CNTL/M
        // pop ret address, push remainder, cntH/L, push ret addr
        //RET
/*****************************************************************************/
/*****************************************************************************/
//#define delayCNTL_reg R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
//#define delayCNTH_reg R25
//#define delayNx4cycRmd_reg  R0 // Remainder
//funcDelayN4xCycle16: // 32 x bytes
// ref 1.avr-libc and 2.Precision delay
// need push n-cycle/4 and <<2 into delayCNT before rcall
// 20c overhead, 20+[4x4=16,D65532/0xFFFCx4=D262,128/0x3FFF0]+[0,1,2,3]
// =[36,D262,151/0x40007]
                                    // RCALL 3/4 overhead
      //PUSH  delayCNTH_reg             // 2c
      //PUSH  delayCNTL_reg             // 2c
      //RCALL funcDelayN4xCycle16
      //POP   delayCNTL_reg             // 2c
      //POP   delayCNTH_reg             // 2c
      //_mcp  SPH,ZH                // 1c
      //_mcp  SPL,ZL                // 1c
      //_mcp_p Z,delayCNTL_reg,d,3      // 2c
      //_mcp_p Z,delayCNTH_reg,d,4      // 2c
      //_mcp  delayCNTL_reg,delayNx4cycRmd_reg  // 1c
      //CBR   delayCNTL_reg,0x03        // 1c, clr 0,1-bit, 2+2+1+1=6c overhead
  jp_funcDelayNCycle24_Loop_reg:
        SBIW  delayCNTL_reg,1           // 2c
        BRNE  .-4                   // f1c/t2c
  jp_funcDelayNCycle24_add_1_2_3_cyc_reg:
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

/*****************************************************************************/
funcDelayNCycle24_sram: // 36c overhead+[4x4=16,4xD65535/0xFFFF=D262140/0x3FFFC]+[0,1,2,3]
// 36+16+[0,1,2,3] - 36+D262140+[0,1,2,3]
// =[52-2,             D262179/0x40023 -2]
#define delayCNTL_sram R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
#define delayCNTM_sram R25
#define delayCNTH_sram R26
#define delayNx4cycRmd_sram  R27 // Remainder
#define delayCNTL_ram_addr_sram 0x100
#define delayCNTM_ram_addr_sram 0x101
#define delayCNTH_ram_addr_sram 0x102
#define funcDelayNCycle24_OVERHEAD_sram 36-2
/*****************************************************************************/
  jp_call_method_gpr_sram:
        //_ldi24hl 52,delayCNTH_sram,delayCNTM_sram,delayCNTL_sram //3c
        //RCALL funcDelayNCycle24_sram           // RCALL 3/4 overhead
  jp_call_method_ram_sram:
        //_ldi24hl 52,delayCNTH_sram,delayCNTM_sram,delayCNTL_sram //3c
        //_mcp delayCNTH_sram, delayCNTH_ram_addr_sram
        //_mcp delayCNTM_sram, delayCNTM_ram_addr_sram 
        //_mcp delayCNTL_sram, delayCNTL_ram_addr_sram
  jp_call_method_sp_sram:
        //_pushki24hl 52
        //RCALL funcDelayNCycle24_sram           // RCALL 3/4 overhead
        //_pop24lh r16,r16,r16
  jp_get_arg_cnt_from_sp_sram: //reg, save -8c
        //_mcp  SPH,ZH                        // 1c
        //_mcp  SPL,ZL                        // 1c
        //_mcp_p Z,delayCNTL_sram,d,3              // 2c
        //_mcp_p Z,delayCNTM_sram,d,4              // 2c
        //_mcp_p Z,delayCNTH_sram,d,5              // 2c
  jp_get_arg_cnt_from_ram_sram:
        _mcp delayCNTL_ram_addr_sram,delayCNTL_sram   // 2c
        _mcp delayCNTM_ram_addr_sram,delayCNTM_sram   // 2c
        _mcp delayCNTH_ram_addr_sram,delayCNTH_sram   // 2c
  jp_get_arg_cnt_from_gpr_sram:
  jp_calc_overhead_start_sram:
        _if <SBIW delayCNTL_sram,36-2>,cs  // true=1c, false=2c, overhead=
        //BRCS  .+2
        DEC   delayCNTH_sram
        _Eif                        // true=2+1+1=4c, false=2+2=4c
  jp_calc_overhead_done_sram:
  jp_divide_cnt_by_4_sram:
  jp_divide_cnt_by_2_1_sram:
        LSR   delayCNTH_sram            // 1c /2 H
        ROR   delayCNTM_sram            // 1c /2 M
        ROR   delayCNTL_sram            // 1c /2 L
  jp_save_remainder_bit0_sram:
        CLR   delayNx4cycRmd_sram       // 1c save remainder, clr 
        ROR   delayNx4cycRmd_sram       // 1c shift c/lsb to msb
  jp_divide_cnt_by_2_2_sram:
        LSR   delayCNTH_sram            // 1c /2 H
        ROR   delayCNTM_sram            // 1c /2 M
        ROR   delayCNTL_sram            // 1c /2 L = counter/4, now CNTH = 0
  jp_save_remainder_bit1_sram:
        ROR   delayNx4cycRmd_sram       // 1c shift c/lsb-b1 to msb
  jp_recover_remainder_sram:
        ROL   delayNx4cycRmd_sram       // 1c recover remainder b1->c
        ROL   delayNx4cycRmd_sram       // 1c recover remainder c->b1, b0->c
        ROL   delayNx4cycRmd_sram       // 1c recover remainder c->b0
  jp_divide_done_sram: // now quotient = CNTL:CNTM, remainder = rRmd
        //ADD   delayCNTL_sram,delayNx4cycRmd_sram // now get CNTL/M
        // pop ret address, push remainder, cntH/L, push ret addr
        //RET
/*****************************************************************************/
/*****************************************************************************/
//#define delayCNTL_sram R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
//#define delayCNTH_sram R25
//#define delayNx4cycRmd_sram  R0 // Remainder
//funcDelayN4xCycle16: // 32 x bytes
// ref 1.avr-libc and 2.Precision delay
// need push n-cycle/4 and <<2 into delayCNT before rcall
// 20c overhead, 20+[4x4=16,D65532/0xFFFCx4=D262,128/0x3FFF0]+[0,1,2,3]
// =[36,D262,151/0x40007]
                                    // RCALL 3/4 overhead
      //PUSH  delayCNTH_sram             // 2c
      //PUSH  delayCNTL_sram             // 2c
      //RCALL funcDelayN4xCycle16
      //POP   delayCNTL_sram             // 2c
      //POP   delayCNTH_sram             // 2c
      //_mcp  SPH,ZH                // 1c
      //_mcp  SPL,ZL                // 1c
      //_mcp_p Z,delayCNTL_sram,d,3      // 2c
      //_mcp_p Z,delayCNTH_sram,d,4      // 2c
      //_mcp  delayCNTL_sram,delayNx4cycRmd_sram  // 1c
      //CBR   delayCNTL_sram,0x03        // 1c, clr 0,1-bit, 2+2+1+1=6c overhead
  jp_funcDelayNCycle24_Loop_sram:
        SBIW  delayCNTL_sram,1           // 2c
        BRNE  .-4                   // f1c/t2c
  jp_funcDelayNCycle24_add_1_2_3_cyc_sram:
// fix 6c overhead for branch, [6,7,8]=6+[0,1,2] overhead
  _if <LSR delayNx4cycRmd_sram>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
  _Eif              // true 1+1+2=4c=3+1c, false 1+2 = 3c
  _if <LSR delayNx4cycRmd_sram>,cs // 1c LSR, 1/2c BRCC
      RJMP .        // 2c
      NOP           // 1c
  _Eif              // true 1+1+2+1=5c=3+2c, false 1+2=3c
      RET                           // 4c/5c overhead
/*****************************************************************************/
