; https://gist.github.com/gamaral/3773527
; gamaral/delay_ms.inc.asm
;*****************************************************************************
; Copyright 2012 Guillermo A. Amaral B. All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;
;   1. Redistributions of source code must retain the above copyright notice,
;      this list of conditions and the following disclaimer.
;
;   2. Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
;
; THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
; IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
; OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
; NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
; INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
; SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
; CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
; LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
; OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
; DAMAGE.
;*****************************************************************************
;
; DELAY_MS
;
; Loops N number of milliseconds, delay (N) must be stored in ZH, ZL before
; calling.
;
; Adjust R16_COUNT/R17_COUNT to your clock speed; Default is 1 MHz (8/8).
;
;*****************************************************************************
DELAY_MS:                     ; rcall 3
	; 128 KHz
	;.EQU R16_COUNT = 2
	;.EQU R17_COUNT = 20

	; 1 MHz (default for AVR)
	;.EQU R16_COUNT = 10
	;.EQU R17_COUNT = 32

	; 8 MHz
	;.EQU R16_COUNT = 80
	;.EQU R17_COUNT = 32

	; 16 MHz
	.EQU R16_COUNT = 160
	.EQU R17_COUNT = 32

	; 20 MHz
	;.EQU R16_COUNT = 200
	;.EQU R17_COUNT = 32

	push R16              ; 2
	push R17              ; 2

DELAY_LOOP:
	ldi R16, R16_COUNT    ; 1
DELAY_LOOP_A:
	ldi R17, R17_COUNT    ; 1
DELAY_LOOP_B:
	dec R17               ; 1
	brne DELAY_LOOP_B     ; 2 || 1
	nop                   ; 1

	dec R16               ; 1
	brne DELAY_LOOP_A     ; 2 || 1
	nop                   ; 1

	sbiw ZL, 1            ; 2
	brne DELAY_LOOP       ; 2 || 1

	pop R17               ; 2
	pop R16               ; 2

	ret                   ; 4

; vim:syn=asm:
