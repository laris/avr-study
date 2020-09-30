##define __AVR_ATtiny85__
##define __AVR_ATmega328P__
##define __DEBUG_MACSIM__
##define __SFR_OFFSET 0x20
##include <avr/io.h>
##include "avr_macro.S"
##include "lib_gpio.S"

.include "macro_all.inc"
.include "lib_gpio.inc"
/*
_gpio_pullup_set_on
_gpio_pullup_set_off
_gpio_pullup_get_bit
*/
