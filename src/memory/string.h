#ifndef STRING_H
#define STRING_H

#include "../common.h"

void strcpy(char *dest, const char *src);
char **split(char *in);
void backspace(char *in);
void append(char *in, char ch);
int strlen(char *in);

void firstWord(char *in, char *res);

#endif

