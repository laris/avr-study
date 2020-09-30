##define __AVR_ATtiny85__
##define __AVR_ATmega328P__
/* default offset 0x20 */
##define __SFR_OFFSET  0x20
##include <avr/io.h>
##include "avr_macro_common.S"

##define __DEBUG_PMCP__ 1
##define __DEBUG_MCP_P__ 1

.macro  _mcp src:req, dst:req, rhtmp=r16
  // copy src to dst, src/dst = any memory space, include gpr/sfr/sram
  __mem_type_src = -1
  __mem_part_src = -1
  __abs_addr_src = -1
  __mem_type_dst = -1
  __mem_part_dst = -1
  __abs_addr_dst = -1
  _get_aaddr \src
    __mem_type_src = __mem_type
    __mem_part_src = __mem_part
    __abs_addr_src = __abs_addr
    .if (__mem_type_src == _GPR)
      #.print "__mem_type_src = _GPR"
    .endif
    .if (__mem_part_src == _GPRL)
      #.print "__mem_part_src = _GPRL"
    .endif
  _get_aaddr \dst
    __mem_type_dst = __mem_type
    __mem_part_dst = __mem_part
    __abs_addr_dst = __abs_addr
    .if (__mem_type_dst == _GPR)
      #.print "__mem_type_dst = _GPR"
    .endif
    .if (__mem_part_dst == _GPRH)
      #.print "__mem_part_dst = _GPRH"
    .endif
  .if (__abs_addr_src == __abs_addr_dst)
    .warning "Warn: src == dst"
    #.error   "Warn: src == dst"
    .exitm
  .endif
  /* now src != dst 
  .if ((__abs_addr_src - __abs_addr_dst)<0)
    .print "src < dst"
  .elseif ((__abs_addr_src - __abs_addr_dst)>0)
    .print "src > dst"
  .endif
  */
  /* src = _GPR */
  // gpr gpr
  .if ((__mem_type_src == _GPR)&&(__mem_type_dst == _GPR))
    MOV __abs_addr_dst, __abs_addr_src
    #.print "gpr-gpr:\t MOV dst, src"
    .exitm
  .endif
  // gpr sfr
  .if ((__mem_type_src == _GPR)&&(__mem_type_dst == _SFR))
    OUT (__abs_addr_dst - __SFR_OFFSET), __abs_addr_src
    #.print "gpr-sfr:\t OUT [dst-__SFR_OFFSET], src"
    .exitm
  .endif
  // gpr esfr isram esram
  .if ((__mem_type_src == _GPR)&&(__mem_type_dst >= _ESFR)&&(__mem_type_dst <= _ESRAM))
    STS __abs_addr_dst, __abs_addr_src
    #.print "gpr-esfr+:\t STS dst, src"
    .exitm
  .endif
  /* src = _SFR */
  // sfr gpr
  .if ((__mem_type_src == _SFR)&&(__mem_type_dst == _GPR))
    IN __abs_addr_dst, (__abs_addr_src - __SFR_OFFSET)
    #.print "sfr-gpr:\t  IN dst, [src-__SFR_OFFSET]"
    .exitm
  .endif
  // sfr sfr
  .if ((__mem_type_src == _SFR)&&(__mem_type_dst == _SFR))
    .ifnes "rhtmp",""
      #.print "sfr-sfr:\t  IN rhtmp, src \n\t\t OUT [dst-__SFR_OFFSET], rhtmp"
      IN  \rhtmp, (__abs_addr_src - __SFR_OFFSET)
      OUT (__abs_addr_dst - __SFR_OFFSET), \rhtmp
      .exitm
    .else
      .warning "sfr sfr need temp reg"
    .endif
  .endif
  // sfr esfr isram esram
  .if ((__mem_type_src == _SFR)&&(__mem_type_dst >= _ESFR)&&(__mem_type_dst <= _ESRAM))
    .ifnes "rhtmp",""
      #.print "sfr-esfr+:\t  IN rhtmp, [src-__SFR_OFFSET] \n\t\t STS dst, rhtmp"
      IN  \rhtmp, (__abs_addr_src - __SFR_OFFSET)
      STS __abs_addr_dst, \rhtmp
      .exitm
    .else
      .warning "sfr esfr+ need temp reg"
    .endif
  .endif
  /* src = esfr */
  // esfr gpr
  .if ((__mem_type_src >= _ESFR)&&(__mem_type_src <= _ESRAM)&&(__mem_type_dst == _GPR))
    #.print "esfr+-gpr: \t LDS dst, src"
    LDS __abs_addr_dst, __abs_addr_src
    .exitm
  .endif
  // esfr sfr
  .if ((__mem_type_src >= _ESFR)&&(__mem_type_src <= _ESRAM)&&(__mem_type_dst == _SFR))
    .ifnes "rhtmp",""
      #.print "esfr+-sfr:\t LDS rhtmp, src \n\t\t OUT [dst-__SFR_OFFSET], rhtmp"
      LDS  \rhtmp, __abs_addr_src
      OUT (__abs_addr_dst - __SFR_OFFSET), \rhtmp
      .exitm
    .else
      .warning "esfr sfr need temp reg"
    .endif
  .endif
  // esfr esfr isram esram
  .if ((__mem_type_src >= _ESFR)&&(__mem_type_src <= _ESRAM)&&\
       (__mem_type_dst >= _ESFR)&&(__mem_type_dst <= _ESRAM))
    .ifnes "rhtmp",""
      #.print "esfr+-esfr+: \t LDS rhtmp, src \n\t\t STS dst, rhtmp"
      LDS  \rhtmp, __abs_addr_src
      STS __abs_addr_dst, \rhtmp
      .exitm
    .else
      .warning "esfr esfr+ need temp reg"
    .endif
  .endif

.endm

.macro _mld dst:req, ptr:req, mod=0, dp
  // load into dst/gpr from ptr with [+/-/disp]
  .ifc  mod,0
    LD  \dst, \ptr
    .print "LD dst, ptr"
  .endif
  .ifc  \mod,-
    LD  \dst, \mod\ptr
    .print "LD dst, mod ptr"
  .endif
  .ifc  \mod,+
    LD  \dst, \ptr\mod
    .print "LD dst, ptr mod"
  .endif
  .ifc  \mod,d
    .print "LDD dst, ptr+dp"
    LDD \dst, \ptr+\dp
  .endif
.endm
.macro _mst ptr:req, src:req, mod=0, dp
  // store into ptr from src/gpr with [+/-/disp] 
  .ifc  mod,0
    ST  \ptr, \src
    .print "ST ptr, src"
  .endif
  .ifc  \mod,-
    ST  \mod\ptr, \src
    .print "ST mod ptr, src"
  .endif
  .ifc  \mod,+
    ST  \ptr\mod, \src
    .print "ST ptr mod, src"
  .endif
  .ifc  \mod,d
    .print "STD ptr+dp, src"
    STD \ptr+\dp, \src
  .endif
.endm
/* mem copy from src to dst, src/dst can use x/y/z, mod=[+/-/d], dp */
.macro _mcp_p src:req, dst:req, mod=0, dp, rhtmp=r16
  __mem_type_src = -1
  __mem_part_src = -1
  __abs_addr_src = -1
  __mem_type_dst = -1
  __mem_part_dst = -1
  __abs_addr_dst = -1
  _get_aaddr \src
    __mem_type_src = __mem_type
    __mem_part_src = __mem_part
    __abs_addr_src = __abs_addr
  _get_aaddr \dst
    __mem_type_dst = __mem_type
    __mem_part_dst = __mem_part
    __abs_addr_dst = __abs_addr
  .if (__abs_addr_src == __abs_addr_dst)
    .warning "Warn: src == dst"
    #.error   "Warn: src == dst"
    .exitm
  .endif

  __ptr_type_src = -1
  __ptr_type_dst = -1
  _get_ptr_type \src
    __ptr_type_src = __ptr_type
  _get_ptr_type \dst
    __ptr_type_dst = __ptr_type

  .if ((__ptr_type_src == -1) && (__ptr_type_dst == -1))
    #if defined (__DEBUG_MCP_P__)
    .print "None pointer exist"
    #endif
    .print "None pointer exist"
    #.error "None pointer exist"
    .exitm
  .endif
  .if ((__ptr_type_src != -1) && (__ptr_type_dst == -1))
    #if defined (__DEBUG_MCP_P__)
    .print "LoaD from src ptr to dst"
    #endif
    .if (__mem_type_dst == _GPR)
      _mld  \dst, \src, \mod, \dp
    .elseif ((__mem_type_dst >= _SFR) && (__mem_type_dst <= _ESRAM))
      _mld  \rhtmp, \src, \mod, \dp
      _mcp  \rhtmp, \dst
    .endif
    .exitm
  .endif
  .if ((__ptr_type_src == -1) && (__ptr_type_dst != -1))
    #if defined (__DEBUG_MCP_P__)
    .print "STore from src to dst ptr"
    #endif
    .if (__mem_type_src == _GPR)
      _mst  \dst, \src, \mod, \dp
    .elseif ((__mem_type_src >= _SFR) && (__mem_type_dst <= _ESRAM))
      _mcp  \src, \rhtmp
      _mst  \dst, \rhtmp, \mod, \dp
    .endif
    .exitm
  .endif
  .if ((__ptr_type_src != -1) && (__ptr_type_dst != -1))
    #if defined (__DEBUG_MCP_P__)
    .print "load/store from src ptr to dst ptr"
    #endif
      LD   \rhtmp, \src
      ST   \dst,  \rhtmp
    .exitm
  .endif
.endm
/* mem write k, write immediate k8[0,255] to memory byte location */
.macro _mwrk k8:req, dst:req, rhtmp=r16
/*      mem write konstant value (can convert to integer) */
#  .if !((\k8 >= -128)&&(\k8 <=255))
#              #0x80=-128 0x7F=127 
#    .exitm
#  .endif
  _get_aaddr \dst
  .if (__mem_part == _GPRH)
    LDI \dst, \k8
    #.print "k8-GPRH:\t LDI dst, k8"
    .exitm
  .endif
  .if (__mem_part != _GPRH)
    LDI  \rhtmp, \k8
    _mcp \rhtmp, \dst
    #.print "k8-!GPRH:\t LDI rhtmp, k8 \n\t\t _mcp rhtmp, dst"
    .exitm
  .endif
.endm
.macro  _mwrki k8:req, dst:req, rhtmp=r16
/*      mem write konstant-integer */
  .if !((\k8 >= -128)&&(\k8 <=255))
              #0x80=-128 0x7F=127 
    .exitm
  .endif
  _get_aaddr \dst
  .if (__mem_part == _GPRH)
    LDI \dst, \k8
    #.print "k8-GPRH:\t LDI dst, k8"
    .exitm
  .endif
  .if (__mem_part != _GPRH)
    LDI  \rhtmp, \k8
    _mcp \rhtmp, \dst
    #.print "k8-!GPRH:\t LDI rhtmp, k8 \n\t\t _mcp rhtmp, dst"
    .exitm
  .endif
.endm
.macro _mwrk_p k8:req, ptr:req, mod=0, dp, rhtmp=r16
  .if !((\k8 >= -128)&&(\k8 <=255))
              #0x80=-128 0x7F=127 
    #.error "K8 error"
    .warning "K8 error"
    .exitm
  .endif
  _get_ptr_type \ptr
  .if (__mem_type != _PTR)
    #.error "PTR error"
    .warning "PTR error"
    .exitm
  .endif
  _mwrk  \k8,   \rhtmp
  _mcp_p \rhtmp, \ptr, \mod, \dp, \rhtmp
.endm
.macro _pmrd_gpr dst, pi
  // program mem read to dst[r0/gpr] from ptr[Z] with option pi[+](ptr inc)
  .ifc \dst,
    .ifc \pi,
      LPM
    .endif
  .endif
  .ifnc \dst,
    .ifc \pi,
      LPM \dst, Z
    .endif
    .ifc \pi,+
      LPM \dst, Z+
    .endif
  .endif
.endm
.macro _pmcp dst, pi, rhtmp=r16
  // copy program mem to dst[any data mem] from ptr[Z] with option pi[+](ptr inc)
  .ifc \dst,
    .ifc \pi,
      _pmrd_gpr
    .endif
    .exitm
  .endif
  .ifnc \dst,
    #if defined (__DEBUG_PMCP__)
    .print "dst not null"
    #endif
    _get_aaddr \dst
    .if ((__abs_addr == 30) || (__abs_addr == 31))
      .warning "cannot use dst=R30:R31 as DST = dst"
      #.error "cannot use dst=R30:R31 as DST = dst"
    .elseif ((__abs_addr >= 0) && (__abs_addr < 30))
      #if defined (__DEBUG_PMCP__)
      .print "GPR[0,30)"
      #endif
      _pmrd_gpr \dst, \pi
      #if defined (__DEBUG_PMCP__)
      .print "_pmrd_gpr dst, pi"
      #endif
/*
      .ifnc \pi,
        #if defined (__DEBUG_PMCP__)
        .print "pi=null"
        #endif
        _pmrd_gpr \dst
      .endif
      .ifc  \pi,+
      .endif
*/
    .elseif ((__mem_type >= _SFR) && (__mem_type <= _ESRAM))
/*
      .ifnc \pi,
        _pmrd_gpr \rhtmp
      .endif
      .ifc  \pi,+
        _pmrd_gpr \rhtmp, +
      .endif
*/
      _pmrd_gpr \rhtmp, \pi
      _mcp \rhtmp, \dst
    .else
      .print "ERR: dst out of ESRAM"
    .endif
  .endif
  #if defined (__DEBUG_PMCP__)
  #.print "_pmcp end"
  #endif
.endm

.macro  _mwrki16 src_k16:req, dst:req, rhtmp=r16
// not work 
        _mwrk lo8(\src_k16),\dst
        _mwrk hi8(\src_k16),\dst+1
.endm
.macro  _mwrki24 src_k24:req, dst_lo8:req, dst_hi8:req, dst_hh8:req
        LDI dst_lo8, lo8(\src_k24)
        LDI dst_hi8, hi8(\src_k24)
        LDI dst_hh8, hh8(\src_k24)
.endm
.macro  _ldi16hl src_k16:req, dst_hi8:req, dst_lo8:req
        LDI   \dst_hi8, hi8(\src_k16)
        LDI   \dst_lo8, lo8(\src_k16)
.endm
.macro  _ldi24hl src_k16:req, dst_hh8:req, dst_hi8:req, dst_lo8:req
        LDI   \dst_hh8,  hh8(\src_k16)
        LDI   \dst_hi8,  hi8(\src_k16)
        LDI   \dst_lo8,  lo8(\src_k16)
.endm
.macro  _ldi32hl src_k32:req, dst_hhi8:req, dst_hlo8:req, dst_hi8:req, dst_lo8:req
        LDI   \dst_hhi8, hhi8(\src_k32)
        LDI   \dst_hlo8, hlo8(\src_k32)
        LDI   \dst_hi8,   hi8(\src_k32)
        LDI   \dst_lo8,   lo8(\src_k32)
.endm
/* The Stack and Function Calls p205pp236
One final note regarding pushing a word onto the stack. 
It is customary (but not required) to push the high byte first. 
Because of the orientation of the stack, this results in the word 
being placed on the stack in little-endian order. 
If you consistently use this convention, you will be less likely to 
make mistakes when accessing multibyte data. 
However, the return address is an exception to this convention. 
The call instruction places the return address on the stack in big-endian 
order! We will need to know this in the following section. 
Unless noted otherwise, we will always follow the little-endian convention 
for data on the stack.
*/
.macro  _pushki16hl src_k16:req, rhtmp=r16
// push high first with little-endian, little: store/tx lsb firstly in ram
// call  low first with big-endian to save return address
        LDI   \rhtmp, hi8(\src_k16)
        PUSH  \rhtmp
        LDI   \rhtmp, lo8(\src_k16)
        PUSH  \rhtmp
.endm
.macro  _pop16lh dst_lo8:req, dst_hi8:req
        POP  dst_lo8
        POP  dst_hi8
.endm
.macro  _pushki24hl src_k24:req, rhtmp=r16
        LDI   \rhtmp, hh8(\src_k24)
        PUSH  \rhtmp
        LDI   \rhtmp, hi8(\src_k24)
        PUSH  \rhtmp
        LDI   \rhtmp, lo8(\src_k24)
        PUSH  \rhtmp
.endm
.macro  _pop24lh dst_lo8:req, dst_hi8:req, dst_hh8:req
        POP  dst_lo8
        POP  dst_hi8
        POP  dst_hh8
.endm
.macro  _pushki32hl src_k32:req, rhtmp=r16
        LDI   \rhtmp, hhi8(\src_k32)
        PUSH  \rhtmp
        LDI   \rhtmp, hlo8(\src_k32)
        PUSH  \rhtmp
        LDI   \rhtmp,  hi8(\src_k32)
        PUSH  \rhtmp
        LDI   \rhtmp,  lo8(\src_k32)
        PUSH  \rhtmp
.endm
.macro  _pop32lh dst_lo8:req, dst_hi8:req, dst_hlo8:req, dst_hhi8:req
        POP  dst_lo8
        POP  dst_hi8
        POP  dst_hlo8
        POP  dst_hhi8
.endm

.macro  _mcp_mask src:req, dst:req, mask:req, rhtmp1=r16, rhtmp2=r17, rhtmp3=r18
        // copy n x bits(mask=1 bits) from src to dst, src not change
        // src/dst = _GPRH, no rhtmp req
        // src/dst have 1 x _GPRH, req 1 x rhtmp
        // src/dst no _GPRH, req 2 x rhtmp
        // src/dst no GPR, req rhtmp3
L_macro_mcp_mask_s\@:
/*
                              // msk = 0b0011_0000
        _mcp \dst,  \rhtmp    // dst = 0b0001_1000
        CBR  \rhtmp,\mask     // dst = 0b0000_1000
                              // src = 0bxx10_xxxx
        ANDI \src,  \mask     // src = 0b0010_0000
        OR   \rhtmp,\src      // dst = 0b0010_1000
        _mcp \rhtmp,\dst
*/
  _get_aaddr \src
  __mem_type_src_mcp_mask = __mem_type
  __mem_part_src_mcp_mask = __mem_part
  _get_aaddr \dst
  __mem_type_dst_mcp_mask = __mem_type
  __mem_part_dst_mcp_mask = __mem_part
  .if (__mem_part_dst_mcp_mask == _GPRH)
      CBR  \dst, \mask
      _mcp \src, \rhtmp1, \rhtmp3
      ANDI \rhtmp1, \mask
      OR   \dst, \rhtmp1
      .exitm
  .else
      _mcp \dst,    \rhtmp1, \rhtmp3
      CBR  \rhtmp1, \mask
      _mcp \src,    \rhtmp2, \rhtmp3
      ANDI \rhtmp2, \mask
      OR   \rhtmp1, \rhtmp2
      _mcp \rhtmp1, \dst,    \rhtmp3
      .exitm
    .endif
L_endm_mcp_mask_e\@:
.endm

