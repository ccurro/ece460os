	.global _start

	.text
_start:
	# write(1, message, 13)
	mov     $1, %rax
	mov     $1, %rdi
	mov     $message, %rsi
	mov     $13, %rdx
	syscall

	# exit(10)
	mov     $60, %rax
	mov     $10, %rdi
	syscall
	message:
	.ascii  "Hello, world\n"
	