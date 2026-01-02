global _start

section .data

message db "Hello Server!", 0
msglen db $ - message

sockaddr_in:
af	dw 2		; address family INET
port	dw 0x901F	; port 8080 in network-byte order
addr_in	db 192, 168, 0, 13 ; my PC's local IP
zeros	dq 0x0 		; trailing zero padding to 16 bytes

section .text

_start:
mov	rax, 41 ; socket()
mov	rdi, 2  ; AF_INET
mov	rsi, 1  ; SOCK_STREAM
mov	rdx, 0  ; N/A
syscall

mov	r14, rax

cmp rax, -1
je socket_failed

sub	rsp, 16
mov	r12, rsp
movups	xmm0, [sockaddr_in]
movups	[r12], xmm0
;created sockaddr struct

mov	rax, 42  ; connect()
mov	rdi, r14 ; socket fd
mov	rsi, r12 ; pointer to sockaddr struct - was r12
mov	rdx, 16  ; size of sockaddr struct
syscall

cmp	rax, 0
jne	conn_failed

xor	rcx, rcx
mov	cl, byte [msglen]
sub	rsp, rcx
mov	r13, rsp
lea	rsi, [message]
lea	rdi, [r13]
rep	movsb
mov	cl, [msglen]
;creating a buffer on the stack and copying the message to it

mov	rax, 1  ; write()
mov	rdi, r14 ; fd
mov	rsi, r13 ; buf pointer - was r13
mov	rdx, rcx  ; count
syscall

add	rsp, rcx

conn_failed:
add	rsp, 21
mov	rax, 3  ; close()
mov	rdi, r14; socket fd
syscall

socket_failed:

mov	rax, 60 ;exit()
xor	rdi, rdi
syscall
