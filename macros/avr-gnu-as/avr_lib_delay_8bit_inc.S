.macro  _init_delay_8bit_cycle _reg_cnt:req, _reg_rmd:req
        ##define delayCNT_reg        \_reg_cnt
        .set delayCNT_reg,        \_reg_cnt
                //R24 //24:25, 26:27=X, 28:29=Y, 30:31=Z
        ##define delayNx4cycRmd_reg  \_reg_rmd
        .set delayNx4cycRmd_reg,  \_reg_rmd
                //R25 // Remainder
                //#define delayCNT_ram_addr_sram 0x100
        #define funcDelayNCycle8_OVERHEAD_reg  19
.endm
