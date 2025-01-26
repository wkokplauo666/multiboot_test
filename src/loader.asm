global loader
extern kmain

MAGIC equ 0xE85250D6
ARCHITECTURE equ 0x00
HEAD_LENGTH equ hend - hstart
CHECKSUM equ -(MAGIC + ARCHITECTURE + HEAD_LENGTH)

KERNEL_STACK_SIZE equ 4096  ; 4KB

section .text.multiboot align=8
hstart:
	dd MAGIC     
    dd ARCHITECTURE                  
    dd HEAD_LENGTH
    dd CHECKSUM

align_mod:
	dw 6
	dw 1
	dd 8

end_s:
	dd 0
	dd end_e - end_s
end_e:
hend:
	
section .text align=4

gdt_start:
	dq 0
.code:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 0b10011010
	db 0b11001111
	db 0x00
.data:
	dw 0xffff
	dw 0x0000
	db 0x00
	db 0b10010010
	db 0b11001111
	db 0x00
gdt_end:

gdt_desc:
	dw gdt_end - gdt_start - 1;
	dd gdt_start

flush_sr:
	mov ax, gdt_start.data - gdt_start
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	call kmain

	mov eax, 0xdeadbeef
	jmp loader.loop

loader:
	mov ebp, kernel_stack + KERNEL_STACK_SIZE
	mov esp, ebp
	push 0
	popf

	cli
	lgdt [gdt_desc]
	jmp 0x8:flush_sr

.loop:
	jmp .loop

section .bss align=16
kernel_stack:
	resb KERNEL_STACK_SIZE
