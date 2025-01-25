#ifndef MEMORY_H
#define MEMORY_H

#define POOL_SIZE 1024 // 1KB

#include "../common.h"

void *malloc(u32 size);
void free(void *ptr);
void reset_pool();

#endif