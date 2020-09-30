#.include "m328p_def.inc"
.equ __DEB_UTILS__, 1
/******************************************************************************/
.altmacro
/*****************************************************************************\
  String
\*****************************************************************************/
.macro  _cmd_join2args cmd, arg1, arg2
        \cmd \arg1\arg2
.endm
#_cmd_join2args .print, "a"
#
.macro  _cmd_join3args cmd, arg1, arg2, arg3
        \cmd \arg1\arg2\arg3
.endm
#_cmd_join3args .print, "a"
#
.macro  _cmd_join2args_wcomma cmd, arg1, arg2
        \cmd \arg1, ?, \arg2
.endm
# not work
#_cmd_join2args_wcomma ldi, r24, 0
#
.macro  _prt_num num:req
        .print "num"
.endm
.macro  _prt_num_tb
        __tst_num = -1
        #__tst_num = 'a'
        _prt_num %__tst_num
.endm
#_prt_num_tb
#
.macro  _prt_sym2 _sym1, _sym2
#        .print "\_sym1\_sym2"
.endm
/******************************************************************************/
.macro  _str_gen strlist:vararg
        __str_gen_ret = \strlist
.endm
#.print "ABCC\bD"
#.print "0,1,2,three"
#_str_gen "0,1,2"
#
.macro  _str_concat str1, str2
        \str1\str2
        __str_concat_ret = \str1\str2
.endm
#_str_concat a, b
.macro  _strc str1, str2
        \str1\str2
        __str_concat_ret = \str1\str2
.endm
#
.macro  _strc_vararg strlist:vararg
        #__strc_vararg  = 0
        ##define __strc_vararg  r1
        #inc __strc_vararg
        #.irp str strlist
        #    .print "strc"
        #    inc _strc \str, %__strc_vararg
        #.endr
.endm
.macro  _str_len strlist:vararg
        __str_len_ret = 0
        .ifnc strlist,
            #_str_gen \strlist
            #.irp str __str_gen_ret
            #        __str_len_ret = __str_len_ret +1
            #.endr
            .irp str strlist
                    __str_len_ret = __str_len_ret +1
            .endr
            .print "string lenth ="
            _prt_num %__str_len_ret
        .endif
        .ifc strlist,
            .if  (__str_gen_ret=="")
                .print "string empty"
                .exitm
            .endif
            .ifnc  __str_gen_ret,
                .print "string= __str_gen_ret \t lenth ="
                .irp s "__str_gen_ret"
                        __str_len_ret = __str_len_ret +1
                .endr
                _prt_num %__str_len_ret
            .endif
        .endif
.endm
#_str_len
#_str_len a
#_str_len a,b
#_str_gen ""
#_str_len
#_str_gen a,b,c
#_str_len
#
.equ __DEB_STR_INDEX__,1
.macro  _str_index _str:req, _str_list:vararg
        __str_index_ret = -1
        __str_index_id = -1
        #_str_len \_str_list
        .irp _s \_str_list
            __str_index_id  = __str_index_id + 1
            .ifc _str,_s
            #_prt_debug_msg
                #.print "_str_index _str"
                #_prt_num %__str_index_id
                __str_index_ret = 1
                .exitm
            .endif
        .endr
.endm
#_str_gen "0,1,2,three"
#_str_index 0, %__str_gen_ret
#_str_index three, 1,2 three, 4
#
.macro  _str_srch str:req, strlist:vararg
        __str_srch_ret = -1
        .irp s \strlist
            .ifc \str,s
                __str_srch_ret = 1
                .print "INPUT=\"str\" found in string list=\"strlist\""
                .exitm
            .endif
        .endr
        .if (__str_srch_ret==-1)
            .print "INPUT=\"str\" not found in string list=\"strlist\""
        .endif
.endm
#_str_srch a, a,b,c,d
#_sstr a, b,c,d,e,f
.macro  _str_split strlist:vararg
        __str_split_cnt = 0
        _str_len \strlist
        #_prt_num %__str_len_ret
        .irp str \strlist
            #__str_split%__str_split_cnt = str
            #__str_split_0 = str
            _str_concat __str_split_, %__str_split_cnt
            #_prt_num __str_split_0
            .print "__str_concat_ret"
            #= str
            .print "str1 str __str_split_0"
            __str_split_cnt = __str_split_cnt +1
            #_prt_num %__str_split_cnt
        .endr
.endm
# not work
#_str_split a,b,c
#

.macro  _gen_jp_ ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        \prefix\ifcnt\delim\nest\delim\macro:
        .print "prefix+ifcnt+delim+nest+delim+macro:"
.endm
.macro  _gen_jp1a arg1
        \arg1:
.endm
.macro  _gen_jp2a arg1,arg2
        \arg1\arg2:
.endm
.macro  _gen_jp3a arg1,arg2,arg3
        \arg1\arg2\arg3:
.endm
.macro  _gen_jp4a arg1,arg2,arg3,arg4
        \arg1\arg2\arg3\arg4:
.endm
.macro  _gen_jp5a arg1,arg2,arg3,arg4,arg5
        \arg1\arg2\arg3\arg4\arg5:
.endm
.macro  _gen_jp6a arg1,arg2,arg3,arg4,arg5,arg6
        \arg1\arg2\arg3\arg4\arg5\arg6:
.endm
.macro  _gen_jp7a arg1,arg2,arg3,arg4,arg5,arg6,arg7
        \arg1\arg2\arg3\arg4\arg5\arg6\arg7:
.endm
.macro  _gen_jp8a arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8
        \arg1\arg2\arg3\arg4\arg5\arg6\arg7\arg8:
.endm
.macro  _gen_jp9a arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg9
        \arg1\arg2\arg3\arg4\arg5\arg6\arg7\arg8\arg9:
.endm

/******************************************************************************/
.macro  _prt_b  a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16
        .print "a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 a16"
.endm
.macro  _prt_d  a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16
        .print "a1-a2-a3-a4-a5-a6-a7-a8-a9-a10-a11-a12-a13-a14-a15-a16"
.endm
.macro  _strc_12b3 s1,s2,s3
        \s1\s2  \s3
.endm
.macro  _strc_c12bc16xarg s1,s2, a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16
        \s1\s2  a1\a2\a3\a4\a5\a6\a7\a8\a9\a10\a11\a12\a13\a14\a15\a16
.endm
.macro  _strc_12b3b4 s1,s2,s3,s4
        \s1\s2  \s3 \s4
.endm
.macro  _strc_c1b2b34 s1,s2,s3,s4
        \s1 \s2 \s3\s4
.endm

.macro  _strc_1b23 s1,s2,s3
        \s1 \s2\s3
.endm
.macro  _strc_1b2c3 s1, s2, s3
        \s1 \s2 , \s3
.endm
.macro  _strc_1comma2  s1, s2
        \s1 , \s2
.endm
.macro  _strc_1b23c4 s1,s2,s3,s4
        \s1 \s2\s3 , \s4
.endm
.macro  _strc_1b2b3 s1,s2,s3,s4
        \s1 \s2 \s3 \s4
.endm
/*****************************************************************************\
  Software Stack
\*****************************************************************************/
.equ __GAS_STACK_SIZE__,  100
.equ __gas_stack_ptr,      0
.equ __gas_stack_value,    -1
.macro  __gas_stack_init
        __gas_stack_ptr  =  __GAS_STACK_SIZE__
        .rept __GAS_STACK_SIZE__
            #.set __gas_stack_val_p %__gas_stack_ptr,    -1
            _strc_12b3 __gas_stack_val_p, %__gas_stack_ptr, =-1
            #_prt_num %__gas_stack_ptr
            __gas_stack_ptr = __gas_stack_ptr - 1
        .endr
.endm
.macro  __gas_stack_push val_push
        #.print "push b4 __gas_stack_ptr"
        #_prt_num %__gas_stack_ptr
        __gas_stack_ptr   = __gas_stack_ptr  +1
        #.print "push af __gas_stack_ptr"
        #_prt_num %__gas_stack_ptr
        __gas_stack_value = \val_push
        _strc_12b3b4 __gas_stack_val_p, %__gas_stack_ptr, =, \val_push
        #.print "push __gas_stack_push val_push"
.endm
.macro  __gas_stack_pop
        __gas_stack_ptr  = __gas_stack_ptr  -1
        .if (__gas_stack_ptr > 0)
            #.print "pop  b4 __gas_stack_ptr"
            #_prt_num %__gas_stack_ptr
            #.print "pop  af __gas_stack_ptr"
            #_prt_num %__gas_stack_ptr
            _strc_c1b2b34 __gas_stack_value, =, __gas_stack_val_p, %__gas_stack_ptr
            #.print "pop  __gas_stack_pop"
            #_prt_num %__gas_stack_value
        .elseif (__gas_stack_ptr <= 0)
            #__gas_stack_value = -1
            .print "pop  stack empty"
        .endif
.endm
/*****************************************************************************/
.macro  _if_sym_eq_val _sym, _val
        .if (\_sym==\_val)
          .print "_sym"
        .endif
.endm
.macro _prt_val _sym:req
    __rpt_cnt = -100
    .rept 200
        _if_sym_eq_val %_sym, %__rpt_cnt
        __rpt_cnt = __rpt_cnt + 1
    .endr
.endm
#.set __abc, 1
#_prt_val __abc
#
.macro  _prt_debug_msg _deb_sym, _msg, _value
                      #debug symbol, msg, val
  _macro_prt_debug_msg_s_\@:
        .ifdef \_deb_sym
          .print "_prt_debug_msg: \_msg, \_value ="
          #_prt_sym2 \_m1, \_m2, \_m3
          _prt_val \_value
        .endif
  _macro_prt_debug_msg_e_\@:
.endm
/******************************************************************************/
.macro  _define_symbol_list _prefix, _debug_symbol, _list:vararg
  _macro_define_symbol_list_start_\@:
        __symbol_cnt = -1
        .irp  __symbol_name \_list
              _strc_1b23c4 .equiv, \_prefix, \__symbol_name, __symbol_cnt
              _prt_debug_msg \_debug_symbol, _prefix\__symbol_name, __symbol_cnt
              __symbol_cnt = __symbol_cnt + 1
        .endr
  _macro_define_symbol_list_end_\@:
.endm
/******************************************************************************/
#.set __DEB_MEM_TYPE__, 1
_define_symbol_list __MEM_TYPE_, __DEB_MEM_TYPE__, UNKNOWN, GPR, SFR, ESFR, ISRAM, ESRAM, FLASH, EEPROM, FUSE
/*
__MEM_TYPE_UNKNOWN
__MEM_TYPE_GPR
__MEM_TYPE_SFR
__MEM_TYPE_ESFR
__MEM_TYPE_ISRAM
__MEM_TYPE_ESRAM
__MEM_TYPE_FLASH
__MEM_TYPE_EEPROM
__MEM_TYPE_FUSE
 */
_define_symbol_list __MEM_PART_, __DEB_MEM_PART__, UNKNOWN, GPRL, GPRH, SFRL, SFRH
/*
__MEM_PART_UNKNOWN
__MEM_PART_GPRL
__MEM_PART_GPRH
__MEM_PART_SFRL
__MEM_PART_SFRH
 */
_define_symbol_list __MEM_PTR_, __DEB_MEM_SREG__, UNKNOWN, PTR
/*
__MEM_PTR_UNKNOWN
__MEM_PTR_PTR
 */
_define_symbol_list __MEM_PTR_TYPE_, __DEB_PTR_TYPE__, UNKNOWN, PX, PY, PZ
/*
__MEM_PTR_TYPE_UNKNOWN
__MEM_PTR_TYPE_PX
__MEM_PTR_TYPE_PY
__MEM_PTR_TYPE_PZ
 */
_define_symbol_list __MEM_PTR_PART_, __DEB_PTR_PART__, UNKNOWN, PXL, PXH, PYL, PYH, PZL, PZH
/*
__MEM_PTR_PART_UNKNOWN
__MEM_PTR_PART_PXL
__MEM_PTR_PART_PXH
__MEM_PTR_PART_PYL
__MEM_PTR_PART_PYH
__MEM_PTR_PART_PZL
__MEM_PTR_PART_PZH
 */
_define_symbol_list __MEM_SREG_, __DEB_MEM_SREG__, UNKNOWN, SREG
/*
__MEM_SREG_UNKNOWN
__MEM_SREG_SREG
 */
/*****************************************************************************\
  SRAM address check
\*****************************************************************************/
#.set    __DEB_GET_OP__, 1
/******************************************************************************/
.macro  _get_op _op:req
        #get operand info
  _macro_get_op_s_\@:
  __mem_type = -1
  __mem_part = -1
  __mem_addr = -1
  __mem_ptr  = -1
  __ptr_type = -1
  __ptr_part = -1
  __mem_sreg = -1
  #--------------------------------------------------------
  _str_index \_op, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15
  .if (__str_index_ret==1)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRL
    __mem_addr = (__str_index_id % 16)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, R16, R17, R18, R19, R20, R21, R22, R23, R24, R25
  .if (__str_index_ret==1)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = ((__str_index_id % 10) + 16)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 26, r26, R26, xl, xL, Xl, XL
  .if (__str_index_ret==1)||(\_op==26)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 26
    __mem_ptr_type = __MEM_PTR_TYPE_PX
    __mem_ptr_part = __MEM_PTR_PART_PXL
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 27, r27, R27, xh, xH, Xh, XH
  .if (__str_index_ret==1)||(\_op==27)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 27
    __mem_ptr_type = __MEM_PTR_TYPE_PX
    __mem_ptr_part = __MEM_PTR_PART_PXH
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 28, r28, R28, yl, yL, Yl, YL
  .if (__str_index_ret==1)||(\_op==28)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 28
    __mem_ptr_type = __MEM_PTR_TYPE_PY
    __mem_ptr_part = __MEM_PTR_PART_PYL
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 29, r29, R29, yh, yH, Yh, YH
  .if (__str_index_ret==1)||(\_op==29)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 29
    __mem_ptr_type = __MEM_PTR_TYPE_PY
    __mem_ptr_part = __MEM_PTR_PART_PYH
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 30, r30, R30, zl, zL, Zl, ZL
  .if (__str_index_ret==1)||(\_op==30)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 30
    __mem_ptr_type = __MEM_PTR_TYPE_PZ
    __mem_ptr_part = __MEM_PTR_PART_PZL
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #--------------------------------------------------------
  _str_index \_op, 31, r31, R31, zh, zH, Zh, ZH
  .if (__str_index_ret==1)||(\_op==31)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 31
    __mem_ptr_type = __MEM_PTR_TYPE_PZ
    __mem_ptr_part = __MEM_PTR_PART_PZH
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  # PTR X --------------------------------------------------------
  _get_symbol \_op, 0, x, X
    .if (__get_symbol_ret==0)
      __mem_ptr       = __MEM_PTR_PTR
      __mem_ptr_type  = __MEM_PTR_TYPE_PX
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
      .exitm
    .endif
  # PTR Y --------------------------------------------------------
  _get_symbol \_op, 1, y, Y
    .if (__get_symbol_ret==1)
      __mem_ptr       = __MEM_PTR_PTR
      __mem_ptr_type  = __MEM_PTR_TYPE_PY
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
      .exitm
    .endif
  # PTR Z --------------------------------------------------------
  _get_symbol \_op, 2, z, Z
    .if (__get_symbol_ret==2)
      __mem_ptr       = __MEM_PTR_PTR
      __mem_ptr_type  = __MEM_PTR_TYPE_PZ
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
      .exitm
    .endif
  #end search string start search value -------------------
  #GPRL [0,15][0x00,0x0F] -----------------------------------------------------
  .if (\_op >= 0x00)&&(\_op <= 0x0F)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRL
    __mem_addr = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    .exitm
  .endif
  #GPRH [16,25][0x10,0x19] ----------------------------------------------------
  .if (\_op >= 0x10)&&(\_op <= 0x19)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    .exitm
  .endif
  #GPRH xyz --------------------------------------------------------
  #GPRH xl 26/0x1A --------------------------------------------------------
  .if (\_op==0x1A)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 0x1A
    __mem_ptr_type = __MEM_PTR_TYPE_PX
    __mem_ptr_part = __MEM_PTR_PART_PXL
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #GPRH xh 27/0x1B --------------------------------------------------------
  .if (\_op==0x1B)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 0x1B
    __mem_ptr_type = __MEM_PTR_TYPE_PX
    __mem_ptr_part = __MEM_PTR_PART_PXH
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #GPRH yl 28/0x1C --------------------------------------------------------
  .if (\_op==0x1C)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 0x1C
    __mem_ptr_type = __MEM_PTR_TYPE_P
    __mem_ptr_part = __MEM_PTR_PART_PYL
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #GPRH yh 29/0x1D --------------------------------------------------------
  .if (\_op==0x1D)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 0x1D
    __mem_ptr_type = __MEM_PTR_TYPE_P
    __mem_ptr_part = __MEM_PTR_PART_PYH
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #GPRH zl 30/0x1E --------------------------------------------------------
  .if (\_op==0x1E)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 0x1E
    __mem_ptr_type = __MEM_PTR_TYPE_P
    __mem_ptr_part = __MEM_PTR_PART_PZL
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #GPRH zh 31/0x1F --------------------------------------------------------
  .if (\_op==0x1F)
    __mem_type = __MEM_TYPE_GPR
    __mem_part = __MEM_PART_GPRH
    __mem_addr = 0x1F
    __mem_ptr_type = __MEM_PTR_TYPE_P
    __mem_ptr_part = __MEM_PTR_PART_PZH
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_ptr_part
    .exitm
  .endif
  #SFRL [0x20,0x3F] --------------------------------------------------------
  .if ((\_op > 0x1F) && (\_op <= 0x3F))
    __mem_type  = __MEM_TYPE_SFR
    __mem_part  = __MEM_PART_SFRL
    __mem_addr  = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
   .exitm
  .endif
  #SFRH [0x40,0x5F] --------------------------------------------------------
  .if ((\_op > 0x3F) && (\_op <= 0x5F))
    __mem_type  = __MEM_TYPE_SFR
    __mem_part  = __MEM_PART_SFRH
    __mem_addr  = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
    .if (\_op==0x5F)
      __mem_sreg = __MEM_SREG_SREG
      _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_sreg
    .endif
    .exitm
  .endif
  #--------------------------------------------------------
  .if ((\_op > 0x5F) && (\_op <= IOEND)) 
  #IOEND define in device_def.inc
    __mem_type  = __MEM_TYPE_ESFR
    #__mem_part  = __MEM_PART_ESFR
    __mem_addr  = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
   .exitm
  .endif
  #--------------------------------------------------------
  .if ((\_op >= SRAM_START) && (\_op <= RAMEND)) 
  #SRAM_START/RAMEND define in device_def.inc
    __mem_type  = __MEM_TYPE_ISRAM
    #__mem_part  = __MEM_PART_ISRAM
    __mem_addr  = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    #_prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
   .exitm
  .endif
  #--------------------------------------------------------
  .if ((\_op > RAMEND)&&(\_op <=0x3FFFFF)) 
    #RAMEND define in device_def.inc
    #External SRAM, P16 = (RAMEND,0xFFFF] 64k words
    #External SRAM, P22 = (RAMEND,0x3F FFFF] 4M words
    __mem_type  = __MEM_TYPE_ESRAM
    #__mem_part  = __MEM_PART_ISRAM
    __mem_addr  = (\_op)
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_type
    #_prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_part
    _prt_debug_msg __DEB_GET_OP__, input = \_op, __mem_addr
   .exitm
  .endif
  #--------------------------------------------------------
  .if (\_op > 0x3FFFFF) 
  #RAMEND define in device_def.inc
    .error "memory address out of range"
    .exitm
  .endif
  #--------------------------------------------------------
  _macro_get_op_e_\@:
.endm
/******************************************************************************/
#.set __DEB_GET_SYMBOL__, 1
.macro  _get_symbol _input:req, _val:req, _sym_list:vararg
/* check _input if exist in _sym_list, return _val, else return -1 */
  _macro_get_symbol_s_\@:
    __get_symbol_ret = -1
      .irp _sym \_sym_list
        .ifc \_input,\_sym
          __get_symbol_ret = \_val
          #_prt_debug_msg __DEB_GET_SYMBOL__, _get_symbol->input=\_input, _val
          .exitm
        .endif
      .endr
  _macro_get_symbol_e_\@:
.endm
#.set __DEB_GET_BIN__, 1
.macro  _get_bin _input
/* check _input string match with 0/1 value str list, return 0/1/-1 */
  _macro_get_bin_s_\@:
  __binary_value = -1
  .irp __one TRUE, True, true, YES, Yes, yes, SET, Set, set, one, One, ONE, on, On, ON
    .ifc \_input,__one
      __binary_value = 1
      _prt_debug_msg __DEB_GET_BIN__, _get_bin->input=\_input, __binary_value
      .exitm
    .endif
  .endr
  .irp __zero FALSE, False, false, NO, No, no, CLEAR, Clear, clear, CLR, Clr, clr, zero, Zero, ZERO, off, Off, OFF
    .ifc \_input,__zero
      __binary_value = 0
      _prt_debug_msg __DEB_GET_BIN__, _get_bin->input=\_input, __binary_value
      .exitm
    .endif
  .endr
  .if     (\_input==0)
    __binary_value = 0
  .elseif (\_input==1)
    __binary_value = 1
  .endif
  .if (__binary_value==-1)
    _prt_debug_msg __DEB_GET_BIN__, _get_bin->input=\_input, __binary_value
    /* .print "_input not valid binary 1 or 0 symbol" */
    /* .error "ERR: _input not 0 or 1 or symbol" */
  .endif
  _macro_get_bin_e_\@:
.endm
/******************************************************************************/
.macro  _mem_opcode_1x_addr _opcode:req, _dst:req, _rtmp=r24
/* do _opcode GPR/SRAM */
  _macro_mem_opcode_1x_addr_start_\@:
  __mem_type_dst_\@ = -1
  __mem_part_dst_\@ = -1
  __mem_addr_dst_\@ = -1
  _get_op \_dst
    __mem_type_dst_\@ = __mem_type
    __mem_part_dst_\@ = __mem_part
    __mem_addr_dst_\@ = __mem_addr
  .if (__mem_type_dst_\@==__MEM_TYPE_GPR)
      \_opcode \_dst
      .exitm
  .endif
  .if (__mem_type_dst_\@ >= __MEM_TYPE_SFR)
      _mcp \_dst,   \_rtmp
      \_opcode  \_rtmp
      _mcp \_rtmp, \_dst
      .exitm
  .endif
  _macro_mem_opcode_1x_addr_end_\@:
.endm

.macro  _mem_opcode_2x_addr _opcode:req, _src:req, _dst:req, _rtmp1=r24, _rtmp2=r25
/* do _opcode with 2 x GPR, any memory space, include gpr/sfr/sram */
  _macro_mem_opcode_2x_addr_start_\@:
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
  .if ((__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_type_dst_\@==__MEM_TYPE_GPR))
      \_opcode \_dst, \_src
      .exitm
  .endif
  .if ((__mem_type_src_\@==__MEM_TYPE_GPR)&&(__mem_type_dst_\@ >= __MEM_TYPE_SFR))
      _mcp \_dst,  \_rtmp1
      \_opcode  \_rtmp1, \_src
      _mcp \_rtmp1, \_dst
      .exitm
  .endif
  .if ((__mem_type_dst_\@==__MEM_TYPE_GPR)&&(__mem_type_src_\@ >= __MEM_TYPE_SFR))
      _mcp \_src,  \_rtmp1
      \_opcode  \_dst,  \_rtmp1
      .exitm
  .endif
  .if ((__mem_type_src_\@ >= __MEM_TYPE_SFR)&&(__mem_type_dst_\@ >= __MEM_TYPE_SFR))
      _mcp \_src,  \_rtmp1
      _mcp \_dst,  \_rtmp2
      \_opcode  \_rtmp2, \_rtmp1
      _mcp \_rtmp2, \_dst
      .exitm
  .endif
  _macro_mem_opcode_2x_addr_end_\@:
.endm
/******************************************************************************/
.macro  _mem_xaddr_set _xaddr:req, _value:req
        _macro_mem_xaddr_set_s_\@:
        _mwri \_value, \_xaddr
        _macro_mem_xaddr_set_e_\@:
.endm
.macro  _mem_xaddr_get _xaddr:req, _ret_xaddr:req
        _macro_mem_xaddr_get_s_\@:
        _mcp \_xaddr, \_ret_xaddr
        _macro_mem_xaddr_get_e_\@:
.endm
.macro  _mem_xaddr_toggle _xaddr:req
        _macro_mem_xaddr_toggle_s_\@:
        _not \_xaddr
        _macro_mem_xaddr_toggle_e_\@:
.endm
.macro  _mem_xaddr_bit_set_on _xaddr:req, _bitx:req
        _macro_mem_xaddr_bit_set_on_s_\@:
        _bwrir 1, \_xaddr, \_bitx
        _macro_mem_xaddr_bit_set_on_e_\@:
.endm
.macro  _mem_xaddr_bit_set_off _xaddr:req, _bitx:req
        _macro_mem_xaddr_bit_set_off_s_\@:
        _bwrir 0, \_xaddr, \_bitx
        _macro_mem_xaddr_bit_set_off_e_\@:
.endm
.macro  _mem_xaddr_bit_wr _xaddr:req, _bit_val:req
        _macro_mem_xaddr_bit_wr_s_\@:
        _get_bin _bit_val
        .if     (__binary_value == 0)
          _bwrir 0, \_xaddr, \_bitx
        .elseif (__binary_value == 1)
          _bwrir 1, \_xaddr, \_bitx
        .endif
        _macro_mem_xaddr_bit_wr_e_\@:
.endm
.macro  _mem_xaddr_bit_toggle _xaddr:req, _bitx:req
        _macro_mem_xaddr_bit_toggle_s_\@:
        _bit_not \_xaddr, \_bitx
        _macro_mem_xaddr_bit_toggle_e_\@:
.endm
.macro  _mem_xaddr_bit_get _xaddr:req, _bitx:req
        _macro_mem_xaddr_bit_get_s_\@:
        _bcprt \_xaddr, \_bitx
        _macro_mem_xaddr_bit_get_e_\@:
.endm
/******************************************************************************/
/* vim: ft=gas :
 */
