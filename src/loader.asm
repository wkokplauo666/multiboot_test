global loader
extern kmain

MAGIC equ 0xE85250D6
ARCHITECTURE equ 0x00
FLAGS equ 0x00
HEAD_LENGTH equ hend - hstart
CHECKSUM equ -(MAGIC + ARCHITECTURE + HEAD_LENGTH)

KERNEL_STACK_SIZE equ 4096  ; 4KB

section .text.multiboot align=8
hstart:
	dd MAGIC     
    dd ARCHITECTURE                  
    dd HEAD_LENGTH
    dd CHECKSUM

framebuffer_s:
	dw 5
	dw 1

	dd framebuffer_e - framebuffer_s

	dd 0
	dd 0
	dd 0
framebuffer_e:

	dd 0, 0,0 ;padding
end_s:
	dd 0
	dd end_e - end_s
end_e:
hend:
	
section .text align=4
loader:
	mov ebp, kernel_stack + KERNEL_STACK_SIZE
	mov esp, ebp

	push 0
	popf

	call kmain 
	
	mov eax, 0xdeadbeef
	;mov [0xb8000], ax
.loop:
	jmp .loop

section .bss align=16
kernel_stack:
	resb KERNEL_STACK_SIZE
