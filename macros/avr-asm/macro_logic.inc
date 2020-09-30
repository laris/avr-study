
.macro  _bit_not_gpr _reg:req, _bit:req
        _macro_bit_not_gpr_s_\@:
        COM \_reg
        BST \_reg, \_bit
        COM \_reg
        BLD \_reg, \_bit
        _macro_bit_not_gpr_e_\@:
.endm
.macro  _bit_not _xaddr:req, _bit:req, _rtmp=r24
/* _bit from GPR to ram */
        _macro_bit_not_s_\@:
        _get_op \_xaddr
        __mem_type_\@ = __mem_type
        .if (__mem_type_\@ == __MEM_TYPE_GPR)
            _bit_not_gpr \_xaddr, \_bit
        .else
            _mcp \_xaddr, \_rtmp
            _bit_not_gpr  \_rtmp, \_bit
            _mcp \_rtmp,  \_xaddr
        .endif
        _macro_bit_not_e_\@:
.endm

.macro  _mem_not _xaddr:req, _rtmp=r24
        _macro_not_s_\@:
        _mem_opcode_1x_addr COM, \_xaddr, \_rtmp
        _macro_not_e_\@:
.endm
