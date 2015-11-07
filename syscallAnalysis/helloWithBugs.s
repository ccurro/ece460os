	.global _start

	.text
_start:
		# using invalid syscall no 31415 instead of 1 for write()
		mov     $31415, %rax 
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
