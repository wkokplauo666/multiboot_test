#ifndef IO_H
#define IO_H

#include "../common.h"

#define COM1 0x3f8
#define LSR_EMPTY 0x20

u8 inb(u16 port);
void outb(u16 port, u8 data);

void init_serial();
void serial_puts(const char *str);
void serial_putc(const char c);

#define BUFFER_SIZE 64

char *itoa(u32 input);
char *itohex(u32 input);
int strcmp(const char *a, const char *b);
void print(const char *s1, ...);
void printf(const char *s1, ...);

#endif