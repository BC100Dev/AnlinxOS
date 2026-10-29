.section .text
.global _start
.type _start, @function

_start:
    xor     %rbp, %rbp
    mov     (%rsp), %rdi
    lea     8(%rsp), %rsi
    lea     8(%rsi,%rdi,8), %rdx
    and     $~15, %rsp
    call    __libc_start_main
    ud2