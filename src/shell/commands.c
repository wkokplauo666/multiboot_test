#include "commands.h"

void print_command(int argc, char **argv) {
    if(argc == 2 && strcmp(argv[1], "vgatext") == 0) {
        for(int i = 0; i < 0xff; i++) {
            char tmp[] = {i, 0};
            printf2("%d. %af%s%as ", i, tmp);
        }
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
}