.set __DEB_IF_BIT__,0
.set __DEB_IFB_TRUE_SKIP__,0
.set __DEB_IF__,1
.set __AVR_SFR_OFFSET, 0x20
.altmacro
/******************************************************************************\
  _gen_label  ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
  _gen_cjmp xjmp:req, ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
  _gen_ujmp xjmp=rjmp, ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
  _if_bit xaddr:req, bit:req, set_or_clr=set, jp_false, rtmp=r24
  _ifb_true_skip xaddr:req, bit:req, set_or_clr=set, rtmp=r24
  _if_cond cond:req, jp1:req, jp2, jp3, jp4, jp5, jp6, jp7, jp8, jp9
  _if cmd, cond, jp=Eif
  _then
  _Ethen jp
  _else
  _Eelse
  _elif cmd, cond, jp
  _Eelif jp
  _Eif jp
  _jtb_gen_tbl case_count:req
  _jtb_case_do id:req
  _jtb_case_end id:req
  _jtb_end
  _jtb_z_ld_addr prefix=__jtb_, id, suffix=_caseid_0
  _jtb_switch_if true_id, false_id, rtmp=r24, cond:req, _cmds:vararg
  _jtb_switch reg
  _jtb_gen_batch_cases cmds, case_list:vararg
 _st_xjmp_always target, dir
 _st_label   ind, prefix=10
  _st_mk_xjmp   xjmp, ind, dir, prefix=10
 _do
 _until cond
 _while cond
 _break cond=always
 _sw_case_ REG VALUE
 _break_sw_
 _case_ REG VALUE
 _default_
 _end_sw_
 _sw_switch cmp
\******************************************************************************/
/*****************************************************************************/
.macro  _xjmp jp:req, type=r
        _strc_12b3 \type, jmp, \jp
.endm
.macro  _xjmp_njp type=r,    jp1, jp2, jp3, jp4, jp5, jp6, jp7, jp8, jp9
        _strc_12b3 \type, jmp, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
.endm
.macro  _rjmp_njp           jp1, jp2, jp3, jp4, jp5, jp6, jp7, jp8, jp9
        _strc_12b3 r, jmp, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
.endm
.macro  _gen_label  ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        \prefix\ifcnt\delim\nest\delim\macro:
        .print "prefix+ifcnt+delim+nest+delim+macro:"
.endm
#_gen_label %__if_nest_id, if
.macro  _gen_cjmp xjmp:req, ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        .print "\t c-branch:\t xjmp prefix+ifcnt+delim+nest+delim+macro"
        \xjmp \prefix\ifcnt\delim\nest\delim\macro
.endm
.macro  _gen_ujmp xjmp=rjmp, ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        .print "\t u-branch:\t xjmp prefix+ifcnt+delim+nest+delim+macro"
        \xjmp \prefix\ifcnt\delim\nest\delim\macro
        #_gen_cjmp \xjmp, \ifcnt, \nest, \macro, \prefix, \delim
.endm
/*****************************************************************************/
.macro  _if_bit xaddr:req, bit:req, set_or_clr=set, jp_false, rtmp=r24
  _get_op \xaddr
  .if (__mem_type==__MEM_TYPE_GPR)
  /* GPR SBRS/C */
      .if (\set_or_clr==set)
        .if (__DEB_IF_BIT__)
          .print "__MEM_TYPE_GPR(xaddr), if _bit(bit) = 1 = SET -> True, do THEN"
        .endif
        SBRS  \xaddr, \bit
        RJMP  \jp_false
      .endif
      .if (\set_or_clr==clr)
        .if (__DEB_IF_BIT__)
          .print "__MEM_TYPE_GPR(xaddr), if _bit(bit) = 0 = CLR -> True, do THEN"
        .endif
        SBRC  \xaddr, \bit
        RJMP  \jp_false
       .endif
  .elseif ((__mem_type==__MEM_TYPE_SFR)&&(__mem_part==__MEM_PART_SFRL))
  /* SFRL SBIS/C */
      .if (\set_or_clr==set)
        .if (__DEB_IF_BIT__)
          .print "__MEM_PART_SFRL(xaddr), if _bit(bit) = 1 = SET -> True, do THEN"
        .endif
        SBIS  \xaddr - __AVR_SFR_OFFSET, \bit
        RJMP  \jp_false
      .endif
      .if (\set_or_clr==clr)
        .if (__DEB_IF_BIT__)
          .print "__MEM_PART_SFRL(xaddr), if _bit(bit) = 0 = CLR -> True, do THEN"
        .endif
        SBIC  \xaddr - __AVR_SFR_OFFSET, \bit
        RJMP  \jp_false
      .endif
  .elseif ((__mem_type>=__MEM_TYPE_SFR)&&(__mem_part>=__MEM_PART_SFRH))
  /* SFRH -> GPR -> SBRS/C */
      .if (__mem_sreg==__MEM_SREG_SREG)
      /* SFRH/SREG -> BRBS/C */
          .if (\set_or_clr==set)
              .if (__DEB_IF_BIT__)
                .print "__MEM_SREG_SREG, if _bit(bit) = 1 = SET -> True, do THEN"
              .endif
              BRBC  \bit, \jp_false
          .endif
          .if (\set_or_clr==clr)
              .if (__DEB_IF_BIT__)
                .print "__MEM_SREG_SREG, if _bit(bit) = 0 = CLR -> True, do THEN"
              .endif
              BRBS  \bit, \jp_false
          .endif
          .exitm
      .else
      /* SFRH+ (-SREG) -> GPR -> SBRS/C */
          .if (__DEB_IF_BIT__)
            .print "(xaddr)>=SFRH"
          .endif
          _mcp \xaddr, \rtmp
          _if_bit \rtmp, \bit, \set_or_clr, \jp_false
      .endif
  .endif
.endm
/*****************************************************************************/
.macro  _ifb_true_skip xaddr:req, bit:req, set_or_clr=set, rtmp=r24
  _macro_ifb_true_skip_s_\@:
        /* if bit set/clr true then skip, false do */
        _get_op \xaddr
        .if (__mem_type==__MEM_TYPE_GPR)
        /* SBXSC */
            .if (\set_or_clr==set)
                .if (__DEB_IFB_TRUE_SKIP__)
                .print "__MEM_TYPE_GPR(xaddr), if _bit(bit) = 1 = SET -> True, do THEN"
                .endif
                SBRS  \xaddr, \bit
                #RJMP  \jp_false
            .endif
            .if (\set_or_clr==clr)
                .if (__DEB_IFB_TRUE_SKIP__)
                .print "__MEM_TYPE_GPR(xaddr), if _bit(bit) = 0 = CLR -> True, do THEN"
                .endif
                SBRC  \xaddr, \bit
                #RJMP  \jp_false
             .endif
        .elseif ((__mem_type==__MEM_TYPE_SFR)&&(__mem_part==__MEM_PART_SFRL))
        /* SBXSC */
            .if (\set_or_clr==set)
                .if (__DEB_IFB_TRUE_SKIP__)
                .print "__MEM_PART_SFRL(xaddr), if _bit(bit) = 1 = SET -> True, do THEN"
                .endif
                SBIS  \xaddr - __AVR_SFR_OFFSET, \bit
                #RJMP  \jp_false
            .endif
            .if (\set_or_clr==clr)
                .if (__DEB_IFB_TRUE_SKIP__)
                .print "__MEM_PART_SFRL(xaddr), if _bit(bit) = 0 = CLR -> True, do THEN"
                .endif
                SBIC  \xaddr - __AVR_SFR_OFFSET, \bit
                #RJMP  \jp_false
            .endif
        .else
                .if (__DEB_IFB_TRUE_SKIP__)
                .print "(xaddr)>=SFRH"
                .endif
                _mcp \xaddr, \rtmp
                _ifb_true_skip \rtmp, \bit, \set_or_clr
        .endif
  _macro_ifb_true_skip_e_\@:
.endm
/*****************************************************************************/
.macro  _if_cond cond:req, jp1:req, jp2, jp3, jp4, jp5, jp6, jp7, jp8, jp9
        #.rept _cond   cc,cs, sh,lo, ne,eq, pl,mi, vc,vs, ge,lt, hc,hs, tc,ts, id,ie
#.macro  _if_bit xaddr:req, bit:req, set_or_clr=set, jp_prefix, jp_false, rhtmp=r24
        .ifc \cond,cs
            _if_bit SREG, SREG_C, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,cc
            _if_bit SREG, SREG_C, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,lo
            _if_bit SREG, SREG_C, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,sh
            _if_bit SREG, SREG_C, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,eq
            _if_bit SREG, SREG_Z, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,ne
            _if_bit SREG, SREG_Z, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,e
            _if_bit SREG, SREG_Z, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,z
            _if_bit SREG, SREG_Z, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,nz
            _if_bit SREG, SREG_Z, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,pl
            _if_bit SREG, SREG_N, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,mi
            _if_bit SREG, SREG_N, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,vs
            _if_bit SREG, SREG_V, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,vc
            _if_bit SREG, SREG_V, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,le
            _if_bit SREG, SREG_S, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,ge
            _if_bit SREG, SREG_S, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,hs
            _if_bit SREG, SREG_H, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,hc
            _if_bit SREG, SREG_H, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,ts
            _if_bit SREG, SREG_T, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,tc
            _if_bit SREG, SREG_T, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif

        .ifc \cond,is
            _if_bit SREG, SREG_I, set, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
        .ifc \cond,ic
            _if_bit SREG, SREG_I, clr, \jp1\jp2\jp3\jp4\jp5\jp6\jp7\jp8\jp9
        .endif
.endm
/******************************************************************************/
# _if version 3
.set __IF_EXE_CNT__, 100
__if_nest_id =  0
__if_exe_cnt =  __IF_EXE_CNT__
__elif_e_cnt =  0
.macro  _if cmd, cond, jp=Eif
  _macro_if_s_\@:
        # init stack
        .if (__if_exe_cnt==__IF_EXE_CNT__)
            __gas_stack_init
        .endif
        __if_nest_id = __if_nest_id +1
        __if_exe_cnt = __if_exe_cnt +1
        __gas_stack_push %__if_exe_cnt
        __if_session_id = __gas_stack_value
        _prt_b _if      \t, __jp_, %__if_session_id, _, %__if_nest_id, _, if
        _gen_jp6a __jp_, %__if_session_id, _, %__if_nest_id, _, if
        \cmd
        .ifc \jp,else
          .if (__DEB_IF__)
            _prt_b _if cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
          .endif
            _if_cond \cond,  __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
            .exitm
        .endif
        .ifc \jp,elif
          .if (__DEB_IF__)
            _prt_b _if cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt+1, _, \jp
          .endif
            _if_cond \cond,  __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt+1, _, \jp
            .exitm
        .endif
        .ifc \jp,Eif
          .if (__DEB_IF__)
            _prt_b _if cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
          .endif
            _if_cond \cond,  __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
            .exitm
        .endif
        .ifc \jp,
          .if (__DEB_IF__)
            _prt_b _if cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
          .endif
            _if_cond \cond,  __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            .exitm
        .endif
        .ifnc \jp,
            .print "warning jp error"
            .exitm
        .endif
  _macro_if_e_\@:
.endm
.macro  _then
      .if (__DEB_IF__)
        _prt_b _then\t, __jp_, %__if_session_id, _, %__if_nest_id, _, then
      .endif
      _gen_jp6a       __jp_, %__if_session_id, _, %__if_nest_id, _, then
.endm
.macro  _Ethen jp
        __if_session_id = __gas_stack_value
      .if (__DEB_IF__)
        _prt_b _Ethen  \t, __jp_, %__if_session_id, _, %__if_nest_id, _, Ethen
      .endif
        _gen_jp6a          __jp_, %__if_session_id, _, %__if_nest_id, _, Ethen
      .if (__DEB_IF__)
        _prt_b _Ethen ujp, __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
      .endif
        .ifc \jp,Eif
          # no jp to Eif
            .exitm
        .else
            _rjmp_njp __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
        .endif
.endm
.macro  _else
      .if (__DEB_IF__)
        _prt_b _else   \t, __jp_, %__if_session_id, _, %__if_nest_id, _, else
      .endif
        _gen_jp6a          __jp_, %__if_session_id, _, %__if_nest_id, _, else
.endm
.macro  _Eelse
        __if_session_id = __gas_stack_value
      .if (__DEB_IF__)
        _prt_b _Eelse   \t, __jp_, %__if_session_id, _, %__if_nest_id, _, Eelse
      .endif
        _gen_jp6a           __jp_, %__if_session_id, _, %__if_nest_id, _, Eelse
        #_if_cond \cond,     __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
        #_rjmp_njp __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
.endm
.macro  _elif cmd, cond, jp
        __elif_e_cnt = __elif_e_cnt +1
      .if (__DEB_IF__)
        _prt_b _elif    \t, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, elif
      .endif
        _gen_jp8a           __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, elif
        \cmd
        .ifc \jp,else
          .if (__DEB_IF__)
            _prt_b _elif cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
          .endif
            _if_cond \cond,    __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
            .exitm
        .endif
        .ifc \jp,Eif
          .if (__DEB_IF__)
            _prt_b _elif cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
          .endif
            _if_cond \cond,    __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
            .exitm
        .endif
        .ifc \jp,elif
          .if (__DEB_IF__)
            _prt_b _elif cFjp, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt+1, _, elif
          .endif
            _if_cond \cond,    __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt+1, _, elif
            .exitm
        .endif
        .ifc \jp,
            .error "elif fail jp cannot empty"
            .exitm
        .endif
        .ifnc \jp,
            .print "warning jp error"
            .exitm
        .endif
.endm
.macro  _Eelif jp
        __if_session_id = __gas_stack_value
      .if (__DEB_IF__)
        _prt_b _Eelif \t, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eelif
      .endif
        _gen_jp8a         __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eelif
        .ifc \jp,Eif
            # no jp to Eif
            .exitm
        .else
            _rjmp_njp __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
        .endif
.endm
.macro  _Eif jp
        # jp=default=blank
        .ifc \jp,
          .if (__if_nest_id > 1)
            .if (__DEB_IF__)
              _prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            .endif
            _gen_jp6a      __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            .if (__DEB_IF__)
            _prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eif
            .endif
            _gen_jp8a      __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eif
            __gas_stack_pop
            __if_session_id = __gas_stack_value
            __if_nest_id    = __if_nest_id -1
            #_prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            #_gen_jp6a      __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            #_prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eif
            #_gen_jp8a      __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eif
          .elseif (__if_nest_id==1)
            # confirm no parent nest if
            .if (__DEB_IF__)
            _prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            .endif
            _gen_jp6a      __jp_, %__if_session_id, _, %__if_nest_id, _, Eif
            .if (__DEB_IF__)
            _prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eif
            .endif
            _gen_jp8a      __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, Eif
            #__gas_stack_pop
            #__if_session_id = __gas_stack_value
            __if_nest_id    = __if_nest_id -1
            __elif_e_cnt = 0
          .endif
          .exitm
        .endif
        .ifnc \jp,
        # jp=no-blank=err, [Ethen,Eelse,Eelif], not need with previous ifc
            .irp p_jp Ethen, Eelse, Eelif
               .ifc \jp,p_jp
#            _prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
#            _gen_jp6a      __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
#            _prt_b _Eif\t, __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, \jp
#            _gen_jp8a      __jp_, %__if_session_id, _, %__if_nest_id, _, %__elif_e_cnt, _, \jp
                  __gas_stack_pop
                  __if_session_id = __gas_stack_value
                  __if_nest_id = __if_nest_id -1
                  #_prt_b _Eif cFjp-, __jp_, %__gas_stack_value, _, %__if_nest_id-1, _, \jp
                  #_if_cond \cond,    __jp_, %__gas_stack_value, _, %__if_nest_id-1, _, \jp
                .if (__DEB_IF__)
                  _prt_b _Eif cFjp-, __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
                .endif
                  _if_cond \cond,    __jp_, %__if_session_id, _, %__if_nest_id, _, \jp
                  .exitm
               .endif
           .endr
        .else
           .print "warning jp error"
        .endif
.endm
/*****************************************************************************/
.set __JTB_INDEX_ID_INIT, 99
.set __JTB_EXEC_CNT_INIT,  0
  __jtb_id        = __JTB_INDEX_ID_INIT
  __jtb_exec_cnt  = __JTB_EXEC_CNT_INIT
.macro  _jtb_gen_tbl case_count:req
        __jtb_id = __jtb_id + 1
        __jtb_cnt = 0
  #_gen_label  ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        #\prefix\ifcnt\delim\nest\delim\macro:
        # __jtb __jtb_id _ _ _ _jtb_gen_tbl
#      _gen_label        %__jtb_id, _, _jtb_gen_tbl, __jtb_,
      _gen_label        %__jtb_id, vector, addr, __jtb_
      .print "_gen_label __jtb_id, $, _jtb_gen_tbl, __jtb,"
      .rept \case_count
        #_gen_cjmp xjmp:req, ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        #.print "\t c-branch:\t xjmp prefix+ifcnt+delim+nest+delim+macro"
        #\xjmp \prefix\ifcnt\delim\nest\delim\macro
          #_gen_ujmp __jtb_\__jtb_id\()_caseid_\__jtb_cnt
        _gen_ujmp rjmp, %__jtb_id, caseid, %__jtb_cnt, __jtb_
        __jtb_cnt = __jtb_cnt +1
      .endr
      _gen_label        %__jtb_id, vector, addr_end, __jtb_
.endm
.macro  _jtb_case_do id:req
        #_gen_label %__jtb_id, caseid, %_jtb_cnt, __jtb_
        _gen_label      %__jtb_id, caseid,    \id, __jtb_
        _gen_label      %__jtb_id, do_caseid, \id, __jtb_
.endm
.macro  _jtb_case_end id:req
        # rjmp __jtb_+100+_+caseid+_+0
        # rjmp __jtb_ 100 _end _ _
        _gen_ujmp rjmp, %__jtb_id, end, _, __jtb_
        _gen_label      %__jtb_id, end_caseid, \id, __jtb_
.endm
.macro  _jtb_end
        # rjmp __jtb_+100+_+caseid+_+0
        # rjmp __jtb_ 100 _end _ _
        _gen_label      %__jtb_id, end, _, __jtb_
.endm
.macro  _jtb_z_ld_addr prefix=__jtb_, id, suffix=_caseid_0
        LDI ZH, hi8(\prefix\id\suffix)
        LDI ZL, lo8(\prefix\id\suffix)
.endm
.macro  _jtb_switch_if true_id, false_id, rtmp=r24, cond:req, _cmds:vararg
        /* not done */
        _jtb_z_ld_addr , %__jtb_id
        # cmds = ADD ZL,R0, or ADI ZL,1 or ADIW ZH:ZL,K6[0,63]
        # cmds = any CP4T ; sh/lo ; jmp deflt
        _exec_opcode_0a _cmds
        .print "_cmds"
        # manual check carry bit, cs = carry set, inc ZH
        .ifc \cond,cs
          _gen_cjmp brcc, %__jtb_id, nocarry, _, __jtb_
          INC   ZH
          _gen_label      %__jtb_id, nocarry, _, __jtb_
          IJMP
        .endif
        # any if like condition, cp/c r1,r2 eq/ne; cpi eq; cpse/tst
        # check range conditions, like < 10 or>=100
        .ifnc \cond,cs
          _gen_cjmp _st_xjmp_not_\cond, %__jtb_id, true_id, \false_id, __jtb_

          LDI   rtmp,\true_id /* load true jmp id */
          ADD   ZL, rtmp
          _gen_cjmp br\cond,  %__jtb_id, true_jmp, _, __jtb_
          INC   ZH
          _gen_label          %__jtb_id, true_jmp, \true_id, __jtb_
          IJMP

          _gen_label      %__jtb_id, true_id, \false_id, __jtb_
          ADI ZL,\false_id
          _gen_cjmp brcc, %__jtb_id, false_id_nc, _, __jtb_
          INC   ZH
          _gen_label      %__jtb_id, false_id_nc, _, __jtb_
          IJMP
        .endif
.endm
.macro  _jtb_switch reg
        __jtb_exec_cnt = __jtb_exec_cnt + 1
        _gen_label      %__jtb_id, do_switch, %__jtb_exec_cnt, _jtb_
        _jtb_z_ld_addr , %__jtb_id,
#        ADD   ZL, R16
        ADD   ZL, \reg
#        BRCC  no_carry
        #_gen_cjmp xjmp:req, ifcnt:req, nest:req, macro:req, prefix=_jp_, delim=_
        #.print "\t c-branch:\t xjmp prefix+ifcnt+delim+nest+delim+macro"
        #\xjmp \prefix\ifcnt\delim\nest\delim\macro
        #_gen_cjmp _st_xjmp\not_\cond, %__jtb_id, nocarry, _, __jtb_
        _gen_cjmp brcc, %__jtb_id, do_switch_nc, %__jtb_exec_cnt, _jtb_
        INC   ZH
#no_carry:
        _gen_label      %__jtb_id, do_switch_nc, %__jtb_exec_cnt, _jtb_
/*
        _if <ADD ZL,R0>, cs
            INC ZH
        _endif
*/
        IJMP
        _gen_label      %__jtb_id, end_switch, %__jtb_exec_cnt, _jtb_
.endm
.macro  _jtb_gen_batch_cases cmds, case_list:vararg
        .irp case \case_list
          _jtb_case_do \case
            cmds
          _jtb_case_end \case
        .endr
.endm
/*****************************************************************************/
.macro _st_xjmp_always target, dir
  rjmp \target\dir
.endm

/*
 * define all of our conditional branches, "true" logic
 * SREG - Status Register
 * SREG_C  = 0 ; Carry Flag
 * SREG_Z  = 1 ; Zero Flag
 * SREG_N  = 2 ; Negative Flag
 * SREG_V  = 3 ; Two's Complement Overflow Flag
 * SREG_S  = 4 ; Sign Bit
 * SREG_H  = 5 ; Half Carry Flag
 * SREG_T  = 6 ; Bit Copy Storage
 * SREG_I  = 7 ; Global Interrupt Enable
 * 0-C 1-Z 2-N 3-V 4-S 5-H 6-T 7-I
 *       0-1 SREG_C,SREG_C,SREG_Z,SREG_N,SREG_V,SREG_S,SREG_H,SREG_T,SREG_I */
.irp  cond,   cc,cs, sh,lo, ne,eq, pl,mi, vc,vs, ge,lt, hc,hs, tc,ts, id,ie
    .macro  _st_xjmp_\cond  target
              br\cond \target
    .endm
.endr
/*
 * Additional positive logic branches
 */
.macro _st_xjmp_z target
  breq \target
.endm

.macro _st_xjmp_e target
  breq \target
.endm
  
.macro _st_xjmp_nz target
  brne \target
.endm

/*
 * for each possible branch condition "x", create a macro _st_xjmp_not_x
 * that does a jump for "NOT x".
 */
.macro _st_xjmp_not_z target, dir
  brne \target\dir
.endm
.macro _st_xjmp_not_nz target, dir
  breq \target\dir
.endm
.macro _st_xjmp_not_e target, dir
  brne \target\dir
.endm
.macro _st_xjmp_not_n target, dir
  breq \target\dir
.endm
.macro _st_xjmp_not_eq target, dir
  brne \target\dir
.endm
.macro _st_xjmp_not_ne target, dir
  breq \target\dir
.endm
.macro _st_xjmp_not_cs target, dir
  brcc \target\dir
.endm
.macro _st_xjmp_not_cc target, dir
  brcs \target\dir
.endm
.macro _st_xjmp_not_sh target, dir
  brlo \target\dir
.endm
.macro _st_xjmp_not_lo target, dir
  brsh \target\dir
.endm
.macro _st_xjmp_not_mi target, dir
  brpl \target\dir
.endm
.macro _st_xjmp_not_pl target, dir
  brmi \target\dir
.endm
.macro _st_xjmp_not_ge target, dir
  brlt \target\dir
.endm
.macro _st_xjmp_not_lt target, dir
  brge \target\dir
.endm
.macro _st_xjmp_not_hs target, dir
  brhc \target\dir
.endm
.macro _st_xjmp_not_hc target, dir
  brhs \target\dir
.endm
.macro _st_xjmp_not_ts target, dir
  brtc \target\dir
.endm
.macro _st_xjmp_not_tc target, dir
  brts \target\dir
.endm
.macro _st_xjmp_not_vs target, dir
  brvc \target\dir
.endm
.macro _st_xjmp_not_vc target, dir
  brvs \target\dir
.endm
.macro _st_xjmp_not_ie target, dir
  brid \target\dir
.endm
.macro _st_xjmp_not_id target, dir
  brie \target\dir
.endm

;;; allow _if skp after an instruction that might have skipped.
;;; this assembles to an unconditional jump
.macro  _st_xjmp_not_skp target, dir
  rjmp \target\dir
.endm
/*
 * _st_label
 * define a local label here (at ":")
 * depending on the counter "ind" and optional prefix
*/
.macro  _st_label   ind, prefix=10
        \prefix\ind:
        .print "prefix ind:"
.endm
.macro  _st_mk_xjmp   xjmp, ind, dir, prefix=10
        \xjmp  \prefix\ind\dir
        .print "_st_mk_xjmp   xjmp, ind, dir, prefix=10"
        .print "xjmp  prefix+ind+dir"
.endm

/*
;;; Looping macros.  "until cc" will generate a jmp_not_cc backward,
;;; and "while cc" will generate a jmp_cc backward.
*/
/* Nesting level initialization */
  .set __ST_IFNEST, -2
  .set __ST_LPNEST, -2
.macro _do
  .set __ST_LPNEST, __ST_LPNEST+2
  _st_label %__ST_LPNEST, 20
.endm
  
.macro _until cond
  _st_mk_xjmp _st_xjmp_not_\cond, %__ST_LPNEST, b, 20
  _st_label %__ST_LPNEST+1, 20
  .set __ST_LPNEST, __ST_LPNEST-2
.endm

.macro _while cond
  _st_mk_xjmp _st_xjmp_\cond, %__ST_LPNEST, b, 20
  _st_label %__ST_LPNEST+1, 20
  .set __ST_LPNEST, __ST_LPNEST-2
.endm

.macro _break cond=always
  _st_mk_xjmp _st_xjmp_\cond, %(__ST_LPNEST+1), f, 20
.endm
/*****************************************************************************/
/*
https://github.com/WestfW/structured_gas/issues/1
jpelletier commented on Jun 17, 2013
These macros specific for the AVR microcontrollers add a switch construct
;;;-----------------------------------------------------------------
;;; Switch
;;; _sw_case R16-31,Value
;;; _case R16-31,Value
;;; _break_sw
;;; _default
;;; _end_sw
;;;-----------------------------------------------------------------
.macro _sw_case_ REG VALUE
cpi \REG,\VALUE
.set __ST_IFNEST, __ST_IFNEST+2
_st_mkjmp _st_jmp_not_eq, %__ST_IFNEST, f
.endm

.macro _break_sw_
_st_mkjmp _st_jmp_always, %__ST_IFNEST+1, f
.endm

.macro _case_ REG VALUE
_st_label %__ST_IFNEST
cpi \REG,\VALUE
_st_mkjmp _st_jmp_not_eq, %__ST_IFNEST, f
.endm

.macro _default_
_st_label %__ST_IFNEST
.endm

.macro _end_sw_
_st_label %__ST_IFNEST
_st_label %__ST_IFNEST+1
.set __ST_IFNEST, __ST_IFNEST-2
.endm
*/
/*
.macro _sw_switch cmp
# same as _if_elif_elif_else_tb
_if <cpi r24,0>, <eq>, elif
    jmp_case_n
    _endthen
_elif <cpi r24,1>, <eq>
    jmp_case_n
_endelif
_elif <cpi r24,1>, <eq>
    jmp_case_n
_endelif
_else
    do else
_endif
.endm
*/
/* jmp table switch case 
P320PP351 Some Assembly Required CH9.3.4
TABLE 9.8 Implementation of a Multiway Selection Using a Jump Table
;start switch
cpi m, 5
brsh casedefault 
ldi ZH, high(jtab) 
ldi ZL, low(jtab) 
add ZL, m
brcc no_carry
inc ZH
no_carry:
  ijmp
;jump table
jtab:
  rjmp casedefault
  rjmp case1
  rjmp case2
  rjmp case3
  rjmp case4
;cases
case1:
case3:
inc m
  rjmp end_switch
case2:
inc n
  rjmp end_switch
case4:
add m, n 
casedefault:
  clr n
end_switch:
*/
