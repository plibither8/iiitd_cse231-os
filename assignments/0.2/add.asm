	global	add

	section	.text
add:	mov	rax, rdi	; store value of first argument (rdi) in rax
	add	rax, rsi	; rax = rax + rsi
	ret			; return rax
