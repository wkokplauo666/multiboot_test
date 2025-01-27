#ifndef STRING_H
#define STRING_H

#include "../common.h"

void strcpy(char *dest, const char *src);
void backspace(char *in);
void append(char *in, char ch);
int strlen(char *in);

void firstWord(char *in, char *res);

char *strtok(char *str, const char *delim);
char *strchr(const char *str, char ch);
void split(char *in, char *buf[]);

int arglen(char **arg);
void skip(char **in);
int atoi(const char *in);

void shell();

#endif

