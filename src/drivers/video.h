#ifndef VIDEO_H
#define VIDEO_H

#include "../common.h"

#define VIDEO_ADDRESS (char *)0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4


#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5


void clear_screen();
void kprint_at(char *message, int col, int row, u8 attr);
void kprint_c_at(char message, int col, int row, u8 attr);
void kprint(char *message, u8 attr);
void kprint_c(char message, u8 attr);
void kprintd(char *message);
void kprint_backspace();


int get_cursor_offset();
void set_cursor_offset(int offset);
int print_char(char c, int col, int row, char attr);
int get_offset(int col, int row);
int get_offset_row(int offset);
int get_offset_col(int offset);

#endif