#ifndef MEMORY_H
#define MEMORY_H

#define POOL_SIZE 1024 // 1KB
#define NULL (void *) 0

#include "../common.h"

void *malloc(u32 size);
void free(void *ptr);
void reset_pool();
void memcpy(char *s, char *d, u32 n);
int free_last_block();
void memset(char *dest, u8 v, u32 n);

#endif