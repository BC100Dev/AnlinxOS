.section .text
.global _start
.type _start, @function

_start:
    mov     x29, #0
    mov     x30, #0
    ldr     x0, [sp]
    add     x1, sp, #8
    add     x2, x1, x0, lsl #3
    add     x2, x2, #8
    and     sp, sp, #~15
    bl      __libc_start_main
    brk     #0