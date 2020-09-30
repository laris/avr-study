/******************************************************************************\
  _initsp
  _pushi8 k8:req, rhtmp=r24
  _pushi16hl src_k16:req, rhtmp=r24
  _pop16lh dst_lo8:req, dst_hi8:req
  _pushi24hl src_k24:req, rhtmp=r24
  _pop24lh dst_lo8:req, dst_hi8:req, dst_hh8:req
  _pushi32hl src_k32:req, rhtmp=r24
  _pop32lh dst_lo8:req, dst_hi8:req, dst_hlo8:req, dst_hhi8:req

\******************************************************************************/
.macro  _initsp
        .if (__DEB_MACSIM__)
        _mwrk hi8(0x07FE), SPH
        _mwrk lo8(0x07FE), SPL
        .else
        _mwrk hi8(RAMEND), SPH
        _mwrk lo8(RAMEND), SPL
        .endif
.endm
.macro  _init_sp
        _initsp
.endm
/******************************************************************************/
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
/******************************************************************************/
.macro  _pushi8 k8:req, rhtmp=r24
        _macro_pushi8_s_\@:
        LDI  \rhtmp, lo8(\k8)
        PUSH \rhtmp
        _macro_pushi8_e_\@:
.endm
.macro  _pushi16hl src_k16:req, rhtmp=r24
# push high first with little-endian, little: store/tx lsb firstly in ram
# call  low first with big-endian to save return address
        LDI   \rhtmp, hi8(\src_k16)
        PUSH  \rhtmp
        LDI   \rhtmp, lo8(\src_k16)
        PUSH  \rhtmp
.endm
.macro  _pop16lh dst_lo8:req, dst_hi8:req
        POP  dst_lo8
        POP  dst_hi8
.endm
.macro  _pushi24hl src_k24:req, rhtmp=r24
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
.macro  _pushi32hl src_k32:req, rhtmp=r24
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
/******************************************************************************/
.macro  _pushp ptr:req
  _macro_pushp_s\@:
  .irp str x, X
  #, xl, xL, Xl, XL, xh, xH, Xh, XH, 26, r26, R26, 27, r27, R27
    .ifc \ptr,\str
        PUSH XH
        PUSH XL
    .endif
    .exitm
  .endr
  .irp str y, Y
  #, yl, yL, Yl, YL, yh, yH, Yh, YH, 28, r28, R28, 29, r29, R29
    .ifc \ptr,\str
        PUSH YH
        PUSH YL
    .endif
    .exitm
  .endr
  .irp str z, Z
  #, zl, zL, Zl, ZL, zh, zH, Zh, ZH, 30, r30, R30, 31, r31, R31
    .ifc \ptr,\str
        PUSH ZH
        PUSH ZL
    .endif
    .exitm
  .endr
  _macro_pushp_e\@:
.endm
/*----------------------------------------------------------------------------*/
.macro  _popp ptr:req
  _macro_popp_s\@:
  .irp str x, X
  #, xl, xL, Xl, XL, xh, xH, Xh, XH, 26, r26, R26, 27, r27, R27
    .ifc \ptr,\str
        POP   XL
        POP   XH
    .endif
    .exitm
  .endr
  .irp str y, Y
  #, yl, yL, Yl, YL, yh, yH, Yh, YH, 28, r28, R28, 29, r29, R29
    .ifc \ptr,\str
        POP   YL
        POP   YH
    .endif
    .exitm
  .endr
  .irp str z, Z
  #, zl, zL, Zl, ZL, zh, zH, Zh, ZH, 30, r30, R30, 31, r31, R31
    .ifc \ptr,\str
        POP   ZL
        POP   ZH
    .endif
    .exitm
  .endr
  _macro_popp_e\@:
.endm
/******************************************************************************/
.macro  _stack_alloc_ls byte_c:req, reg_p=28, rhtmp=r24
        // allocate local storage
  _macro_stack_alloc_ls_s\@:
        PUSH \reg_p+1         // push YH first
        PUSH \reg_p           // push YL
        _mcp SPL,   \reg_p    // 28=YL, get stack pointer
        _mcp SPH,   \reg_p+1
        SBIW \reg_p,\byte_c   // prepare to reserve byte_c x bytes local storage
        _mcp SREG,  \rhtmp    // store interrupt flag state
        CLI
        _mcp \reg_p,  SPL     // establish new top of stack
        _mcp \reg_p+1,SPH
        _mcp \rhtmp,  SREG    // restore previous interrupt state
  _macro_stack_alloc_ls_e\@:
.endm
/*----------------------------------------------------------------------------*/
.macro  _stack_free_ls byte_c:req, reg_p=28, rhtmp=r24
        // clean up stack frame, restore reg and return
  _stack_macro_free_ls_s\@:
        _mcp SREG,  \rhtmp
        CLI
        ADIW \reg_p,  \byte_c // calc end of local storage
        _mcp \reg_p,  SPL     // restore stack pointer
        _mcp \reg_p+1,SPH
        _mcp \rhtmp,  SREG
        POP  \reg_p           // restore Y
        POP  \reg_p+1
        //RET                   // return to caller (must remove arg)
  _stack_macro_free_ls_e\@:
.endm
/******************************************************************************/
.macro  _ret_pop_z
  _macro_ret_pop_z_start\@:
        POP ZH // high byte of return addr
        POP ZL // low  byte of return addr
  _macro_ret_pop_z_end\@:
.endm
/*----------------------------------------------------------------------------*/
.macro  _ret_val_push_hl reg_list:vararg
  _macro_ret_val_push_hl_start\@:
        .irp reg reg_list
            PUSH reg
        .endr
  _macro_ret_val_push_hl_end\@:
.endm
/*----------------------------------------------------------------------------*/
.macro  _ret 
# before _ret, need pop ret addr into Z
  _macro_ret_s_\@:
  #= PUSH Z, RET
        IJMP 
  _macro_ret_e_\@:
.endm
/******************************************************************************/
.macro  _push_sreg _rtmp=r24
        _macro_push_sreg_s_\@:
        IN   \_rtmp, SREG
        PUSH \_rthmp
        _macro_push_sreg_e_\@:
.endm
.macro  _pop_sreg _rtmp=r24
        _macro_pop_sreg_s_\@:
        POP  \_rthmp
        OUT  SERG, \_rtmp
        _macro_pop_sreg_e_\@:
.endm


