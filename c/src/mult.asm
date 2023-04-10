
%include "./include/asm_io.inc"

segment .data

segment .bss

segment .text

        global asm_mult

asm_mult:

        enter           4,0
        
        mov             dword [ebp-4], 0
        mov             eax, [ebp+8]
        mov             ebx, [ebp+12]
        imul            ebx, eax
        mov             [ebp-4], ebx
        mov             eax, ebx
        dump_stack      1, 1, 3

        leave        
        ret