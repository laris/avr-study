/*
#define __AVR_ATtiny85__
##define __AVR_ATmega328P__
#define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
#include <avr/io.h>
#include "avr_macro.S"

#define __GPIO_PULLUP_ON  0
#define __GPIO_PULLUP_OFF 1
#define __REG_MCUCR MCUCR
#define __BIT_PUD   PUD
*/
.set __GPIO_PULLUP_ON,  0
.set __GPIO_PULLUP_OFF, 1
.set __REG_MCUCR,       MCUCR
.set __BIT_PUD,         PUD

/*
_gpio_pullup_bit_set_on
_gpio_pullup_bit_set_off
_gpio_pullup_bit_get    copy bit to T
_gpio_chnl_reg_mod_set   _ddrx, _ddrx_val
_gpio_chnl_reg_mod_get  _ddrx, _ret_xaddr
_gpio_chnl_reg_out  _portx,_portx_value
_gpio_chnl_reg_out_get _portx,_ret_xaddr
_gpio_chnl_reg_in_set   _pinx, _pinx_value
_gpio_chnl_reg_in_get  _pinx, _ret_xaddr
_gpio_chnl_reg_mod_bit_set_on _ddrx, _xpin
_gpio_chnl_reg_mod_bit_set_off  _ddrx, _xpin
_gpio_chnl_reg_out_bit_set_on    _portx,_xpin
_gpio_chnl_reg_out_bit_set_off    _portx,_xpin
_gpio_chnl_bit_toggle _pinx,_xpin
_gpio_chnl_reg_in_bit_get     _pinx, _xpin
 */

/******************************************************************************/.macro  _gpio_pullup_bit_set_on
        _macro_gpio_pullup_bit_set_on_s_\@:
        _bwrir __GPIO_PULLUP_ON, __REG_MCUCR, __BIT_PUD
        _macro_gpio_pullup_bit_set_on_e_\@:
.endm
.macro  _gpio_pullup_bit_set_off
        _macro_gpio_pullup_bit_set_off_s_\@:
        _bwrir __GPIO_PULLUP_OFF, __REG_MCUCR, __BIT_PUD
        _macro_gpio_pullup_bit_set_off_e_\@:
.endm
.macro  _gpio_pullup_bit_wr _bit_val:req
        _macro_gpio_pullup_bit_wr_s_\@:
        _get_bin _bit_val
        .if     (__binary_value == 0)
          _gpio_pullup_bit_set_off
        .elseif (__binary_value == 1)
          _gpio_pullup_bit_set_on
        .endif
        _macro_gpio_pullup_bit_wr_e_\@:
.endm
.macro  _gpio_pullup_bit_toggle
        _macro_gpio_pullup_bit_toggle_s_\@:
        _bit_not __REG_MCUCR, __BIT_PUD
        _macro_gpio_pullup_bit_toggle_e_\@:
.endm
.macro  _gpio_pullup_bit_get
/* get status and store into SREG_T */
        _macro_gpio_pullup_bit_get_s_\@:
        _bcprt __REG_MCUCR, __BIT_PUD
        _macro_gpio_pullup_bit_get_e_\@:
.endm
/******************************************************************************/
.macro  _gpio_chnl_reg_mod_set _ddrx:req, _ddrx_val:req
        _macro_gpio_chnl_reg_mod_set_s_\@:
        _mwri \_ddrx_val, \_ddrx
        _macro_gpio_chnl_reg_mod_set_e_\@:
.endm
.macro  _gpio_chnl_reg_mod_get _ddrx:req, _ret_xaddr:req
        _macro_gpio_chnl_reg_mod_get_s_\@:
        _mcp \_ddrx, \_ret_xaddr
        _macro_gpio_chnl_reg_mod_get_e_\@:
.endm
.macro  _gpio_chnl_reg_mod_bit_set_on _ddrx:req, _xpin:req
        _macro_gpio_chnl_reg_mod_bit_set_on_s_\@:
        _bwrir 1, \_ddrx, \_xpin
        _macro_gpio_chnl_reg_mod_bit_set_on_e_\@:
.endm
.macro  _gpio_chnl_reg_mod_bit_set_off _ddrx:req, _xpin:req
        _macro_gpio_chnl_reg_mod_bit_set_off_s_\@:
        _bwrir 0, \_ddrx, \_xpin
        _macro_gpio_chnl_reg_mod_bit_set_off_e_\@:
.endm
.macro  _gpio_chnl_reg_mod_bit_wr _bit_val:req
        _macro_gpio_chnl_reg_mod_bit_wr_s_\@:
        _get_bin _bit_val
        .if     (__binary_value == 0)
          _gpio_chnl_reg_mod_bit_set_off
        .elseif (__binary_value == 1)
          _gpio_chnl_reg_mod_bit_set_on
        .endif
        _macro_gpio_chnl_reg_mod_bit_wr_e_\@:
.endm
.macro  _gpio_chnl_reg_mod_bit_toggle _ddrx:req, _xpin:req
        _macro_gpio_chnl_reg_mod_bit_toggle_s_\@:
        _bit_not \_ddrx, \_xpin
        _macro_gpio_chnl_reg_mod_bit_toggle_e_\@:
.endm
.macro  _gpio_chnl_reg_mod_bit_get _ddrx:req, _xpin:req
/* get status and store into SREG_T */
        _macro_gpio_chnl_reg_mod_bit_get_s_\@:
        _bcprt \_ddrx, \_xpin
        _macro_gpio_chnl_reg_mod_bit_get_e_\@:
.endm
/******************************************************************************/
.macro  _gpio_chnl_reg_out_set _portx:req, _portx_value:req
        _macro_gpio_chnl_reg_out_set_s_\@:
        _mwri \_portx_value, \_portx
        _macro_gpio_chnl_reg_out_set_e_\@:
.endm
.macro  _gpio_chnl_reg_out_get _portx:req, _ret_xaddr:req
  _macro_gpio_chnl_reg_out_get_s_\@:
        _mcp \_portx, \_ret_xaddr
  _macro_gpio_chnl_reg_out_get_e_\@:
.endm
.macro  _gpio_chnl_reg_out_bit_set_on _portx:req, _xpin:req
  _macro_gpio_chnl_reg_out_bit_set_on_s_\@:
        _bwrir 1, \_portx, \_xpin
  _macro_gpio_chnl_reg_out_bit_set_on_e_\@:
.endm
.macro  _gpio_chnl_reg_out_bit_set_off _portx:req, _xpin:req
  _macro_gpio_chnl_reg_out_bit_set_off_s_\@:
        _bwrir 0, \_portx, \_xpin
  _macro_gpio_chnl_reg_out_bit_set_off_e_\@:
.endm
.macro  _gpio_chnl_reg_out_bit_wr _bit_val:req
  _macro_gpio_chnl_reg_out_bit_wr_s_\@:
  _get_bin _bit_val
  .if     (__binary_value == 0)
    _gpio_chnl_reg_out_bit_set_off
  .elseif (__binary_value == 1)
    _gpio_chnl_reg_out_bit_set_on
  .endif
  _macro_gpio_chnl_reg_out_bit_wr_e_\@:
.endm
.macro  _gpio_chnl_reg_out_bit_toggle _portx:req, _xpin:req
  _macro_gpio_chnl_reg_out_bit_toggle_s_\@:
        _bit_not \_portx, \_xpin
  _macro_gpio_chnl_reg_out_bit_toggle_e_\@:
.endm
.macro  _gpio_chnl_reg_out_bit_get _ddrx:req, _xpin:req
/* get status and store into SREG_T */
        _macro_gpio_chnl_reg_out_bit_get_s_\@:
        _bcprt \_ddrx, \_xpin
        _macro_gpio_chnl_reg_out_bit_get_e_\@:
.endm
/******************************************************************************/
.macro  _gpio_chnl_reg_in_set _pinx:req, _pinx_value:req
  _macro_gpio_chnl_reg_in_set_s_\@:
        _mwri \_pinx_value, \_pinx
  _macro_gpio_chnl_reg_in_set_e_\@:
.endm
.macro  _gpio_chnl_reg_in_get _pinx:req, _ret_xaddr:req
  _macro_gpio_chnl_reg_in_get_s_\@:
        _mcp \_pinx, \_ret_xaddr
  _macro_gpio_chnl_reg_in_get_e_\@:
.endm
.macro  _gpio_chnl_reg_in_bit_set_on _pinx:req, _xpin:req
  _macro_gpio_chnl_reg_in_bit_set_on_s_\@:
        _bwrir 1, \_pinx, \_xpin
  _macro_gpio_chnl_reg_in_bit_set_on_e_\@:
.endm
.macro  _gpio_chnl_reg_in_bit_set_off _pinx:req, _xpin:req
  _macro_gpio_chnl_reg_in_bit_set_off_s_\@:
        _bwrir 0, \_pinx, \_xpin
  _macro_gpio_chnl_reg_in_bit_set_off_e_\@:
.endm
.macro  _gpio_chnl_reg_in_bit_wr _bit_val:req
  _macro_gpio_chnl_reg_in_bit_wr_s_\@:
  _get_bin _bit_val
  .if     (__binary_value == 0)
    _gpio_chnl_reg_in_bit_set_off
  .elseif (__binary_value == 1)
    _gpio_chnl_reg_in_bit_set_on
  .endif
  _macro_gpio_chnl_reg_in_bit_wr_e_\@:
.endm
.macro  _gpio_chnl_reg_in_bit_toggle _pinx:req, _xpin:req
  _macro_gpio_chnl_reg_in_bit_toggle_s_\@:
  _bit_not \_pinx, \_xpin
  _macro_gpio_chnl_reg_in_bit_toggle_e_\@:
.endm
.macro  _gpio_chnl_reg_in_bit_get _pinx:req, _xpin:req
/* get status and store into SREG_T */
  _macro_gpio_chnl_reg_in_bit_get_s_\@:
  _bcprt \_pinx, \_xpin
  _macro_gpio_chnl_reg_in_bit_get_e_\@:
.endm
/******************************************************************************/
.macro  _gpio_chnl_byte_toggle _pinx:req
  _macro_gpio_chnl_byte_toggle_s_\@:
  _mwri 0xFF, _pinx
  _macro_gpio_chnl_byte_toggle_e_\@:
.endm
.macro  _gpio_chnl_bit_toggle _pinx:req, _xpin:req
  _macro_gpio_chnl_bit_toggle_s_\@:
  _bwrir 1, \_pinx, \_xpin
  _macro_gpio_chnl_bit_toggle_e_\@:
.endm
