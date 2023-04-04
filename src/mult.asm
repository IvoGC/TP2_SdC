;
; file mult.asm
;

;
; Initialized data is put in the .data segment
;
segment .data

;
; Uninitialized data is put in the .bss segment
;
segment .bss

;
; Code is put in the .text segment
;   
; Definition of the function in C
;
;               float PRE_CDECL asm_mult(float, float) POST_CDECL;
;   
; Call of the function from C
;               
;               asm_mul(operator1, operator2)
;
; Stack frame
;               ebp+16 -> res
;               ebp+12 -> operator2
;               ebp+8 -> operator1    
;               ebp+4 -> return direction
;
segment .text

        global asm_mult
asm_mult:
                    
               
        mulss           xmm0, xmm1
        
        ret
