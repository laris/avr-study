##define __DEBUG_COMMON__ 1
/*
 * RAM address range = \
 * GPR[0,$1F] + SFR[$20,$5F] + ESFR[$60,ESFR_END/IOEND] + RAM[SRAM_START,RAMEND]
 */

/*
.set  _avr_symbol_cnt, 0
.irp  _avr_symbol _ZERO,\
      _GPR,_GPRL,_GPRH,_PTR,\
      _SFR,_SFRL,_SFRH,_ESFR,\
      _ISRAM,_ESRAM,\
      _PX,_PXL,_PXH,\
      _PY,_PYL,_PYH,\
      _PZ,_PZL,_PZH
      #define \_avr_symbol \_avr_symbol_cnt
.endr
*/
#define _PNULL -1 /* Part = NULL */
#define _ZERO   0
#define _GPR    1
#define _GPRL   2
#define _GPRH   3
#define _PTR    4
#define _SFR    5
#define _SFRL   6
#define _SFRH   7
#define _ESFR	  8
#define _ISRAM  9
#define _ESRAM 10
#define _PX    11
#define _PXL   12
#define _PXH   13
#define _PY    14
#define _PYL   15
#define _PYH   16
#define _PZ    17
#define _PZL   18
#define _PZH   19
#define _SREG  20 

#define _SFR_START  0x0020
#define _SFR_END    0x005f
#define _SFRL_START 0x0020
#define _SFRL_END   0x003f
#define _SFRH_START 0x0040
#define _SFRH_END   0x005f
#define _ESFR_START -1 /*0x0060
#define _ESFR_END   0x00ff 
#define _IOEND      _ESFR_END */
#define _SRAM_START 0x60 /* 0x0100 */
#define _SRAM_SIZE  2048
#define _RAMEND     0x25F /*0x08ff */
#define _SRAM_END   _RAMEND

/*
.set  __X = -1 ; Range
.set  __Y = -1 ; Detail
.set  __Z = -1 ; Input symbol
*/
.altmacro

.macro  _get_ptr_type xptr:req
  __abs_addr  = -1
  __ptr_type  = -1
  __mem_type  = -1
  .if (\xptr == "x")||(\xptr == "X")
    __abs_addr = 26
    __ptr_type = _PX
    __mem_type = _PTR
    #if defined (__DEBUG_GET_PRT_TYPE__)
    .print "_PX"
    #endif
    .exitm
  .endif
  .if (\xptr == "y")||(\xptr == "Y")
    __abs_addr = 28
    __ptr_type = _PY
    __mem_type = _PTR
    #if defined (__DEBUG_GET_PRT_TYPE__)
    .print "_PY"
    #endif
    .exitm
  .endif
  .if (\xptr == "z")||(\xptr == "Z")
    __abs_addr = 30
    __ptr_type = _PZ
    __mem_type = _PTR
    #if defined (__DEBUG_GET_PRT_TYPE__)
    .print "_PZ"
    #endif
    .exitm
  .endif
.endm
.macro _get_ptr_type_tb
  .irp item x,X,y,Y,z,Z
    _get_ptr_type \item
  .endr
.endm
#_get_ptr_type_tb

.macro  _get_aaddr  xaddr:req  /* input reg/sfr/ram symbal/address, get abs addr+region */
/* Assume SFR address = absolute addr
#define __SFR_OFFSET  0x20
*/
  __abs_addr = -1
  __mem_type = -1
  __mem_part = -1
  __mem_part_sreg = -1
  __ptr_type = -1
  __ptr_part = -1
  __cnt      =  0
   /* check init value */
  .if ((__abs_addr==-1) && (__mem_type==-1)&&(__mem_part==-1)&&(__cnt==0))
    #.print "start to check GPR"
    #.print "checking R[0,15]"
    /* GPRL R[0,15]*/
    .irp  _cpu_reg,  R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10,R11,R12,R13,R14,R15
      #.print "irp"
      .ifc  \_cpu_reg,\xaddr
        __abs_addr = __cnt
        __mem_type = _GPR
        __mem_part = _GPRL
  #if defined (__DEBUG_GET_AADDR__)
            .print "\xaddr was found in R_ GPRL"
  #endif
        .exitm
        #.ifeq (__mem_part - _GPRL)
        #  .print "eq OK"
        #  .exitm
        #.endif
      .endif
      __cnt = __cnt + 1       /* now cnt = 16 */
    .endr
    #.print "checking r[0,15]"
    .if ((__cnt==16)&&(__mem_type==-1)) /* no result in R[0,15], check r[0,15] */
      __cnt = 0                          /* reset cnt = 0 */
      /* GPRL r[0,15]*/
      .irp  _cpu_reg,  r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10,r11,r12,r13,r14,r15
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = __cnt
          __mem_type = _GPR
          __mem_part = _GPRL
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in r_ GPRL"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1     /* now cnt = 16 */
      .endr
    .endif
    #.print "checking R[16,31]"
    .if ((__cnt==16)&&(__mem_type==-1)) /* no result in r[0,15], check R[16,31] */
      /*__cnt = 16                             keep cnt = 16 */
      /* GPRH R[16,31]*/
      .irp  _cpu_reg,  R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,R26,R27,R28,R29,R30,R31
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = __cnt
          __mem_type = _GPR
          __mem_part = _GPRH
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in R_ GPRH"
  #endif
            .ifc \_cpu_reg,R26
              __ptr_type = _PX
              __ptr_part = _PXL
            .endif
            .ifc \_cpu_reg,R27
              __ptr_type = _PX
              __ptr_part = _PXH
            .endif
            .ifc \_cpu_reg,R28
              __ptr_type = _PY
              __ptr_part = _PYL
            .endif
            .ifc \_cpu_reg,R29
              __ptr_type = _PY
              __ptr_part = _PYH
            .endif
            .ifc \_cpu_reg,R30
              __ptr_type = _PZ
              __ptr_part = _PZL
            .endif
            .ifc \_cpu_reg,R31
              __ptr_type = _PZ
              __ptr_part = _PZH
            .endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 32 */
    .endif
    #.print "checking r[16,31]"
    .if ((__cnt==32)&&(__mem_type==-1)) /* no result in R[0,15], check r[16,31] */
      __cnt = 16                             /* reset cnt = 16 */
      /* GPRH r[16,31]*/
      .irp  _cpu_reg,  r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = __cnt
          __mem_type = _GPR
          __mem_part = _GPRH
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in r_ GPRH"
  #endif
            .ifc \_cpu_reg,r26
              __ptr_type = _PX
              __ptr_part = _PXL
            .endif
            .ifc \_cpu_reg,r27
              __ptr_type = _PX
              __ptr_part = _PXH
            .endif
            .ifc \_cpu_reg,r28
              __ptr_type = _PY
              __ptr_part = _PYL
            .endif
            .ifc \_cpu_reg,r29
              __ptr_type = _PY
              __ptr_part = _PYH
            .endif
            .ifc \_cpu_reg,r30
              __ptr_type = _PZ
              __ptr_part = _PZL
            .endif
            .ifc \_cpu_reg,r31
              __ptr_type = _PZ
              __ptr_part = _PZH
            .endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 32 */
    .endif
    /* no result in R|r[0,31], check XYZ */
    .if (__mem_type==-1)
      #.print "\xaddr checking xl"
      __cnt = 0   /* reset cnt */
      __mem_type = -1
      .irp  _cpu_reg,  xl, xL, Xl, XL
        #.print "\xaddr checking xl in irp"
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = 26 /* R26 $1A */
          __mem_type = _GPR
          __mem_part = _GPRH
          __ptr_type = _PX
          __ptr_part = _PXL
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in XL"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    #.print "checking xh"
    .if ((__cnt<=4)&&(__mem_type==-1)) /* no result in R|r[0,31], check XYZ */
      __cnt = 0   /* reset cnt */
      .irp  _cpu_reg,  xh, xH, Xh, XH
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = 27 /* R27 $1B */
          __mem_type = _GPR
          __mem_part = _GPRH
          __ptr_type = _PX
          __ptr_part = _PXH
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in XH"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    #.print "checking yl"
    .if ((__cnt<=4)&&(__mem_type==-1)) /* no result in R|r[0,31], check XYZ */
      __cnt = 0   /* reset cnt */
      .irp  _cpu_reg,  yl, yL, Yl, YL
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = 28 /* R28 $1C */
          __mem_type = _GPR
          __mem_part = _GPRH
          __ptr_type = _PY
          __ptr_part = _PYL
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in YL"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    #.print "checking yh"
    .if ((__cnt<=4)&&(__mem_type==-1)) /* no result in R|r[0,31], check XYZ */
      __cnt = 0   /* reset cnt */
      .irp  _cpu_reg,  yh, yH, Yh, YH
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = 29 /* R29 $1D */
          __mem_type = _GPR
          __mem_part = _GPRH
          __ptr_type = _PY
          __ptr_part = _PYH
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in YH"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    #.print "checking zl"
    .if ((__cnt<=4)&&(__mem_type==-1)) /* no result in R|r[0,31], check XYZ */
      __cnt = 0   /* reset cnt */
      .irp  _cpu_reg,  zl, zL, Zl, ZL
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = 30 /* R30 $1E */
          __mem_type = _GPR
          __mem_part = _GPRH
          __ptr_type = _PZ
          __ptr_part = _PZL
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in ZL"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    #.print "checking zh"
    .if ((__cnt<=4)&&(__mem_type==-1)) /* no result in R|r[0,31], check XYZ */
      __cnt = 0   /* reset cnt */
      .irp  _cpu_reg,  zh, zH, Zh, ZH
        .ifc  \_cpu_reg,\xaddr
          __abs_addr = 31 /* R31 $1F */
          __mem_type = _GPR
          __mem_part = _GPRH
          __ptr_type = _PZ
          __ptr_part = _PZH
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in ZH"
  #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    #if defined (__DEBUG_GET_AADDR__)
      #.print "check ptr xXyYzZ"
    #endif
    .if ((__cnt<=4)&&(__mem_type==-1))
      __cnt = 0   /* reset cnt */
      .irp  _cpu_reg,  x,y,z,X,Y,Z
        .ifc  \_cpu_reg,\xaddr
          __mem_type = _PTR
          _get_ptr_type \xaddr
          #if defined (__DEBUG_GET_AADDR__)
            .print "\xaddr was found in _PTR" 
          #endif
          .exitm
        .endif
        __cnt = __cnt + 1
      .endr                 /* now cnt = 4 */
    .endif
    /* out of GPR, start SFR */
    #.print "checking SFRL"
    /*
    .if (__mem_type==-1)
      __cnt = 0   /* reset cnt */ /*
      #.print "no result in GPR, check SFR"
      #if defined (__AVR_ATtiny85__)
      .irp _sfr SREG,SPH,SPL,ADCSRB,ADCL,ADCH,ADCSRA,ADMUX,ACSR,USICR,USISR,USIDR,USIBR,GPIOR0,GPIOR1,GPIOR2,DIDR0,PCMSK,PINB,DDRB,PORTB,EECR,EEDR,EEARL,EEARH,PRR,WDTCR,DWDR,DTPS1,DT1B,DT1A,CLKPR,PLLCSR,OCR0B,OCR0A,TCCR0A,OCR1B,GTCCR,OCR1C,OCR1A,TCNT1,TCCR1,OSCCAL,TCNT0,TCCR0B,MCUSR,MCUCR,SPMCSR,TIFR,TIMSK,GIFR,GIMSK
      #endif
        .ifc \_sfr,\xaddr
          __abs_addr = _sfr /*+ 0x20 /* not useful ? */ /*
          __mem_type = _SFR
          __mem_part = _SFRL
          .exitm
          .print "\xaddr was found in SFR"
        .endif
        __cnt = __cnt + 1
      .endr
    .endif
    */
    .if (__mem_type == -1)
  #if defined (__DEBUG_GET_AADDR__)
        .print "\xaddr not found in symbol, search integer addr list"
  #endif
      .if ((\xaddr >= 0) && (\xaddr <= 0x0F))
          __abs_addr = \xaddr
          __mem_type = _GPR
          __mem_part = _GPRL
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in GPRL"
  #endif
          .exitm
      .endif
      .if ((\xaddr >= 0x10) && (\xaddr <= 0x1F))
          __abs_addr = \xaddr
          __mem_type = _GPR
          __mem_part = _GPRH
  #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in GPRH"
  #endif
          .exitm
      .endif
      #.print "out of _cpu_reg, checking SFR/SRAM"
      .if ((\xaddr > 0x1F) && (\xaddr <= 0x3F))
        __cnt       = 0
        __abs_addr  = (\xaddr)
        __mem_type  = _SFR
        __mem_part  = _SFRL
  #if defined (__DEBUG_GET_AADDR__)
        .print "\xaddr was found in SFR/L <--"
  #endif
        .exitm
      .endif
      .if ((\xaddr > 0x3F) && (\xaddr <= 0x5F))
        __cnt       = 0
        __abs_addr  = (\xaddr)
        __mem_type  = _SFR
        __mem_part  = _SFRH
        #if defined (__DEBUG_GET_AADDR__)
          .print "\xaddr was found in SFR/H"
        #endif
            .if (\xaddr == 0x5F)
              __mem_part_sreg = _SREG
              #if defined (__DEBUG_GET_AADDR__)
                .print "\xaddr was found in SFR/H SREG"
              #endif
            .endif
        .exitm
      .endif
      .if ((\xaddr > 0x5F) && (\xaddr < RAMSTART))
        __cnt       = 0
        __abs_addr  = (\xaddr)
        __mem_type  = _ESFR
        __mem_part  = _ESFR
  #if defined (__DEBUG_GET_AADDR__)
        .print "\xaddr was found in ESFR"
  #endif
        .exitm
      .endif
      #.print "checking SRAM"
      .if ((\xaddr >= RAMSTART) && (\xaddr <= RAMEND))
        __cnt       = 0
        __abs_addr  = (\xaddr)
        __mem_type  = _ISRAM
        __mem_part  = _ISRAM
  #if defined (__DEBUG_GET_AADDR__)
        .print "\xaddr was found in ISRAM"
  #endif
        .exitm
      .endif
      .if ((\xaddr > RAMEND) && (\xaddr <= 0xFFFF ))
        /* External SRAM, P16 = (RAMEND,0xFFFF] */
        __cnt       = 0
        __abs_addr  = (\xaddr)
        __mem_type  = _ESRAM
        __mem_part  = _ESRAM
  #if defined (__DEBUG_GET_AADDR__)
        #.warning  "out of ISRAM"
        .print    "\xaddr was found in ESRAM"
        #.error    "out of ISRAM"
  #endif
        .exitm
      .endif
      .if ((\xaddr > 0xFFFF ))
        #.print    "\xaddr was found "
        .warning  "out of ISRAM"
        #.error    "out of I/E/SRAM"
        .exitm
      .endif
    .endif
    #.endif
  .else
    .error "not init value"
  .endif
.endm

.macro  _get_aaddr_tb
        .irp  xaddr R0, R1, R2, R3, R4, R5, R6, R7, R8, R9, R10,R11,R12,R13,R14,R15,\
                    r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10,r11,r12,r13,r14,r15,\
                    R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,R26,R27,R28,R29,R30,R31,\
                    r16,r17,r18,r19,r20,r21,r22,r23,r24,r25,r26,r27,r28,r29,r30,r31,\
                    xl, xL, Xl, XL,\
                    xh, xH, Xh, XH,\
                    yl, yL, Yl, YL,\
                    yh, yH, Yh, YH,\
                    0x20,0x3f,PORTB,DDRB,\
                    0x40,0x5f,\
                    0x60,0x25f,0x260
                    #if defined (RHT1)
                    \,RHT1
                    #endif
          _get_aaddr \xaddr
        .endr
.endm

#_get_aaddr r0
##define RHT1 R16
# test bench
#_get_aaddr_tb
#_get_aaddr TCNT0
#IN R16, (__abs_addr)
#IN RHT1, (__abs_addr)
#LDS RHT1, (__abs_addr)
/*****************************************************************************/
/* String */
/*****************************************************************************/
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
#_cmd_join2args_wcomma ldi, r16, 0
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
            .if  (__str_gen_ret == "")
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
.macro  _str_index str:req, var_strlist:req
        __str_index_ret = -1
        _str_len \var_strlist
        .irp s \var_strlist
            .ifnc str,s
                __str_index_ret  = __str_index_ret +1
            .endif
            .ifc str,s
                __str_index_ret  = __str_index_ret +1
                _prt_num %__str_index_ret
                .exitm
            .endif
        .endr
.endm
#_str_gen "0,1,2,three"
#_str_index 0, %__str_gen_ret
#_str_index three
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
        .if (__str_srch_ret == -1)
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
.macro  _strc_c12b3 s1,s2,s3
        \s1\s2  \s3 
.endm
.macro  _strc_c12bc16xarg s1,s2, a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16
        \s1\s2  a1\a2\a3\a4\a5\a6\a7\a8\a9\a10\a11\a12\a13\a14\a15\a16
.endm
.macro  _strc_c12b3b4 s1,s2,s3,s4
        \s1\s2  \s3 \s4
.endm
.macro  _strc_c1b2b34 s1,s2,s3,s4
        \s1 \s2 \s3\s4
.endm

.macro  _strc_1bc23 s1,s2,s3,s4
        \s1 \s2\s3
.endm
.macro  _strc_1b2comma3 s1, s2, s3
        \s1 \s2 , \s3
.endm
.macro  _strc_1comma2  s1, s2
        \s1 , \s2
.endm

#define __GAS_STACK_SIZE__ 100
__gas_stack_ptr  =   0
__gas_stack_value = -1
.macro  __gas_stack_init
        __gas_stack_ptr  =  __GAS_STACK_SIZE__
        .rept __GAS_STACK_SIZE__
            #.set __gas_stack_val_p %__gas_stack_ptr,    -1
            _strc_c12b3b4 __gas_stack_val_p, %__gas_stack_ptr, =-1
            #_prt_num %__gas_stack_ptr
            __gas_stack_ptr = __gas_stack_ptr -1
        .endr
.endm
.macro  __gas_stack_push val_push
        #.print "push b4 __gas_stack_ptr"
        #_prt_num %__gas_stack_ptr
        __gas_stack_ptr   = __gas_stack_ptr  +1
        #.print "push af __gas_stack_ptr"
        #_prt_num %__gas_stack_ptr
        __gas_stack_value = \val_push
        _strc_c12b3b4 __gas_stack_val_p, %__gas_stack_ptr, =, \val_push
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
/******************************************************************************/
.macro  _get_bin __input
        // get binary code from __input string
  _macro_get_bin_s_\@:
        __binary_code = -1
        .irp __one 1, \
                   TRUE, True, true, \
                   YES, Yes, yes, \
                   SET, Set, set
            .ifc \__input,__one
                __binary_code = 1
                #if defined(__DEBUG_COMMON__)
                .print "__input"
                _prt_num %__binary_code
                .print "\n"
                #endif
                .exitm
            .endif
        .endr
        .irp __zero 0, \
                   FALSE, False, flase, \
                   NO, No, no, \
                   CLEAR, Clear, clear,\
                   CLR, Clr, clr
            .ifc \__input,__zero
                __binary_code = 0
                #if defined(__DEBUG_COMMON__)
                .print "__input"
                _prt_num %__binary_code
                .print "\n"
                #endif
                .exitm
            .endif
        .endr
        .if (__binary_code == -1)
            .print "__input not valid binary 1 or 0 symbol"
        .endif
  _macro_get_bin_e_\@:
.endm
/******************************************************************************/
.macro  _mem_opcode_1x_addr _opcode:req, _dst:req, _rtmp1=r16
  _macro_mem_opcode_1x_addr_start_\@:
  __mem_type_dst = -1
  __mem_part_dst = -1
  __abs_addr_dst = -1
  _get_aaddr \_dst
    __mem_type_dst = __mem_type
    __mem_part_dst = __mem_part
    __abs_addr_dst = __abs_addr
  .if (__mem_type_dst == _GPR)
      \_opcode \_dst
      .exitm
  .endif
  .if (__mem_type_dst >= _SFR)
      _mcp \_dst,   \_rtmp1
      \_opcode  \_rtmp1
      _mcp \_rtmp1, \_dst
      .exitm
  .endif
  _macro_mem_opcode_1x_addr_end_\@:
.endm

.macro  _mem_opcode_2x_addr _opcode:req, _src:req, _dst:req, _rtmp1=r16, _rtmp2=r17
  _macro_mem_opcode_2x_addr_start_\@:
  // add src+dst to dst, src/dst = any memory space, include gpr/sfr/sram
  __mem_type_src = -1
  __mem_part_src = -1
  __abs_addr_src = -1
  __mem_type_dst = -1
  __mem_part_dst = -1
  __abs_addr_dst = -1
  _get_aaddr \_src
    __mem_type_src = __mem_type
    __mem_part_src = __mem_part
    __abs_addr_src = __abs_addr
  _get_aaddr \_dst
    __mem_type_dst = __mem_type
    __mem_part_dst = __mem_part
    __abs_addr_dst = __abs_addr
  .if ((__mem_type_src == _GPR)&&(__mem_type_dst == _GPR))
      \_opcode \_dst, \_src
      .exitm
  .endif
  .if ((__mem_type_src == _GPR)&&(__mem_type_dst >= _SFR))
      _mcp \_dst,  \_rtmp1
      \_opcode  \_rtmp1, \_src
      _mcp \_rtmp1, \_dst
      .exitm
  .endif
  .if ((__mem_type_dst == _GPR)&&(__mem_type_src >= _SFR))
      _mcp \_src,  \_rtmp1
      \_opcode  \_dst,  \_rtmp1
      .exitm
  .endif
  .if ((__mem_type_src >= _SFR)&&(__mem_type_dst >= _SFR))
      _mcp \_src,  \_rtmp1
      _mcp \_dst,  \_rtmp2
      \_opcode  \_rtmp2, \_rtmp1
      _mcp \_rtmp2, \_dst
      .exitm
  .endif
  _macro_mem_opcode_2x_addr_end_\@:
.endm
