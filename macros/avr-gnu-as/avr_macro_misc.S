.macro  _clr_reg _reg:req
  _macro_clr_reg_s_\@:
        XOR \_reg, \_reg
  _macro_clr_reg_e_\@:
.endm

