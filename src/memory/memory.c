#include "memory.h"

// Metadata structure for tracking allocations.
typedef struct {
    u32 size;      // Size of the allocated block.
    int is_free;  // Whether this block is free.
} BlockHeader;

static u8 memory_pool[POOL_SIZE]; // The static memory pool.
static u32 offset = 0;            // Current offset into the memory pool.

void *malloc(u32 size) {
    // Align size to the nearest multiple of 4 for alignment.
    size = (size + 3) & ~3;

    u32 current_offset = 0;
    while (current_offset < offset) {
        BlockHeader *header = (BlockHeader *)&memory_pool[current_offset];
        if (header->is_free && header->size >= size) {
            header->is_free = 0;
            return (void *)(header + 1);
        }
        current_offset += sizeof(BlockHeader) + header->size;
    }

    // No free block, allocate at the end of the pool.
    if (offset + sizeof(BlockHeader) + size > POOL_SIZE) {
        return 0; // Not enough memory.
    }

    BlockHeader *header = (BlockHeader *)&memory_pool[offset];
    header->size = size;
    header->is_free = 0;

    void *ptr = (void *)(header + 1);
    offset += sizeof(BlockHeader) + size;

    return ptr;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    BlockHeader *header = (BlockHeader *)ptr - 1;
    header->is_free = 1;
}

void reset_pool() {
    offset = 0; // Reset the entire pool for reuse.
}

void memcpy(char *source, char *dest, u32 nbytes) {
    u32 i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}
