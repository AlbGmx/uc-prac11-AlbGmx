#define _SFR_ASM_COMPAT 1
#define __SFR_OFFSET 0
#include <avr/io.h>

;.global Delay  ;cycles
;Delay:		.global Delay
;   clr  r24      1
;Rnxt0:           xLoop
;   push r24      2x
;   clr r24       x
;Rnxt1:           yLoop
;   nop           xy
;   nop           xy
;   nop           xy
;   nop           xy
;   nop           xy
;   dec r24       xy
;	 brne Rnxt1    x(2y-1)
;	 pop r24       2x
;	 dec r24       x
;	 brne Rnxt0    (2x-1)
;	 pop r24       2
;	 ret           4 => possible bug as avr shows 9 cycles for ret
;Contemplating call from c to asm
;                 5
;
;Total:  2 + 1 + 2x + x + 6xy + x(2y-1) + 2x + x + 2x -1 + 11
;        8xy + 7x + 13, in this case r24 is clear, so it repeats 256 in each cicle
;        8xy + 7x + 13 when x,y = 256 gives us 526094 cycles
;        Total cycles: 526093 
;        
;        Time of function before modifications 526092cycles/16MHz = 32.881ms
;
;        If we need a n-ms delay, we need x amount of 16000 cycles

.global Delay
Delay:         ;Cycles used
                  ;5 call from C
   push r24       ;2
delayLoop:        ;loop for delay
   cpi r24,1      ;1
   breq delay1ms  ;2 if x=1, otherwise 1
   rcall delayAux ;3
   dec r24        ;1
   jmp delayLoop  ;3
end:
   pop r24        ;2
   ret            ;4

; Delay aprox 1ms should take 16000 - 16, the cycles used by only the function (not counting loop)
;  eq = 4xy + 3x + 4 = 15989 = > x = 68, y = 58
delay1ms:
	ldi r26,68   ;1
   nop          ;1
x1Loop: 	      ;loop for x
   ldi r27,58  ;x  
y1Loop:	      ;loop for y
   nop         ;xy
   dec r27     ;xy
	brne y1Loop  ;(2y-1)(x)
	dec r26      ;x
	brne x1Loop  ;(2x-1)
   jmp end      ; 3


; Auxiliar function, should take 16000 - 9, the cycles used by the loop 3xy + 3x + 4 = 15991 => x = 73, y = 72
delayAux:
	ldi r26,73   ;1
xLoop: 	      ;loop for x
   ldi r27,72 ;x  
yLoop:	      ;loop for y
   dec r27     ;xy
	brne yLoop  ;(2y-1)(x)
	dec r26     ;x
	brne xLoop  ;(2x-1)
   ret         ; 4
