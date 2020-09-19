global long_add

long_add:
  push ebp
  mov ebp, esp

  mov eax, [ebp+20]
  add eax, [ebp+16]
  add eax, [ebp+12]
  add eax, [ebp+8]

  mov esp, ebp
  pop ebp
  ret
