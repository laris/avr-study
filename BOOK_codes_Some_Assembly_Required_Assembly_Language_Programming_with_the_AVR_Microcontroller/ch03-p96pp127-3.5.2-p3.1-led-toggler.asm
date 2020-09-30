; PROGRAM 3.1 LED Toggler
; P96 PP127
; A simple AVR example to illustrate I/O using LED's and
; Pushbuttons
; Toggler - This program uses pushbuttons (PORTD) to
; toggle bits in a byte. The value of the byte is
; displayed on the LED's (PORTB)
; PORTB must be connected to the LEDS
; PORTD must be connected to the SWITCHES
; Programmer: TM
; Date: 5/2010
; Platform: STK-500
; Device: ATMega16A
.cseg ;select current segment as code
.org 0 ;begin assembling at address 0
.def leds = r16 ;current LED state
.def switches = r17 ; switch values just read
.def temp = r18 ;used as a temporary register
.equ PORTB = 0x18 ;Port B's output register
.equ DDRB = 0x17 ;Port B's Data Direction Register
.equ PIND = 0x10 ;Port D's input register
.equ DDRD = 0x11 ;Port D's Data Direction Register
ldi temp,0xFF ;configure PORTB as output
out DDRB,temp
clr temp ;configure PORTD as input
out DDRD,temp
ldi leds,0x00 ;Initialize LED's all on
out PORTB,leds ;Display initial LED's
;wait for switch to be pressed
;while (no button is depressed);
waitpress:
in switches, PIND
cpi switches, 0xFF ;0xFF means none pressed
breq waitpress
;one or more switches are depressed (0's)
com switches ;flip all bits, now 1's indicate ;pressed
eor leds,switches ;toggle associated bits in ;led status
out PORTB,leds ;(Re)display LED's
;wait for all switches to be released
;while (at least one button is depressed);
waitrelease:
in switches, PIND
cpi switches, 0xFF ;0xFF means none pressed
brne waitrelease
rjmp waitpress ;repeat (forever)