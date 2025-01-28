#include "commands.h"

void print_command(int argc, char **argv) {
    if(argc == 2 && strcmp(argv[1], "vgatext") == 0) {
        for(int i = 0; i < 0xff; i++) {
            char tmp[] = {i, 0};
            printf2("%d. %af%s%as ", i, tmp);
        }
    } else if(argc == 3 && strcmp(argv[1], "char") == 0) {
        char c = atoi(argv[2]);
        char tmp[] = {c, 0};
        printf2("%s", tmp);
        return;
    } else if (argc > 1) {
        for(int i = 1; i < argc; i++) {
            printf("%s ", argv[i]);
        }
    }
}

void info_command(int argc, char **argv) {
    printf("So indeed we made this thing that can run on any x86 (BIOS-Supported) machines.\nThis is exactly the day where i am going to 'release' this thing's source code and show everyone how oh-god-so-good i am!!\n like i spent a whole week on this thing\n");
}

void timer_command(int argc, char **argv) {
    if(argc != 2) {
        printf("Usage: timer (freq)\n");
        return;
    }

    u32 freq = (unsigned) atoi(argv[1]);
    init_timer();
    start_timer(freq);
}

void halt_command(int argc, char **argv) {
    asm ("cli");
    outb(0x64, 0xfe);
    asm ("hlt");
}

void clear_command(int argc, char **argv) {
    clear_screen();
}

void ebda_command(int argc, char **argv) {
    for(u8 i = 0; i < 4; i++) printf("COM%d port: 0x%x\n", (i + 1), *((u16 *)(0x400 + 2 * i)));
    for(u8 i = 0; i < 3; i++) printf("LPT%d port: 0x%x\n", (i + 1), *((u16 *)(0x408 + 2 * i)));
    printf("EBDA base address: 0x%x\n", *((u16 *)0x40e) << 4);
    printf("Flags: 0x%x\n", *((u16 *)0x410));
    printf("Unusable memory: %dKB\n", *((u16 *)0x413));
    printf("Keyboard state flags: 0x%x\n", *((u16 *)0x417));
    printf("Display mode: %d\n", *((u8 *)0x449));
    printf("Text mode columns: %d\n", *((u16 *)0x44a));
    printf("Video base IO port: 0x%x\n", *((u16 *)0x463));
    printf("Number of IRQ0 calls since boot: %d\n", *((u16 *)0x46c));
    printf("Number of detected hard disks: %d\n", *((u8 *)0x475));
}

void malloc_command(int argc, char **argv) {
    if (argc == 2) {
        u32 ptr = (u32) malloc(atoi(argv[1]));
        if(ptr != 0) {
            printf("Successfully allocated %d bytes from 0x%x to 0x%x\n", atoi(argv[1]), ptr, ptr + atoi(argv[1]));
        } else {
            printf("Failed to allocate bytes\n");
        }
    } else {
        printf("Usage: malloc (bytes)\n");
    }
}

void free_blocks_command(int argc, char **argv) {
    if(argc == 1) {
        int status;
        if((status = free_last_block()) != 0) {
            printf("Freeing a block failed! error code: %d\n", status);
            return;
        };
        printf("Freed a block\n");
    } else if(argc == 2) {
        int count = atoi(argv[1]);
        int c2 = 0;
        for(int i = 0; i < count; i++) {
            int status;
            if((status = free_last_block()) != 0) {
                printf("Freeing a block failed! error code: %d\n", status);
                break;
            };
            c2++;
        }
        printf("Freed %d blocks\n", c2);
    } else {
        printf("Usage: free_blocks <blocks>\n");
    }
}

void dump_command(int argc, char **argv) {
    if(argc != 3) {
        printf("Usage: dump (address) (bytes count)\n");
        return;
    } else if(argc == 3) {
        u32 byteptr = atoi(argv[1]);
        int count = atoi(argv[2]);
        int i, j;

        for(i = 0; i < count; i += 16) {
            printf("%08x: ", i + byteptr);
            
            for(j = 0; j < 16; j++) {
                if(i + j < count) {
                    printf("%02x ", ((u8 *)byteptr)[i + j]);
                } else {
                    printf(" ");
                }
            }

            printf(" ");
            for(j = 0; j < 16; j++) {
                if(i + j < count) {
                    char c = ((u8 *)byteptr)[i + j];
                    printf("%s", (char []){(c >= 32 && c <= 126 ? c : '.'), 0});
                }
            }

            printf("\n");
        }
    }
}

void write_command(int argc, char **argv) {
    if(argc != 3) {
        printf("Usage: write (address) (byte)");
        return;
    } else {
        u32 addr = atoi(argv[1]);
        u8 data = atoi(argv[2]);
        *((u8 *)addr) = data;
        printf("Wrote 0x%02x to 0x%08x\n", data, addr);
    }
}

void memset_command(int argc, char **argv) {
    if(argc != 4) {
        printf("Usage: memset (address) (byte) (count)");
        return;
    } else {
        u32 addr = atoi(argv[1]);
        u8 val = atoi(argv[2]);
        u32 count = atoi(argv[3]);
        memset((char *)addr, val, count);
        printf("Wrote 0x%02x from 0x%08x to 0x%08x %d times\n", val, addr, addr + count, count);
    }
}

void debug_command(int argc, char **argv) {
    if(strcmp(argv[1], "cr0") == 0) {
        if(argc == 3) {
            u32 val = atoi(argv[2]);
            asm("mov %0, %%cr0" :: "a"(val));
        } else if(argc == 2) {
            u32 val;
            asm("mov %%cr0, %0" : "=a"(val));
            printf("cr0: 0x%x\n", val);
        }
    } else if(strcmp(argv[1], "time") == 0) {
        printf("Seconds since startup: ", (get_tick() * 3)/100);
        print_float(get_time());
        printf("\n");
    } else if(strcmp(argv[1], "pefr") == 0) {
        int lim = 1000000;
        if(argc == 3) lim = atoi(argv[2]);
        float start = get_time();
        float end;
        for(int i = 0; i < lim; i++) end = get_time();
        print_float(end);
        print_float(start);
        float time = end - start;
        printf("Time taken for %d empty loops: ", lim);
        print_float(time);
        printf("\nEfficiency: ");
        print_float((time * 1000) / lim);
        printf("ms/iteration\n");
    }
}

void outb_command(int argc, char **argv) {
    if(argc != 3) {
        printf("Usage: outb (port) (data)");
    } else {
        u16 port = atoi(argv[1]);
        u8 data = atoi(argv[2]);
        outb(port, data);
    }
}