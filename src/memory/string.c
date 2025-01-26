#include "string.h"
#include "memory.h"

void strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

char **split(char *input) {
    u32 word_count = 1;
    for(int i = 0; input[i] != 0; i++) {
        if(input[i] == ' ') word_count++;
    }
    char **res = (char **)malloc(word_count * sizeof(char *));
    if(res == 0) {
        return 0;
    }
    u32 word_index = 0;
    char *word_start = input;
    for(int i = 0; ; i++) {
        if(input[i] == ' ' || input[i] == 0) {
            if (i > 0 && input[i - 1] != ' ') {
                u32 word_lenght = &input[i] - word_start;
                res[word_index] = (char *)malloc(word_lenght + 1);
                if(res[word_index] == 0) return 0;
                strcpy(res[word_index], word_start);
                word_index++;
            }

            if(input[i] == '\0') break;
            word_start = &input[i + 1];
        }
    }
    return res;
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