extern elfgen
global main

section .bss
	asm: resb 2048 ; an arbitary size, 2048 bytes is more than enough for now 
	len: resd 1

section .text
main:
	enter 0, 0 ; the function prologue
	
	mov [rel asm + 0], byte 0xb8 ; (1)
	mov [rel asm + 1], dword 0x45
	mov [rel asm + 5], byte 0xc3
	add [rel len], byte 6 ; (2)

	lea rdi, [rel asm]
	mov rsi, [rel len]
	call elfgen ; call elfgen

	leave ; function epilogue
	ret