SOURCES = $(shell find src -name "*.c" -o -name "*.asm")
# Generate object files for both .c and .asm sources
OBJECTS = $(patsubst src/%.c, build/%.o, $(patsubst src/%.asm, build/%.o, $(SOURCES)))


CC = i386-elf-gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -g

LD = i386-elf-ld
LDFLAGS = -T env/link.ld -melf_i386

all: out.iso

build/kernel.elf: $(OBJECTS)
	$(LD) $(LDFLAGS) $^ -o $@

build/%.o: src/%.c
	$(CC) $(CFLAGS) $< -o $@

build/%.o: src/%.asm
	nasm -f elf $< -o $@

out.iso: build/kernel.elf
	cp $< iso/boot/
	grub-mkrescue -o out.iso iso/

run: out.iso
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd -cdrom out.iso

runbios: out.iso
	qemu-system-x86_64 -cdrom out.iso

debug: out.iso
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd -s -cdrom out.iso -d guest_errors,int &
	gdb -ex "target remote localhost:1234" -ex "symbol-file build/kernel.elf"

debugbios: out.iso
	qemu-system-x86_64 -cdrom out.iso -s -d guest_errors,int &
	gdb -ex "target remote localhost:1234" -ex "symbol-file build/kernel.elf"

clean:
	rm -rf build/*