.macro  _mem_pool_data_init _ram_start:req, _blk_cnt_all:req, _blk_size:req
  _macro_mem_pool_data_init_s_\@:
        .section .data
        #.org  RAMSTART
        .set __mem_pool_meta_size, 8
        .org \_ram_start
        _mem_pool_meta:
        _mem_pool_blk_cnt_all:      .byte \_blk_cnt_all
        _mem_pool_blk_size:         .byte \_blk_size
        _mem_pool_blk_cnt_free:     .byte \_blk_cnt_all
        _mem_pool_blk_cnt_inited:   .byte 0
        _mem_pool_addr_header:      .word 0
        _mem_pool_addr_next:        .word 0
  _macro_mem_pool_data_init_e_\@:
.endm

