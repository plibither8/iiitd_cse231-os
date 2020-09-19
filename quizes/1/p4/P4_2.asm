global extended_add

extended_add:
  mov rax, rdi
  add rax, rsi
  add rax, rdx
  add rax, rcx

  ret
