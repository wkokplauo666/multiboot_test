ENTRY(loader)

SECTIONS {
	. = 0x100000;

	.text ALIGN (0x1000) :
	{
        . = ALIGN (8);
        KEEP(*(.multiboot))
		*(.text)
	}

	.rodata ALIGN (0x1000) :
	{
		*(.rodata*)
	}

	.data ALIGN (16) :
	{
		*(.data)
	}

	.bss ALIGN (16) :
	{
		*(COMMON)
		*(.bss)
	}
}
