/*
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
// default offset 0x20 
##define __SFR_OFFSET  0x20
#include <avr/io.h>
#include "avr_macro_common.S"
#include "avr_macro_dtrans.S"
*/

.altmacro

.macro  _bit_not_gpr reg:req, bit:req
            COM \reg
            BST \reg, \bit
            COM \reg
            BLD \reg, \bit
.endm
.macro  _bit_not xaddr:req, bit:req, rtmp=r16
        // bit from GPR to ram
        _get_aaddr \xaddr
        __mem_type_bit_not = __mem_type
        .if (__mem_type_bit_not == _GPR)
        #.if (__abs_addr_dst == __abs_addr_rtmp)
            _bit_not_gpr \xaddr, \bit
        .else
            _mcp \xaddr, \rtmp
            _bit_not_gpr \rtmp, \bit
            _mcp \rtmp,  \xaddr
        .endif
.endm

.macro  _bit_wr_sreg bit:req, value:req, reg_sreg=SREG
# not check range
  _get_aaddr \reg_sreg
    .if (__mem_part_sreg == _SREG)
        .if     (\value == 1)
            BSET \bit
        .elseif (\value == 0)
            BCLR \bit
        .else
          .warning "set value error"
        .endif
    .else 
      .warning "reg_sreg != _SREG"
    .endif
.endm
.macro  _bit_wr_sfrl reg_sfrl:req, bit:req, value:req
# not check range
        .if     (\value == 1)
          #if defined(__SFR_OFFSET)&&(__SFR_OFFSET==0x20)
          SBI   \reg_sfrl-0x20, \bit
          #elif defined(__SFR_OFFSET)&&(__SFR_OFFSET==0)
          SBI   \reg_sfrl, \bit
          #endif
        .elseif (\value == 0)
          #if defined(__SFR_OFFSET)&&(__SFR_OFFSET==0x20)
          CBI   \reg_sfrl-0x20, \bit
          #elif defined(__SFR_OFFSET)&&(__SFR_OFFSET==0)
          CBI   \reg_sfrl, \bit
          #endif
        .else
          .warning "set value error"
        .endif
.endm
.macro  _bit_wr_gprh gprh:req, bit:req, value:req
# not check range
        .if     (\value == 1)
          SBR   \gprh, (1<<\bit) /* ORI */
        .elseif (\value == 0)
          CBR   \gprh, (1<<\bit) /* ANDI */
        .else
          .warning "set value error"
        .endif
.endm
.macro  _bit_wr xaddr:req, bit:req, value:req, rtmp=r16
        _get_aaddr \xaddr
        .if (__mem_part == _GPRH)
          _bit_wr_gprh \xaddr, \bit, \value
        .elseif (__mem_part == _SFRL)
          _bit_wr_sfrl \xaddr, \bit, \value
        .elseif ((__mem_part >= _SFRH)||(__mem_part == _GPRL))
          .if (__mem_part_sreg == _SREG)
            _bit_wr_sreg \bit, \value
            .exitm
          .endif
          _mcp \xaddr, \rtmp
          _bit_wr_gprh \rtmp, \bit, \value
          _mcp \rtmp, \xaddr
        .endif
.endm

.macro  _mem_andi dst:req, k8:req, __rhtmp=r16
  _macro_mem_andi_s_\@:
  _get_aaddr \dst
  __mem_type_dst_mem_andi = __mem_type
  __mem_part_dst_mem_andi = __mem_part
  .if (__mem_part_dst_mem_andi == _GPRH)
      ANDI  \dst,    \k8
  .else
      _mcp  \dst,    \__rhtmp
      ANDI  \__rhtmp, \k8
      _mcp  \__rhtmp, \dst
    .endif
  _macro_mem_andi_e_\@:
.endm
.macro  _mem_ori dst:req, k8:req, __rhtmp=r16
  _macro_mem_ori_s_\@:
  _get_aaddr \dst
  __mem_type_dst_mem_ori = __mem_type
  __mem_part_dst_mem_ori = __mem_part
  .if (__mem_part_dst_mem_ori == _GPRH)
      ORI   \dst,    \k8
  .else
      _mcp  \dst,    \__rhtmp
      ORI   \__rhtmp, \k8
      _mcp  \__rhtmp, \dst
    .endif
  _macro_mem_ori_e_\@:
.endm
.macro  _mem_set_bits dst:req, k8:req, __rhtmp=r16
  _macro_mem_set_bit_s_\@:
        _mem_ori \dst, \k8
  _macro_mem_set_bit_e_\@:
.endm
.macro  _mem_clr_bits dst:req, k8:req, __rhtmp=r16
  _macro_mem_clr_bit_s_\@:
  _get_aaddr \dst
  __mem_type_dst_mem_clr_bit = __mem_type
  __mem_part_dst_mem_clr_bit = __mem_part
  .if (__mem_part_dst_mem_clr_bit == _GPRH)
      ANDI  \dst,    0xFF-\k8
  .else
      _mcp  \dst,    \__rhtmp
      ANDI  \__rhtmp, 0xFF-\k8
      _mcp  \__rhtmp, \dst
    .endif
  _macro_mem_clr_bit_e_\@:
.endm
.macro  _mem_bit_wr _xaddr:req, _bit:req, _value:req
  // check _value if avaliable not only 0/1 but also true/false, yes/no, set/clr
  _macro_mem_bit_wr_s_\@:
    _get_bin \_value
    .if (__binary_code == -1)
        .warning "input _value is not valid 0/1"
        .exitm
    .elseif (__binary_code == 1)
        #_mem_set_bit \_xaddr, <1!<!<\_value>
        _bit_wr \_xaddr, \_bit, 1
        .exitm
    .elseif (__binary_code == 0)
        #_mem_clr_bit \_xaddr, <1!<!<\_value>
        _bit_wr \_xaddr, \_bit, 1
        .exitm
    .endif
  _macro_mem_bit_wr_e_\@:
.endm

.macro  _get_bit _xaddr:req, _bit:req, _rtmp=r16
  // get _bit[0,7] value from _xaddr[all ram] and save to SREG_T
  _macro_get_bit_s_\@:
    //__get_bit_index = 0
        _if_bit \_xaddr, \_bit, set, <_macro_get_bit_clr_\@>, \_rtmp
            BSET SREG_T // if set
            RJMP _macro_get_bit_end_\@
  _macro_get_bit_clr_\@:
            BCLR SREG_T // if clr
  _macro_get_bit_end_\@:
  _macro_get_bit_e_\@:
.endm

/******************************************************************************\
  Addition
\******************************************************************************/
.macro  _add8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_add8_start_\@:
    // _mem_opcode_2x_addr from avr_macro_common.S
    _mem_opcode_2x_addr ADD, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_add8_end_\@:
.endm
.macro  _addc8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_adc8_start_\@:
    _mem_opcode_2x_addr ADC, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_addc8_end_\@:
.endm

.macro  _sub8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_sub8_start_\@:
    _mem_opcode_2x_addr SUB, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_sub8_end_\@:
.endm
.macro  _subc8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_subc8_start_\@:
    _mem_opcode_2x_addr SBC, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_subc8_end_\@:
.endm

.macro  _mul8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_mul8_start_\@:
    _mem_opcode_2x_addr MUL, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_mul8_end_\@:
.endm
.macro  _muls8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_muls8_start_\@:
    _mem_opcode_2x_addr MULS, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_muls8_end_\@:
.endm
.macro  _mulsu8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_mulsu8_start_\@:
    _mem_opcode_2x_addr MULSU, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_mulsu8_end_\@:
.endm

.macro  _fmul8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_fmul8_start_\@:
    _mem_opcode_2x_addr FMUL, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_fmul8_end_\@:
.endm
.macro  _fmuls8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_fmuls8_start_\@:
    _mem_opcode_2x_addr FMULS, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_fmuls8_end_\@:
.endm
.macro  _fmulsu8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_fmulsu8_start_\@:
    _mem_opcode_2x_addr FMULSU, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_fmulsu8_end_\@:
.endm

.macro  _and8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_and8_start_\@:
    _mem_opcode_2x_addr AND, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_and8_end_\@:
.endm
.macro  _or8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_or8_start_\@:
    _mem_opcode_2x_addr OR, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_or8_end_\@:
.endm
.macro  _eor8 _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_eor8_start_\@:
    _mem_opcode_2x_addr EOR, \_src, \_dst, \_rtmp1, \_rtmp2
  _macro_eor8_end_\@:
.endm

.macro  _com8 _dst:req, _rtmp1=r16
  _macro_com8_start_\@:
    _mem_opcode_1x_addr COM, \_src, \_dst, \_rtmp1
  _macro_com8_end_\@:
.endm
.macro  _neg8 _dst:req, _rtmp1=r16
  _macro_neg8_start_\@:
    _mem_opcode_1x_addr NEG, \_src, \_dst, \_rtmp1
  _macro_neg8_end_\@:
.endm
.macro  _inc8 _dst:req, _rtmp1=r16
  _macro_inc8_start_\@:
    _mem_opcode_1x_addr INC, \_src, \_dst, \_rtmp1
  _macro_inc8_end_\@:
.endm
.macro  _dec8 _dst:req, _rtmp1=r16
  _macro_dec8_start_\@:
    _mem_opcode_1x_addr DEC, \_src, \_dst, \_rtmp1
  _macro_dec8_end_\@:
.endm

.macro  _lsl8 _dst:req, _rtmp1=r16
  _macro_lsl8_start_\@:
    _mem_opcode_1x_addr LSL, \_src, \_dst, \_rtmp1
  _macro_lsl8_end_\@:
.endm
.macro  _lsr8 _dst:req, _rtmp1=r16
  _macro_lsr8_start_\@:
    _mem_opcode_1x_addr LSR, \_src, \_dst, \_rtmp1
  _macro_lsr8_end_\@:
.endm
.macro  _rol8 _dst:req, _rtmp1=r16
  _macro_rol8_start_\@:
    _mem_opcode_1x_addr ROL, \_src, \_dst, \_rtmp1
  _macro_rol8_end_\@:
.endm
.macro  _ror8 _dst:req, _rtmp1=r16
  _macro_ror8_start_\@:
    _mem_opcode_1x_addr ROR, \_src, \_dst, \_rtmp1
  _macro_ror8_end_\@:
.endm
.macro  _asr8 _dst:req, _rtmp1=r16
  _macro_asr8_start_\@:
    _mem_opcode_1x_addr ASR, \_src, \_dst, \_rtmp1
  _macro_asr8_end_\@:
.endm
.macro  _swap8 _dst:req, _rtmp1=r16
  _macro_swap8_start_\@:
    _mem_opcode_1x_addr SWAP, \_src, \_dst, \_rtmp1
  _macro_swap8_end_\@:
.endm
