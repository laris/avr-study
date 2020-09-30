;-------------------------------------------------------------------------------
; https://www.avrfreaks.net/forum/cascade-two-8-bit-timers-make-16bit-timer
; https://www.avrfreaks.net/comment/2228706#comment-2228706
;-------------------------------------------------------------------------------
; I know this is a very old thread, but i just had to do the same, 
; and why not put it here for reference?
; I realized that the 8 bit timer can be used as a 16 bit timer in CTC mode in another way. 
; I think that this method has some advantages, tell me what you think:
; Let's suppose the 16 bit count is 548, so first divide them in high and low byte, 
; let's call them OCR2A_H, and OCR2A_L. 
; They will hold the values OCR2A_H = 2, and OCR2A_L = 36.
; Then, you just load the low byte OCR2A_L into the actual OCR2A of Timer 2. 
; The timer must be in normal mode, not CTC mode, and activate the compare match interrupt. 
; The value of OCR2A is never changed with this method.
; You also need a variable to hold the high 8 bits of the counter, let's call it TCNT2_H.
; The first time the ISR is called, the total count is 36.
; The  next time the ISR is called, 256 timer tics passed since the last call 
; (as we didn't change the value of OCR2A, and the timer is in normal mode). 
; So now the total count is 36+256 = 292.
; The 3rd time the ISR is called, 256 timer tics passed since the last call. 
; So now the total count is 36+2*256 = 548, our goal.
; For this to work, you need to use a flag or some way to recognize 
; when is the first call to the ISR since the last reset, 
; as you don't need to increment TCNT2_H in that moment, 
; but you need to increment it at the start of the ISR at all the other times.
; You also need to check if TCNT2_H is exactly one value less than your goal, 
; then you activate CTC mode (you still have nearly 256 timer tics to reach your goal, 
; so there's plenty of time to activate it, so the goal won't be missed).
; And of course, you need to check if TCNT2_H is >= than your goal to reset TCNT2_H, 
; put the timer back into normal mode, 
; re-set your flag to check the first "round" of the ISR, etc.
; If OCR2A_H is 0, then the CTC mode must be active all the time.
; Probably there is some room for trouble 
; if the prescaler is low and OCR2A is a very low value, 
; as the timer could get reset again before you have the chance to deactivate CTC mode, 
; but i think that such a situation always causes some trouble 
; when trying to use an 8 bit timer as 16 bit, right?
;-------------------------------------------------------------------------------
; For clarity, what i'm currently doing right now is this:
; 16 bit count is 548 (HIGH_TOPCOUNT = 2, LOW_TOPCOUNT = 36)
; Initialization:
; CLEAR HIGH_COUNTER
; OCRnA = LOW_TOPCOUNT
; TIMER MODE = NORMAL
; TIMER COMPARE MATCH INTERRUPT ENABLED
; START TIMER
; Compare Match ISR:
;   DISABLE CTC MODE (IF ENABLED)
;   CHECK TIMER OVERFLOW_FLAG. 
;   IF OVF SET
;     INCREMENT HIGH_COUNTER
;     CLR OVF
;   ENDIF
;   IF HIGH_COUNTER = (HIGH_TOPCOUNT - 1) 
;       SET TIMER MODE = CTC
;   ELSE IF HIGH_COUNTER >= (HIGH_TOPCOUNT) THEN
;     {
;       CLEAR HIGH_COUNTER
;       DO STUFF
;     }
;   ELSE SKIP ISR
;-------------------------------------------------------------------------------
; My 16 bit top count is a fixed value, so i already know that it's always
; going to be larger than 8 bits. That simplifies things a little. 
; If there's a chance that the top count is lower than 256, some additional 
; check would need to be done in the initialization, at the very least, 
; and probably also on the ISR.
;-------------------------------------------------------------------------------
Timer2Comp:
; Here I make some counts with variables to use the timer as if it were 16 bits.
; This is to save the Status Register without altering it 
; (DO NOT USE the r0, or push it too).
push r0
load r0, SREG
; The timer must normally be in Normal mode (NO CTC). Only during the last "lap"
push xl
load xl, TCCR2A
; CTC mode is activated, and it is crucial that it be deactivated 
; as quickly as possible. This is due to
cbr xl, (1 << WGM21) ; --> off CTC
store TCCR2A, xl
; If the value of OCR2A is low, the timer may reset more than once until
; You have the opportunity to disable CTC mode.
push xh
load xl, TCNT2_H
; If the Overflow bit is marked ...
sbis TIFR2, TOV2    ; --> check OV, if OV, TCNT_H++, 
rjmp T2C_NoOverf    ; -->           no OV, jmp to noOV
; NOTE: THERE IS A BUG IN THE AVR STUDIO 4 SIMULATOR 1, WHICH MAKES IT LOOK LIKE
; THAT THE TOV2 IS NOT SET IN CTC MODE, BUT IN THE REAL AVR IT WORKS OK. 
; IT ALSO WORKS OK IN THE SIMULATOR 2.
; ... increase my "high counter" ...
inc xl              ; --> TCNT_H++
; I delete the overflow flag (yes, it is done by writing a 1. Rare, isn't it?)
sbi TIFR2, TOV2     ; --> clr TOV
; ... and I keep the counter high again in the variable.
store TCNT2_H, xl   ; --> write back TCNT_H
T2C_NoOverf:        ; --> NoOV
load xh, OCR2A_H    ; --> check OCR_H
dec xh              ; --> OCR_H/CTC_TOP -1
; If 1 high count is missing by the end, now the final "lap" begins.
cp xl, xh           ; --> if TCNT_H == TOP_H-1, on CTC, FinalRnd
; (NOTE: and in the way this routine counts, I am exactly 256 tics from the end)
breq T2C_FinalRnd   ; 
inc xh              ; --> else, reset OCR_H/CTC_TOP
; If I reached the full account, jump to Timer2Comp_FullCnt.
cp xl, xh
brsh Timer2Comp_FullCnt
; If none of that matches, I just leave.
rjmp Timer2Comp_Fin
T2C_FinalRnd:
; Being the last "lap", activate CTC mode, when reach the value of OCR2A
load xl, TCCR2A
; the timer resets automatically, counting exactly to the required 16-bit value
sbr xl, (1 << WGM21)
; (and theoretically it would work with ANY value of 16 bits).
store TCCR2A, xl
rjmp Timer2Comp_Fin
Timer2Comp_FullCnt:
; If I get here, I counted to the end.
; I reset my high counter to 0.
clr xl
store TCNT2_H, xl
Timer2Comp_Fin:
pop xh
pop xl
; I restore the Status Register.
store SREG, r0
pop r0
reti
