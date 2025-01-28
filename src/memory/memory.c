#include "memory.h" // Not the standard memory.h header.

// Metadata structure for tracking allocations.
typedef struct {
    u16 size;      // Size of the allocated block. (max size is 65535)
    u8 is_free;    // Whether this block is free.
} __attribute__((packed)) BlockHeader; // Only takes 3 bytes.

static u8 memory_pool[POOL_SIZE] __attribute__((aligned(4))); // The static memory pool.
static u32 offset = 0; // Current offset into the memory pool.

void *malloc(u32 size) {
    if (size == 0 || size > POOL_SIZE - sizeof(BlockHeader)) {
        return NULL; // Invalid or oversized allocation.
    }

    // Align size to the nearest multiple of 4 for alignment.
    size = (size + 3) & ~3;

    BlockHeader *best_fit = NULL;
    u32 best_fit_offset = 0;
    u32 current_offset = 0;

    // Find the best-fit free block.
    while (current_offset < offset) {
        BlockHeader *header = (BlockHeader *)&memory_pool[current_offset];
        if (header->is_free && header->size >= size &&
            (!best_fit || header->size < best_fit->size)) {
            best_fit = header;
            best_fit_offset = current_offset;
        }
        current_offset += sizeof(BlockHeader) + header->size;
    }

    if (best_fit) {
        // Use the best-fit block.
        best_fit->is_free = 0;

        // Split the block if there is excess space.
        if (best_fit->size > size + sizeof(BlockHeader)) {
            u32 remaining_size = best_fit->size - size - sizeof(BlockHeader);
            BlockHeader *new_header = (BlockHeader *)((u8 *)best_fit + sizeof(BlockHeader) + size);
            new_header->size = remaining_size;
            new_header->is_free = 1;
            best_fit->size = size;
        }

        return (void *)(best_fit + 1);
    }

    // Allocate at the end of the pool if no suitable free block was found.
    if (offset + sizeof(BlockHeader) + size > POOL_SIZE) {
        return NULL; // Not enough memory.
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

    // Coalesce adjacent free blocks.
    u32 current_offset = 0;
    while (current_offset < offset) {
        BlockHeader *current = (BlockHeader *)&memory_pool[current_offset];
        if (current->is_free) {
            u32 next_offset = current_offset + sizeof(BlockHeader) + current->size;
            if (next_offset < offset) {
                BlockHeader *next = (BlockHeader *)&memory_pool[next_offset];
                if (next->is_free) {
                    current->size += sizeof(BlockHeader) + next->size;
                    continue; // Check for further coalescing.
                }
            }
        }
        current_offset += sizeof(BlockHeader) + current->size;
    }
}

void reset_pool() {
    offset = 0; // Reset the entire pool for reuse.
}

int free_last_block() {
    if (offset == 0) {
        return 1; // No allocations have been made; nothing to free.
    }

    // Traverse backward to find the last allocated block.
    u32 current_offset = 0;
    u32 last_block_offset = 0;

    while (current_offset < offset) {
        BlockHeader *header = (BlockHeader *)(memory_pool + current_offset);
        last_block_offset = current_offset;
        current_offset += sizeof(BlockHeader) + header->size;
    }

    // Get the last block's header.
    BlockHeader *last_block = (BlockHeader *)(memory_pool + last_block_offset);

    if (last_block->is_free) {
        // If the last block is already free, return an error code.
        return 2;
    }

    // Mark the last block as free.
    last_block->is_free = 1;

    // Adjust the offset to exclude the block from the active pool.
    offset = last_block_offset;

    return 0; // Successfully freed the last block.
}


void memcpy(char *source, char *dest, u32 nbytes) {
    u32 i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memset(char *dest, u8 value, u32 nbytes) {
    u32 i;
    for(i = 0; i < nbytes; i++) {
        *(dest + i) = value;
    }
}
