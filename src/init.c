#include "init.h"

static const char *multiboot_messages[] = {"End", "Command line", "Bootloader name", "Module", "Meminfo", 
"Boot device", "Memory map", "VESA BIOS extension", "Framebuffer", "Elf sections", "APM Table", 
"EFI32 System table pointer", "EFI64 System table pointer", "SMDBIOS Table", "old ACPI", "new ACPI",
"Networking information", "EFI Memory map", "EFI ExitBootServices not terminated", "EFI32 image handler pointer",
"EFI64 Image Handle Pointer", "Image load base address"};

const char *get_message(int type) {
    if (type < 0 || type > 21) {
        return "Invalid multiboot2 type";
    }
    return multiboot_messages[type];
}

