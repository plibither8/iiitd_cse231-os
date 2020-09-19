section .text

global _start
_start:
  mov rdi, $1
  mov rsi, $2
  call checkGreater

global checkGreater
checkGreater:
  cmp rdi, rsi
  mov rax, 1
  mov rdi, 1
  mov rdx, NumLen
  jg write1

write0:
  mov rsi, Zero
  syscall
  jmp exit

write1:
  mov rsi, One
  syscall

exit:
  mov rax, 60
  mov rdi, 0
  syscall

section .rodata
  One: db "1", 10
  Zero: db "0", 10
  NumLen: equ $-Zero
