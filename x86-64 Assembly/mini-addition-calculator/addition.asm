global _start

section .data
msg db "Input numbers please: ",0xA , 0
msglen db $ - msg
msgerr db "Wrong number of bytes inputted!!!", 0xA, 0
errlen db $ - msgerr
inbuf db 0,0,0

section .text

nenough:

mov	rax, 1
mov	rdi, 1
mov	rsi, msgerr
xor	rdx, rdx
mov	dl, byte [errlen]
syscall
jmp	exit

_start:

mov	rax, 1
mov	rdi, 1
mov	rsi, msg
xor	rdx, rdx
mov	dl, byte [msglen]
syscall

mov	rax, 0
mov	rdi, 0
mov	rsi, inbuf
mov	rdx, 3
syscall

xor	rbx, rbx

check:
cmp	rax, 3
jl	nenough
cmp	[inbuf + rbx], byte '0'
jl	nenough
cmp	[inbuf + rbx], byte '9'
jg	nenough
add	rbx, 2
cmp	rbx, 2
jle	check

mov	al, byte [inbuf]
mov	ah, byte [inbuf + 2]
sub	al, '0'
sub	ah, '0'
add	al, ah
add	al, '0'
mov	[inbuf], al
mov	[inbuf + 1], byte 0xA


mov	rax, 1
mov	rdi, 1
mov	rsi, inbuf
xor	rdx, rdx
mov	dl, 2
syscall

exit:

mov	rax, 60
xor	rdi, rdi
syscall

