#include "string.h"
#include "memory.h"
#include "../drivers/io.h"

void strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len-1] = '\0';
}

void firstWord(char *sentence, char *res) {
    while(*sentence == ' ') sentence++;

    while(*sentence && !(*sentence == ' ')) {
        *res = *sentence;
        res++;
        sentence++;
    }

    *res = '\0';
}

char *strchr(const char *str, char ch) {
    while (*str) {
        if (*str == ch) {
            return (char *)str;
        }
        str++;
    }

    // If looking for '\0', return a pointer to the null terminator
    if (ch == '\0') {
        return (char *)str;
    }

    return NULL;
}

char *strtok(char *str, const char *delim) {
    static char *input = NULL;

    // Check for NULL delimiters
    if (delim == NULL) {
        return NULL;
    }

    // Initialize the input string if str is provided
    if (str != NULL) {
        input = str;
    }
    if (input == NULL) {
        return NULL;
    }

    // Skip leading delimiters
    while (*input && strchr(delim, *input)) {
        input++;
    }

    // If we've reached the end of the string, return NULL
    if (*input == '\0') {
        return NULL;
    }

    // Mark the beginning of the token
    char *token = input;

    // Find the end of the token
    while (*input && !strchr(delim, *input)) {
        input++;
    }

    // Null-terminate the token if necessary
    if (*input) {
        *input = '\0';
        input++;
    }

    return token;
}

void split(char *in, char *buf[]) {
    int i = 0;
    char *token = strtok(in, " ");
    while(token != NULL) {
        buf[i] = malloc(strlen(token) + 1);
        strcpy(buf[i], token);
        i++;
        token = strtok(NULL, " ");
    }

    buf[i] = NULL;
}

int arglen(char **args) {
    int i = 0;
    while(args[i] != NULL) {
        i++;
    }
    return i;
}

void skip(char **in) {
    int i;
    for(i = 0; (i + 1) < arglen(in); i++) {
        in[i] = in[i + 1];
    }
    in[i] = NULL;
}

int atoi(const char *str) { 
    int res = 0;
    int sign  = 1;

    if(str == NULL) return 0;
    while(*str == ' ') {
        str++;
    }

    if(*str == '-') {
        sign = -1;
        str++;
    } else if(*str == '+') {
        str++;
    }

    while(*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }

    return res * sign;
}

void shell() {
    printf2("%aeshell>%as ");
}