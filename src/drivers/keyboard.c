#include "keyboard.h"
#include "video.h"
#include "../memory/string.h"
#include "../memory/memory.h"
#include "../shell/commands.h"

#define P KEY_PREFIX
#define N KEY_NONE
#define A KEY_ASCII
#define S KEY_SPECIAL
#define UNA 0

u8 scancode_type[] = {
    N, P, A, A, A, A, A, A, A, A, A, 
    A, A, A, S, S, A, A, A, A, A, A,
    A, A, A, A, A, A, S, P, A, A, A,
    A, A, A, A, A, A, A, A, A, P, A,
    A, A, A, A, A, A, A, A, A, A, P,
    N, P, A, P, S, S, S, S, S, S, S,
    S, S, S, S, S, S, S, S, S, S, S
};

u8 scancode_ascii[] = {
    UNA, UNA, '1', '2', '3', '4', '5', '6', '7', '8', '9', 
    '0', '-', '=', UNA, UNA, 'q', 'w', 'e', 'r', 't', 'y',
    'u', 'i', 'o', 'p', '[', ']', UNA, UNA, 'a', 's', 'd',
    'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', UNA, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', UNA,
    UNA, UNA, ' ', UNA, UNA, UNA, UNA, UNA, UNA, UNA, UNA, //f7
    UNA, UNA, UNA, UNA, UNA, UNA, UNA, UNA, UNA, UNA, UNA
};

u8 scancode_ascii_shift[] = {
    UNA, UNA, '!', '@', '#', '$', '%', '^', '&', '*', '(', 
    ')', '_', '+', UNA, UNA, 'Q','W', 'E', 'R', 'T', 'Y',
    'U', 'I', 'O', 'P', '{', '}', UNA, UNA, 'A', 'S', 'D',
    'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', UNA, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', UNA,
    UNA, UNA, ' ', UNA, UNA, UNA, UNA, UNA, UNA, UNA, UNA
};


static char buffer[128];
static keypress_t keys;

void process_command(char *command);

static void keyboard_callback(registers_t regs) {
    u8 scan_code = inb(0x60);
    int pressed = 1;
    int type;
    char c;
    if(scan_code == KEY_E0) return;
    if(scan_code == KEY_E1 || keys.pause) {
        keys.pause = 1;
        if(scan_code == 0xc5) {
            keys.pause = 0;
        }
        return;
    }
    if (scan_code & KEY_RELEASE) {
        scan_code &= 0x7f;
        pressed = 0;
    }

    type = scancode_type[scan_code];
    if(pressed && type == KEY_NONE) {
        printf("\nScan code not supported: 0x%x\n", scan_code);
        shell();
    }

    if(type == KEY_PREFIX) {
        switch (scan_code) {
            case KEY_ESC:
                keys.esc = pressed;
                break;
            case KEY_CAPSLOCK: 
                if (pressed) {
                    keys.capslock = !keys.capslock;
                } 
                break;
            case KEY_CTRL: 
                keys.ctrl = pressed;
                break;
            case KEY_ALT: 
                keys.alt = pressed;
                break;
            case KEY_START: 
                keys.start = pressed;
                break;
            case KEY_RSHIFT:
            case KEY_LSHIFT: 
                keys.shift = pressed;
                break;
        }

        return;
    }

    if (type == KEY_SPECIAL && pressed) {
        switch (scan_code) {
            case KEY_ENTER: 
                //printf2("\nwhat did you say: %s", buffer);
                process_command(buffer);
                buffer[0] = '\0';
                printf("\n");
                shell();
                break;
            case KEY_BACK: 
                kprint_backspace();
                backspace(buffer);
                break;
        }
        return;
    }

    if(pressed && type == KEY_ASCII) {
        if (keys.capslock || keys.shift) {
            c = scancode_ascii_shift[scan_code];
        } else {
            c = scancode_ascii[scan_code];
        }

        if((c == 'd' || c == 'D') && keys.ctrl) {
            c = '\0';
        }

        char buff[2] = {c, '\0'};
        append(buffer,c);
        printf("%s", buff);
    }

    (void)regs;
}

void init_keyboard() {
   register_interrupt_handler(IRQ1, keyboard_callback); 
}

static char *supported_commands[] = {"help", "info", "halt", "print", "timer", "clear", 
"ebda"};
static void (*commands_func[])(int, char**) = {NULL, info_command, halt_command, print_command,
timer_command, clear_command, ebda_command};

void process_command(char *command1) {
    int supp_len = sizeof(supported_commands)/ sizeof(supported_commands[0]);
    char command[128];
    char *args[10];
    firstWord(command1, command);
    int valid = 0;
    for(int i = 0; i < supp_len; i++) {
        if(strcmp(command, supported_commands[i]) == 0) {
            split(command1, args);

            printf2("\nExecuted command: %a9%s%as\n", command1);
            printf2("Args: \n");
            int argc;
            for(argc = 0; args[argc] != NULL; argc++) {
                printf2("    %d. %s\n", argc, args[argc]);
            }

            if(strcmp(command, "help") == 0) {
                printf("Supported commands: ");
                for(int j = 0; j < supp_len; j++) printf(" %s", supported_commands[j]);
                valid = 1;
                continue;
            }

            if(commands_func[i] != NULL) commands_func[i](argc, args);
            valid = 1;
        }
    }

    if(!valid) {
        printf2("\nInvalid command \"%a4%s%as\"\n", command);
    }
};