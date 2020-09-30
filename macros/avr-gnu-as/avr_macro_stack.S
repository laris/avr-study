.macro  _init_sp
        #if defined(__DEBUG_MACSIM__)
        _mwrk hi8(0x07FE), SPH
        _mwrk lo8(0x07FE), SPL
        #else
        _mwrk hi8(RAMEND), SPH
        _mwrk lo8(RAMEND), SPL
        #endif
.endm
.macro  _push_p ptr:req
  _macro_push_p_s\@:
  .irp str x, X, xl, xL, Xl, XL, xh, xH, Xh, XH, 26, r26, R26, 27, r27, R27
    .ifc \ptr,\str
        PUSH XH
        PUSH XL
    .endif
    .exitm
  .endr
  .irp str y, Y, yl, yL, Yl, YL, yh, yH, Yh, YH, 28, r28, R28, 29, r29, R29
    .ifc \ptr,\str
        PUSH YH
        PUSH YL
    .endif
    .exitm
  .endr
  .irp str z, Z, zl, zL, Zl, ZL, zh, zH, Zh, ZH, 30, r30, R30, 31, r31, R31
    .ifc \ptr,\str
        PUSH ZH
        PUSH ZL
    .endif
    .exitm
  .endr
  _macro_push_p_e\@:
.endm
.macro  _pop_p ptr:req
  _macro_pop_p_s\@:
  .irp str x, X, xl, xL, Xl, XL, xh, xH, Xh, XH, 26, r26, R26, 27, r27, R27
    .ifc \ptr,\str
        POP   XL
        POP   XH
    .endif
    .exitm
  .endr
  .irp str y, Y, yl, yL, Yl, YL, yh, yH, Yh, YH, 28, r28, R28, 29, r29, R29
    .ifc \ptr,\str
        POP   YL
        POP   YH
    .endif
    .exitm
  .endr
  .irp str z, Z, zl, zL, Zl, ZL, zh, zH, Zh, ZH, 30, r30, R30, 31, r31, R31
    .ifc \ptr,\str
        POP   ZL
        POP   ZH
    .endif
    .exitm
  .endr
  _macro_pop_p_e\@:
.endm
.macro  __altmacro_pass_arg reg,input
       #LDI 16, 1<<TOV0|1<<TOV1
        LDI \reg,\input
.endm
.macro  __altmacro_pass_arg_tb
__altmacro_pass_arg r16,<1!<!<PB0>
__altmacro_pass_arg r16,<1!<!<PB0|1!<!<PB1>
__altmacro_pass_arg r16, 1&2
.endm
#__altmacro_pass_arg_tb

/******************************************************************************/
.macro  _stack_alloc_ls byte_c:req, reg_p=28, rhtmp=r16
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
.macro  _stack_free_ls byte_c:req, reg_p=28, rhtmp=r16
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
.macro  _ret_val_push_hl reg_list:vararg
  _macro_ret_val_push_hl_start\@:
        .irp reg reg_list
            PUSH reg
        .endr
  _macro_ret_val_push_hl_end\@:
.endm
.macro  _ret // before _ret, need pop ret addr into Z
  _macro_ret_start\@:
        IJMP // = PUSH Z, RET
  _macro_ret_end\@:
.endm
/******************************************************************************/

