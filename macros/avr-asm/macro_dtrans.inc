#.include "m328p_def.inc"
#.include "macro_utils.inc"
.equ __DEB_DTRANS__, 1
.equ __AVR_SFR_OFFSET, 0x20
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* bit 
  _bit_wr_gprh _gprh:req, _bit:req, _value:req
  _bit_wr_sfrl _sfrl:req, _bit:req, _value:req
  _bit_wr_sreg            _bit:req, _value:req
  _bcp  _src:req, _src_bit, _dst:req, _dst_bit, _rhtmp=r24
        K[sym]->[T|RAM], T<->RAM, RAM<->RAM
  _bwrit _k1:req
  _bwrir _k1:req, _dst:req, _bit:req, _rhtmp
  _bcptr _dst:req, _bit:req, _rhtmp
  _bcprt _src:req, _bit:req, _rhtmp
  _bcprr _src:req, _src_bit:req, _dst:req, _dst_bit:req, _rhtmp
  _bcp_msk src:req, dst:req, msk:req, rhtmp1=r24, rhtmp2=r25, rhtmp3=r26
 * byte
  _mcp  _src:req, _dst:req, _rtmp=r24   RAM<->RAM
  _mwri _k8:req, _dst:req, _rhtmp=r24   k8 -> RAM
  x_mwri16 src_k16:req, dst:req, rhtmp=r24
  x_mwri24 src_k24:req, dst_lo8:req, dst_hi8:req, dst_hh8:req
  _ldi16hl src_k16:req, dst_hi8:req, dst_lo8:req
  _ldi24hl src_k16:req, dst_hh8:req, dst_hi8:req, dst_lo8:req
  _ldi32hl src_k32:req, dst_hhi8:req, dst_hlo8:req, dst_hi8:req, dst_lo8:req
  _mcpp _src:req, _sidp, _sdp, _rtmp=r24, _dst:req, _didp, _ddp
        [RAM]<->[P]+-d, [P]+-d <->[P]+-d
  _mwrip _k8:req, _rhtmp=r24, _ptr:req, _idp, _dp
  _nvmcp
  _nvmcpp
 */
/******************************************************************************/
/* debug symbol */
#.equ __DEB_BIT_TYPE__, 1
#.equ __DEB_BCP__,1
#.equ __DEB_MCP__,1
#.equ __DEB_MWRI__,1
#.equ __DEB_MCPP__,1
/******************************************************************************/
.macro  _mcp _src:req, _dst:req, _rtmp=r24
# memory copy --------------------------------------------------------
  _macro_mcp_s_\@:
  __mem_type_src_\@ = -1
  __mem_part_src_\@ = -1
  __mem_addr_src_\@ = -1
  __mem_type_dst_\@ = -1
  __mem_part_dst_\@ = -1
  __mem_addr_dst_\@ = -1
  _get_op \_src
    __mem_type_src_\@ = __mem_type
    __mem_part_src_\@ = __mem_part
    __mem_addr_src_\@ = __mem_addr
  _get_op \_dst
    __mem_type_dst_\@ = __mem_type
    __mem_part_dst_\@ = __mem_part
    __mem_addr_dst_\@ = __mem_addr
  #src = __MEM_TYPE_GPR --------------------------------------------------------
  #GPR-GPR --------------------------------------------------------
  .if ((__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_type_dst_\@==__MEM_TYPE_GPR))
    .if (__mem_addr_src_\@==__mem_addr_dst_\@)
      .warning "Warn: src==dst"
      .error   "Warn: src==dst"
      .exitm
    .else
      MOV __mem_addr_dst_\@, __mem_addr_src_\@
    .endif
    .exitm
  .endif
  #GPR-SFR --------------------------------------------------------
  .if ((__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_type_dst_\@==__MEM_TYPE_SFR))
    OUT (__mem_addr_dst_\@ - __AVR_SFR_OFFSET), __mem_addr_src_\@
    .exitm
  .endif
  #GPR-ESFR/RAM --------------------------------------------------------
  .if ((__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_type_dst_\@ >= __MEM_TYPE_ESFR))
    STS __mem_addr_dst_\@, __mem_addr_src_\@
    .exitm
  .endif
  #src = __MEM_TYPE_SFR --------------------------------------------------------
  #SFR-GPR --------------------------------------------------------
  .if ((__mem_type_src_\@==__MEM_TYPE_SFR)&&(__mem_type_dst_\@==__MEM_TYPE_GPR))
    IN __mem_addr_dst_\@, (__mem_addr_src_\@ - __AVR_SFR_OFFSET)
    .exitm
  .endif
  #SFR-SFR --------------------------------------------------------
  .if ((__mem_type_src_\@==__MEM_TYPE_SFR)&&(__mem_type_dst_\@==__MEM_TYPE_SFR))
    IN  \_rtmp, (__mem_addr_src_\@ - __AVR_SFR_OFFSET)
    OUT (__mem_addr_dst_\@ - __AVR_SFR_OFFSET), \_rtmp
    .exitm
  .endif
  #SFR-ESFR/RAM --------------------------------------------------------
  .if ((__mem_type_src_\@==__MEM_TYPE_SFR)&&(__mem_type_dst_\@ >= __MEM_TYPE_ESFR))
    IN  \_rtmp, (__mem_addr_src_\@ - __AVR_SFR_OFFSET)
    STS __mem_addr_dst_\@, \_rtmp
    .exitm
  .endif
  #src>=__MEM_TYPE_ESFR--------------------------------------------------------
  #ESFR/SRAM-GPR --------------------------------------------------------
  .if ((__mem_type_src_\@ >= __MEM_TYPE_ESFR)&&(__mem_type_dst_\@==__MEM_TYPE_GPR))
    LDS __mem_addr_dst_\@, __mem_addr_src_\@
    .exitm
  .endif
  #ESFR/SRAM-SFR --------------------------------------------------------
  .if ((__mem_type_src_\@ >= __MEM_TYPE_ESFR)&&(__mem_type_dst_\@==__MEM_TYPE_SFR))
    LDS  \_rtmp, __mem_addr_src_\@
    OUT (__mem_addr_dst_\@ - __AVR_SFR_OFFSET), \_rtmp
    .exitm
  .endif
  #ESFR/SRAM-ESFR/RAM --------------------------------------------------------
  .if ((__mem_type_src_\@ >= __MEM_TYPE_ESFR)&&(__mem_type_dst_\@ >= __MEM_TYPE_ESFR))
    LDS  \_rtmp, __mem_addr_src_\@
    STS __mem_addr_dst_\@, \_rtmp
    .exitm
  .endif
  _macro_mcp_e_\@:
.endm
/******************************************************************************/
.macro  _mwri _k8:req, _dst:req, _rhtmp=r24
# write immediate to ram address
  _macro_mwri_s_\@:
  _get_op \_dst
  .if (__mem_part==__MEM_PART_GPRH)
    LDI \_dst, \_k8
    .exitm
  .elseif (__mem_part != __MEM_PART_GPRH)&&(__mem_type <= __MEM_TYPE_ISRAM)
    LDI  \_rhtmp, \_k8
    _mcp \_rhtmp, \_dst
    .exitm
  .endif
  _macro_mwri_e_\@:
.endm
/******************************************************************************/
.macro  _mcpp _src:req, _sidp, _sdp, _rtmp=r24, _dst:req, _didp, _ddp
# memory copy with ptr
  _macro_mcpp_s_\@:
  # _define src/dst all info _-----------------------------------------------
    __mem_type_src_\@      = -1
    __mem_part_src_\@      = -1
    __mem_addr_src_\@      = -1
    __mem_ptr_src_\@       = -1
    __mem_ptr_type_src_\@  = -1
    __mem_ptr_sidp_src_\@  = -1
    __mem_type_dst_\@      = -1
    __mem_part_dst_\@      = -1
    __mem_addr_dst_\@      = -1
    __mem_ptr_dst_\@       = -1
    __mem_ptr_type_dst_\@  = -1
    __mem_ptr_didp_dst_\@  = -1
  _get_op \_src
  # get type and ptr --------------------------------------------------------
    __mem_type_src_\@      = __mem_type
    __mem_part_src_\@      = __mem_part
    __mem_addr_src_\@      = __mem_addr
    __mem_ptr_src_\@          = __mem_ptr
    __mem_ptr_type_src_\@     = __ptr_type
    _prt_debug_msg __DEB_MCPP__, debug_input=\_src,     __mem_type_src_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_src,     __mem_part_src_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_src,     __mem_addr_src_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_src,      __mem_ptr_src_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_src, __mem_ptr_type_src_\@
  _get_op \_dst
  # get type and ptr --------------------------------------------------------
    __mem_type_dst_\@      = __mem_type
    __mem_part_dst_\@      = __mem_part
    __mem_addr_dst_\@      = __mem_addr
    __mem_ptr_dst_\@       = __mem_ptr
    __mem_ptr_type_dst_\@  = __ptr_type
    _prt_debug_msg __DEB_MCPP__, debug_input=\_dst,     __mem_type_dst_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_dst,     __mem_part_dst_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_dst,     __mem_addr_dst_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_dst,      __mem_ptr_dst_\@
    _prt_debug_msg __DEB_MCPP__, debug_input=\_dst, __mem_ptr_type_dst_\@
  .ifc \_sidp,
  # get src inc or dec or displacement --------------------------------------
    __mem_ptr_sidp_src_\@   = -1
  .else
  # get src inc or dec or displacement --------------------------------------
    _get_symbol \_sidp, 0, -
    .if (__get_symbol_ret==0)
      __mem_ptr_sidp_src_\@   = __get_symbol_ret
    .endif
    _get_symbol \_sidp, 1, +
    .if (__get_symbol_ret==1)
      __mem_ptr_sidp_src_\@   = __get_symbol_ret
    .endif
    _get_symbol \_sidp, 2, d
    .if (__get_symbol_ret==2)
      __mem_ptr_sidp_src_\@   = __get_symbol_ret
    .endif
  .endif
  _prt_debug_msg __DEB_MCPP__, debug_input=\_dst, __mem_ptr_sidp_src_\@
  .ifc \_didp,
  # get dst inc or dec or displacement --------------------------------------
    __mem_ptr_didp_dst_\@   = -1
  .else
  # get dst inc or dec or displacement --------------------------------------
    _get_symbol \_didp, 0, -
    .if (__get_symbol_ret==0)
      __mem_ptr_didp_dst_\@   = __get_symbol_ret
    .endif
    _get_symbol \_didp, 1, +
    .if (__get_symbol_ret==1)
      __mem_ptr_didp_dst_\@   = __get_symbol_ret
    .endif
    _get_symbol \_didp, 2, d
    .if (__get_symbol_ret==2)
      __mem_ptr_didp_dst_\@   = __get_symbol_ret
    .endif
  .endif
  _prt_debug_msg __DEB_MCPP__, debug_input=\_dst, __mem_ptr_sidp_src_\@
  # chk list --------------------------------------------------------
  .if     (__mem_ptr_src_\@==__MEM_PTR_UNKNOWN)&&(__mem_ptr_dst_\@==__MEM_PTR_PTR)
  # RAM -> P --------------------------------------------------------
    .if     (__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_ptr_didp_dst_\@==-1)
    # GPR -> P --------------------------------------------------------
      ST \_dst, \_src
    .elseif (__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_ptr_didp_dst_\@== 0)
    # GPR -> -P+ --------------------------------------------------------
      ST \_didp\_dst, \_src
    .elseif (__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_ptr_didp_dst_\@== 1)
      ST \_dst\_didp, \_src
    .elseif (__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_ptr_didp_dst_\@== 2)
    # GPR -> P+dp --------------------------------------------------------
      .if (__mem_ptr_type_dst_\@ != __MEM_PTR_TYPE_PX)
        STD \_dst+\_ddp, \_src
      .elseif (__mem_ptr_type_dst_\@==__MEM_PTR_TYPE_PX)
        .error "STD cannot work with PTR X"
      .endif
    .elseif (__mem_type_src_\@>__MEM_TYPE_GPR)&&(__mem_type_src_\@<=__MEM_TYPE_ISRAM)
    # GPR+ -> P --------------------------------------------------------
      _mcp \_src, \_rtmp
      .if     (__mem_ptr_didp_dst_\@==-1)
      # GPR+ -> P --------------------------------------------------------
        ST \_dst, \_rtmp
      .elseif (__mem_ptr_didp_dst_\@== 0)
      # GPR+ -> -P+ --------------------------------------------------------
        ST \_didp\_dst, \_rtmp
      .elseif (__mem_ptr_didp_dst_\@== 1)
        ST \_dst\_didp, \_rtmp
      .elseif (__mem_ptr_didp_dst_\@== 2)
      # GPR+ -> P+dp --------------------------------------------------------
        .if (__mem_ptr_type_dst_\@ != __MEM_PTR_TYPE_PX)
          STD \_dst+\_ddp, \_rtmp
        .elseif (__mem_ptr_type_dst_\@==__MEM_PTR_TYPE_PX)
          .error "STD cannot work with PTR X"
        .endif
      .endif
    .endif
  .elseif (__mem_ptr_src_\@==__MEM_PTR_PTR)&&(__mem_ptr_dst_\@==__MEM_PTR_UNKNOWN)
  # P -> RAM --------------------------------------------------------
    .if    (__mem_ptr_sidp_src_\@==-1)&&(__mem_type_dst_\@==__MEM_TYPE_GPR)
    # P     -> GPR --------------------------------------------------------
      LD \_dst, \_src
    .elseif (__mem_ptr_sidp_src_\@==0)&&(__mem_type_dst_\@==__MEM_TYPE_GPR)
    # -P+   -> GPR --------------------------------------------------------
      LD \_dst, \_sidp\_src
    .elseif (__mem_ptr_sidp_src_\@==1)&&(__mem_type_dst_\@==__MEM_TYPE_GPR)
      LD \_dst, \_src\_sidp
    .elseif (__mem_ptr_sidp_src_\@==2)&&(__mem_type_dst_\@==__MEM_TYPE_GPR)
    # P+dp  -> GPR --------------------------------------------------------
      .if (__mem_ptr_type_src_\@ != __MEM_PTR_TYPE_PX)
        LDD \_dst, \_src+\_sdp
      .elseif (__mem_ptr_type_src_\@==__MEM_PTR_TYPE_PX)
        .error "STD cannot work with PTR X"
      .endif
    .elseif (__mem_type_dst_\@>__MEM_TYPE_GPR)&&(__mem_type_dst_\@<=__MEM_TYPE_ISRAM)
    # P     -> rtmp -> GPR+ -------------------------------------------------
      .if     (__mem_ptr_sidp_src_\@==-1)
      # P     -> rtmp -> GPR+ -----------------------------------------------
        LD \_rtmp, \_src
      # -P+   -> rtmp -> GPR+ -----------------------------------------------
      .elseif (__mem_ptr_sidp_src_\@== 0)
        LD \_rtmp, \_sidp\_src
      .elseif (__mem_ptr_sidp_src_\@== 1)
        LD \_rtmp, \_src\_sidp
      .elseif (__mem_ptr_sidp_src_\@== 2)
      # P+dp  -> rtmp -> GPR+ -----------------------------------------------
       .if (__mem_ptr_type_src_\@ != __MEM_PTR_TYPE_PX)
         LDD \_rtmp, \_src+\_sdp
       .elseif (__mem_ptr_type_src_\@==__MEM_PTR_TYPE_PX)
         .error "STD cannot work with PTR X"
       .endif
      .endif
      _mcp \_rtmp, \_dst
    .endif
  .elseif (__mem_ptr_src_\@==__MEM_PTR_PTR)&&(__mem_ptr_dst_\@==__MEM_PTR_PTR)
  # P -> _rtmp -> P ---------------------------------------------------------
    #  P   -> rtmp ->  P ----------------------------------------------------
    #  P   -> rtmp -> -P+ ---------------------------------------------------
    #  P   -> rtmp ->  P+dp -------------------------------------------------
    # -P+  -> rtmp ->  P ----------------------------------------------------
    # -P+  -> rtmp -> -P+ ---------------------------------------------------
    #  P+dp-> rtmp ->  P ----------------------------------------------------
    #  P+dp-> rtmp -> -P+ ---------------------------------------------------
    #  P+dp-> rtmp ->  P+dp -------------------------------------------------
    _mcpp \_src, \_sidp, \_sdp, \_rtmp, \_rtmp,
    _mcpp \_rtmp,      ,      , \_rtmp, \_dst , \_didp, \_ddp
  .else
  # other error
    .error "Other Error conditions"
  .endif
  _macro_cpp_e_\@:
.endm
/******************************************************************************/
.macro  _mwrip _k8:req, _rhtmp=r24, _ptr:req, _idp, _dp
# mem write immediate to [ptr]
  _macro_mwrip_s_\@:
  _mwri \_k8, \_rhtmp
  _mcpp \_rhtmp, , ,\_rhtmp,\_ptr,\_idp,\_dp
  _macro_mwrip_e_\@:
.endm
/******************************************************************************/
/* 
  bit transfer path
  k(0/1) -> SREG_T
  k(0/1) -> Byte-bit
  SREG_T <-> Byte-bit
  Byte-bit <-> Byte-bit
 */
#-------------------------------------------------------------------------------
_define_symbol_list __BIT_TYPE_, __DEB_BIT_TYPE__, UNKNOWN, K, T, BYTE
/*  __BIT_TYPE_UNKNOWN
    __BIT_TYPE_K
    __BIT_TYPE_T
    __BIT_TYPE_BYTE
 */
/******************************************************************************/
.macro  _bit_wr_gprh _gprh:req, _bit:req, _value:req
  _macro_bit_wr_gprh_s_\@:
  _get_bin \_value
    .if     (__binary_value==1)
      SBR   \_gprh, (1<<\_bit) /* ORI */
    .elseif (__binary_value==0)
      CBR   \_gprh, (1<<\_bit) /* ANDI */
    .endif
  _macro_bit_wr_gprh_e_\@:
.endm
.macro  _bit_wr_sfrl _sfrl:req, _bit:req, _value:req
  _macro_bit_wr_sfrl_s_\@:
  _get_bin \_value
    .if     (__binary_value==1)
      SBI   \_sfrl - __AVR_SFR_OFFSET, \_bit
    .elseif (__binary_value==0)
      CBI   \_sfrl - __AVR_SFR_OFFSET, \_bit
    .endif
  _macro_bit_wr_sfrl_e_\@:
.endm
.macro  _bit_wr_sreg _bit:req, _value:req
        _macro_bit_wr_sreg_s_\@:
        .if     (\_value==1)
            BSET \_bit
        .elseif (\_value==0)
            BCLR \_bit
        .endif
        _macro_bit_wr_sreg_e_\@:
.endm
/******************************************************************************/
.macro  _bcp _src:req, _src_bit, _dst:req, _dst_bit, _rhtmp=r24
# BIT copy from [K|T|RAM] to [T,RAM]
  _macro_bcp_s_\@:
  __bit_type_src_\@ = -1
  __bit_val_src_\@  = -1
  __bit_type_dst_\@ = -1
  __bit_val_dst_\@  = -1
  #chk src --------------------------------------------------------
  #check binary 0/1, only symbol accept, like true/false/set/clr/one/zero
  #save to __binary_value=0/1
  _get_bin \_src
  #check SREG_T, save to __get_symbol_ret, yes=1 or no=-1
  _get_symbol \_src, 1, t, T, sreg_t, SREG_T
  .if (__binary_value==0)
    __bit_type_src_\@ = __BIT_TYPE_K
    __bit_val_src_\@  = 0
    _prt_debug_msg __DEB_BCP__, input=\_src, __bit_type_src_\@
  .elseif (__binary_value==1)
    __bit_type_src_\@ = __BIT_TYPE_K
    __bit_val_src_\@  = 1
    _prt_debug_msg __DEB_BCP__, input=\_src, __bit_type_src_\@
  .elseif (__binary_value==-1)&&(__get_symbol_ret==1)
    __bit_type_src_\@ = __BIT_TYPE_T
    _prt_debug_msg __DEB_BCP__, input=\_src, __bit_type_src_\@
  .elseif (__binary_value==-1)&&(__get_symbol_ret==-1)
    __bit_type_src_\@ = __BIT_TYPE_BYTE
    _prt_debug_msg __DEB_BCP__, input=\_src, __bit_type_src_\@
  .else
    __bit_type_src_\@ = __BIT_TYPE_UNKNOWN
    _prt_debug_msg __DEB_BCP__, input=\_src, __bit_type_src_\@
  .endif
  #chk dst --------------------------------------------------------
  /* target not possible immediate data, never do _get_bin 
  _get_bin \_dst 
  _get_symbol \_dst, 1, t, T, sreg_t, SREG_T
  .if (__binary_value==0)
    __bit_type_dst_\@ = __BIT_TYPE_K
    __bit_val_dst_\@  = 0
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .elseif (__binary_value==1)
    __bit_type_dst_\@ = __BIT_TYPE_K
    __bit_val_dst_\@  = 1
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .elseif (__binary_value==-1)&&(__get_symbol_ret==1)
    __bit_type_dst_\@ = __BIT_TYPE_T
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .elseif (__binary_value==-1)&&(__get_symbol_ret==-1)
    __bit_type_dst_\@ = __BIT_TYPE_BYTE
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .else
    __bit_type_dst_\@ = __BIT_TYPE_UNKNOWN
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .endif
  */
  _get_symbol \_dst, 1, t, T, sreg_t, SREG_T
  .if     (__get_symbol_ret==1)
    __bit_type_dst_\@ = __BIT_TYPE_T
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .elseif (__get_symbol_ret==-1)
    __bit_type_dst_\@ = __BIT_TYPE_BYTE
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .else
    __bit_type_dst_\@ = __BIT_TYPE_UNKNOWN
    _prt_debug_msg __DEB_BCP__, input=\_dst, __bit_type_dst_\@
  .endif
  #chk list --------------------------------------------------------
  #k-1-0 SREG_T --------------------------------------------------------
  .if (__bit_type_src_\@==__BIT_TYPE_K)&&(__bit_type_dst_\@==__BIT_TYPE_T)
    .if (__bit_val_src_\@==1)
        BSET SREG_T
    .elseif (__bit_val_src_\@==0)
        BCLR SREG_T
    .endif
  #k-1-0 BYTE-bit --------------------------------------------------------
  .elseif(__bit_type_src_\@==__BIT_TYPE_K)&&(__bit_type_dst_\@==__BIT_TYPE_BYTE)
    _get_op \_dst
    __mem_type_dst_\@ = __mem_type
    __mem_part_dst_\@ = __mem_part
    __mem_sreg_dst_\@ = __mem_sreg
    .if     (__mem_part_dst_\@==__MEM_PART_GPRL)
      _mcp \_dst,   \_rhtmp
      _bit_wr_gprh  \_rhtmp, \_dst_bit, %__bit_val_src_\@
      _mcp \_rhtmp, \_dst
      .exitm
    .elseif (__mem_part_dst_\@==__MEM_PART_GPRH)
      _bit_wr_gprh  \_dst,  \_dst_bit, %__bit_val_src_\@
      .exitm
    .elseif (__mem_part_dst_\@==__MEM_PART_SFRL)
      _bit_wr_sfrl  \_dst,  \_dst_bit, %__bit_val_src_\@
      .exitm
    .elseif (__mem_part_dst_\@==__MEM_PART_SFRH)||(__mem_type_dst_\@==__MEM_TYPE_ISRAM)
      .if   (__mem_sreg_dst_\@==__MEM_SREG_SREG)
        _bit_wr_sreg \_dst_bit, %__bit_val_src_\@
        .exitm
      .endif
      _mcp \_dst,   \_rhtmp
      _bit_wr_gprh  \_rhtmp, \_dst_bit, %__bit_val_src_\@
      _mcp \_rhtmp, \_dst
      .exitm
    /*
    .elseif (__mem_type_dst_\@>=__MEM_TYPE_ESFR)||(__mem_type_dst_\@<=__MEM_TYPE_ESRAM)
      _mcp \_dst,   \_rhtmp
      _bit_wr_gprh  \_rhtmp, \_dst_bit, %__bit_val_src_\@
      _mcp \_rhtmp, \_dst
      .exitm
    */
    .endif
  # T->BYTE-bit --------------------------------------------------------
  .elseif (__bit_type_src_\@==__BIT_TYPE_T)&&(__bit_type_dst_\@==__BIT_TYPE_BYTE)
    _get_op \_dst
    .if     (__mem_type==__MEM_TYPE_GPR)
      BLD   \_dst,  \_dst_bit
      .exitm
   .elseif (__mem_type > __MEM_TYPE_GPR)&&(__mem_type <= __MEM_TYPE_ESRAM)
      _mcp  \_dst,    \_rhtmp
      BLD   \_rhtmp,  \_dst_bit
      _mcp  \_rhtmp,  \_dst
      .exitm
    .endif
  # BYTE-bit -> T --------------------------------------------------------
  .elseif (__bit_type_src_\@==__BIT_TYPE_BYTE)&&(__bit_type_dst_\@==__BIT_TYPE_T)
    _get_op \_src
    .if     (__mem_type==__MEM_TYPE_GPR)
      BST   \_src,  \_src_bit
      .exitm
   .elseif (__mem_type > __MEM_TYPE_GPR)&&(__mem_type <= __MEM_TYPE_ESRAM)
      _mcp  \_src,    \_rhtmp
      BST   \_rhtmp,  \_src_bit
      .exitm
    .endif
  # BYTE-bit -> BYTE-bit ------------------------------------------------------
  .elseif (__bit_type_src_\@==__BIT_TYPE_BYTE)&&(__bit_type_dst_\@==__BIT_TYPE_BYTE)
    _bcp \_src  , \_src_bit , SREG_T  ,           ,
    _bcp SREG_T ,           , \_dst   , \_dst_bit , 
  # others --------------------------------------------------------
  .else
    #others
  .endif
  _macro_bcp_e_\@:
.endm
/******************************************************************************/
.macro  _bwrit _k1:req
# write true/false to SREG_T
        _macro_bwrit_s_\@:
        _bcp \_k1, , SREG_T
        _macro_bwrit_e_\@:
.endm
.macro  _bwrir _k1:req, _dst:req, _bit:req, _rhtmp
# write true/false to RAM-bit [rhtmp]
        _macro_bwrir_s_\@:
        _bcp \_k1, , \_dst, \_bit, \_rhtmp
        _macro_bwrir_e_\@:
.endm
.macro  _bcptr _dst:req, _bit:req, _rhtmp
# Copy SREG_T to RAM-bit [rhtmp]
        _macro_bcptr_s_\@:
        _bcp SREG_T, , \_dst, \_bit, \_rhtmp
        _macro_bcptr_e_\@:
.endm
.macro  _bcprt _src:req, _bit:req, _rhtmp
# Copy RAM-bit to SREG_T [rhtmp]
        _macro_bcprt_s_\@:
        _bcp \_src, \_bit, SREG_T, , \_rhtmp
        _macro_bcprt_e_\@:
.endm
.macro  _bcprr _src:req, _src_bit:req, _dst:req, _dst_bit:req, _rhtmp
# Copy RAM-bit to RAM-bit [rhtmp]
        _macro_bcprr_s_\@:
        _bcp \_src, \_src_bit, \_dst, \_dst_bit, \_rhtmp
        _macro_bcprr_e_\@:
.endm
/******************************************************************************/
.macro  _nvmcp
.endm
.macro  _nvmcpp
.endm
/******************************************************************************/
.macro _pmrd_gpr _dst, _inc
# program mem read to _dst[r0/gpr] from ptr[Z] with option _inc
  _macro_pmrd_gpr_s_\@:
  .ifc \_dst,
    .ifc \_inc,
    # read into R0
      LPM
    .endif
  .endif
  .ifnc \_dst,
    .ifc \_inc,
      LPM \_dst, Z
    .endif
    .ifc \_inc,+
      LPM \_dst, Z+
    .endif
  .endif
  _macro_pmrd_gpr_e_\@:
.endm
#-------------------------------------------------------------------------------
.macro _pmrd _dst, _inc, _rtmp=r24
# copy program mem to _dst[any data mem] via Z option _inc[Z+]
  _macro_pmrd_s_\@:
  .ifc \_dst,
    .ifc \_inc,
      # read into R0
      _pmrd_gpr
    .endif
    .exitm
  .endif
  .ifnc \_dst,
    _get_op \_dst
    .if ((__mem_addr==30) || (__mem_addr==31))
      .warning "cannot use _dst=R30:R31 as DST = _dst"
      .error "cannot use _dst=R30:R31 as DST = _dst"
    .elseif ((__mem_addr >= 0) && (__mem_addr < 30))
      _pmrd_gpr \_dst, \_inc
    .elseif ((__mem_type >= __MEM_TYPE_SFR)&&(__mem_type <= __MEM_TYPE_ISRAM))
      _pmrd_gpr \_rtmp, \_inc
      _mcp \_rtmp, \_dst
    .else
      .print "ERR: _dst out of ESRAM"
    .endif
  .endif
  _macro_pmrd_e_\@:
.endm
/******************************************************************************/
.macro  _mwri16 src_k16:req, dst:req, rhtmp=r24
# not work 
        _mwri lo8(\src_k16),\dst
        _mwri hi8(\src_k16),\dst+1
.endm
.macro  _mwri24 src_k24:req, dst_lo8:req, dst_hi8:req, dst_hh8:req
# not work 
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
/******************************************************************************/
/*
  copy n x bits(mask=1) from src to dst, src not change
  src/dst = _GPRH, no rhtmp req
  src/dst have 1 x _GPRH, req 1 x rhtmp
  src/dst no _GPRH, req 2 x rhtmp
  src/dst no GPR, req rhtmp3
                        // msk = 0b0011_0000
  _mcp \dst,  \rhtmp    // dst = 0b0001_1000
  CBR  \rhtmp,\mask     // dst = 0b0000_1000
                        // src = 0bxx10_xxxx
  ANDI \src,  \mask     // src = 0b0010_0000
  OR   \rhtmp,\src      // dst = 0b0010_1000
  _mcp \rhtmp,\dst
*/
/*----------------------------------------------------------------------------*/
.macro  _bcp_msk src:req, dst:req, msk:req, rhtmp1=r24, rhtmp2=r25, rhtmp3=r26
  _bcp_msk_s_\@:
  _get_op \src
  __mem_type_src_\@ = __mem_type
  __mem_part_src_\@ = __mem_part
  _get_op \dst
  __mem_type_dst_\@ = __mem_type
  __mem_part_dst_\@ = __mem_part
  .if (__mem_part_dst_\@==__MEM_PART_GPRH)
      CBR  \dst, \msk
      _mcp \src, \rhtmp1, \rhtmp3
      ANDI \rhtmp1, \msk
      OR   \dst, \rhtmp1
      .exitm
  .else
      _mcp \dst,    \rhtmp1, \rhtmp3
      CBR  \rhtmp1, \msk
      _mcp \src,    \rhtmp2, \rhtmp3
      ANDI \rhtmp2, \msk
      OR   \rhtmp1, \rhtmp2
      _mcp \rhtmp1, \dst,    \rhtmp3
      .exitm
    .endif
  _bcp_msk_e_\@:
.endm
/******************************************************************************/
/* vim: ft=gas :
 */
