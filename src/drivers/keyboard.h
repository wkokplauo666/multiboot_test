#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "io.h"
#include "../memory/string.h"
#include "video.h"
#include "../interrupt/interrupt.h"

typedef struct {
    unsigned int esc:1, capslock:1, shift:1, ctrl:1, alt:1, start:1, pause:1;
} keypress_t;

#define KEY_NONE 0x00
#define KEY_PREFIX 0x01
#define KEY_ASCII 0x02
#define KEY_SPECIAL 0x04

#define KEY_RELEASE 0x80
#define KEY_E0 0xe0
#define KEY_E1 0xe1

#define KEY_ESC 0x1
#define KEY_1 0x2
#define KEY_2 0x3
#define KEY_3 0x4
#define KEY_4 0x5
#define KEY_5 0x6
#define KEY_6 0x7
#define KEY_7 0x8
#define KEY_8 0x9
#define KEY_9 0xa
#define KEY_0 0xb
#define KEY_MIN 0xc
#define KEY_EQUAL 0xd
#define KEY_BACK 0xe
#define KEY_TAB 0xf
#define KEY_Q 0x10
#define KEY_W 0x11
#define KEY_E 0x12
#define KEY_R 0x13
#define KEY_T 0x14
#define KEY_Y 0x15
#define KEY_U 0x16
#define KEY_I 0x17
#define KEY_O 0x18
#define KEY_P 0x19
#define KEY_SBRACKETS 0x1a
#define KEY_SBARCKETE 0x1b
#define KEY_ENTER 0x1c
#define KEY_CTRL 0x1d
#define KEY_A 0x1e
#define KEY_S 0x1f
#define KEY_D 0x20
#define KEY_F 0x21
#define KEY_G 0x22
#define KEY_H 0x23
#define KEY_J 0x24
#define KEY_K 0x25
#define KEY_L 0x26
#define KEY_SEMI 0x27
#define KEY_QUOTE 0x28
#define KEY_ACCENT 0x29
#define KEY_LSHIFT 0x2a
#define KEY_BACKSLASH 0x2b
#define KEY_Z 0x2c
#define KEY_RSIFT 0x36

void init_keyboard();

#endif