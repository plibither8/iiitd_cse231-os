;; Name: Mihir Chaturvedi
;; Roll no.: 2019061

[bits 16]		; 16-bit Real Mode commands
[org 0x7c00]	; Starting address of the code

real_mode_boot:
	;; Initialise VGA configs
	call init_vga

	;; Switch to protected mode
	call switch_to_pm

	;; We jump straight to the 32 bit, protected mode
	;; of the code and start to run 32-bit instructions
	jmp CODE_SEG:init_pm

;; Here we set the inital value of the
;; AX register (to be printed on screen) as 0
;; and then set the VGA mode to 3 by doing a
;; boot interrupt call
init_vga:
	mov ax, 0x3		; set initial value of AX to 3
	int 0x10		; make the interrup call
	cli				; clear the interrupt's flag value

	ret

;; Operations to switch to protected mode
;; include gdt and cr0 changes
switch_to_pm:
	;; Load the global descriptor table
	lgdt [gdt_pointer]

	;; Enable Protected Mode by setting the
	;; protected mode bit in CR0 register
	mov eax, cr0	; Move current value of CR0 register into EAX for OR'ing
	or eax, 1		; OR the EAX register with 1 (to set protected mode bit)
	mov cr0, eax	; Move contents back into CR0 register

	ret

;; GDT address descriptions
gdt_start:
	dq 0x0
gdt_code:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10011010b
	db 11001111b
	db 0x0
gdt_data:
	dw 0xFFFF
	dw 0x0
	db 0x0
	db 10010010b
	db 11001111b
	db 0x0
gdt_end:
gdt_pointer:
	dw gdt_end - gdt_start
	dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

;; 32-bit Protected Mode instructions
[bits 32]

;; Constants
VID_MEM_OFFSET equ 0xb8000
COLOR_WHITE equ 0x0f00
SPACE_CHAR equ 0x20
DIG_OFFSET equ 0x30

;; Start off the protected mode stuff
init_pm:
	;; Store the message (Hello world) inside the ESI register
	;; and set the starting address to individually store these
	;; characters in EBX@0xB8000 <-- This is where text screen video
	;; memory resides initally
	mov esi, message
	mov ebx, VID_MEM_OFFSET

;; Loop that prints contents of ESI register character-by-character
print_hello_loop:
	lodsb					; Load current value of ESI into AX
	or al, al				; Check whether it is nullish
	jz print_cr0			; If yes, move on to printing CR0 content

	or eax, COLOR_WHITE 	; make the text color white
	mov word [ebx], ax		; Move to video memory address to print
	add ebx, 2				; Add 2 to point to new address

	jmp print_hello_loop	; Loop again

;; Setup to print CR0 register
print_cr0:
	mov ax, SPACE_CHAR	; Store a space char in AX
	mov [ebx], eax		; Print the space
	add ebx, 2			; Add 2 for next char

	mov ecx, 0			; Initialise iterator variable
	mov edx, cr0		; Copy CR0 content into EDX

;; Main loop to print CR0
print_cr0_loop:
	rol edx, 1			; rotate-shift cr0 content by 1

	mov eax, edx		; copy shifted value to EAX for printing
	and eax, 1			; and with 1 to get 0 or 1
	add ax, DIG_OFFSET	; add 48 to convert to ascii
	or eax, COLOR_WHITE	; add white color
	mov [ebx], eax		; Move to video memory address to print
	add ebx, 2			; Add 2 to point to new address

	add ecx, 1			; 1 to iterator var
	cmp ecx, 0x20		; Compare with 32 (CR0 is 32-bit)
	jl print_cr0_loop	; Loop if less than

;; Halt the program
halt:
	cli
	hlt

;; The string we want to print out
;; stored in the data section
message: db "Hello, world!", 0

;; Fill the remaining sector with 0's
;; '$' indicates assembly position at the beginning of the line containing the expression
;; '$$' indicates position beginning at current section
times 510 - ($-$$) db 0

;; Magic number: indicates that the end of the bootloader section
;; and marks the sector as bootable
dw 0xAA55
