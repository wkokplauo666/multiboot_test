SOURCES = $(shell find src -name "*.c" -o -name "*.asm")
OBJECTS = $(patsubst src/%.c, build/%.o, $(patsubst src/%.asm, build/%.o, $(SOURCES)))
DEPS = $(OBJECTS:.o=.d)

CC = i386-elf-gcc
CFLAGS = -O0 -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c -g -fno-omit-frame-pointer -MMD -ffreestanding

LD = i386-elf-ld
LDFLAGS = -T env/link.ld -melf_i386

GDB = i386-elf-gdb
GDBFLAGS = -ex "target remote localhost:1234" -ex "symbol-file build/kernel.elf" -ex "add-symbol-file build/kernel.elf 0x100000"

.PHONY: all clean run runbios debug debugbios

all: out.iso

build/kernel.elf: $(OBJECTS) | build
	$(LD) $(LDFLAGS) $^ -o $@

build/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

build/%.o: src/%.asm
	mkdir -p $(dir $@)
	nasm -f elf $< -o $@

build:
	mkdir -p build

out.iso: build/kernel.elf
	cp $< iso/boot/
	grub-mkrescue -o out.iso iso/

run: out.iso
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd -cdrom out.iso

runbios: out.iso
	qemu-system-x86_64 -cdrom out.iso

debug: out.iso
	qemu-system-x86_64 -drive if=pflash,format=raw,readonly=on,file=/usr/share/OVMF/OVMF_CODE.fd -s -cdrom out.iso -d guest_errors,int &
	$(GDB) $(GDBFLAGS)

debugbios: out.iso
	qemu-system-x86_64 -cdrom out.iso -s -d guest_errors,int &
	$(GDB) $(GDBFLAGS)

clean:
	rm -rf build/* out.iso iso/boot/kernel.elf

-include $(DEPS)
