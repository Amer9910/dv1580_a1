#include "memory_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct BlockMeta {
    size_t size;
    int available;
    void* start;
    struct BlockMeta* next;
} BlockMeta;

static void* global_pool = 0;
static BlockMeta* block_list = 0;
static size_t pool_total_size = 0;


void mem_init(size_t pool_size) {
    global_pool = malloc(pool_size);
    if (global_pool == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory pool\n");
        exit(EXIT_FAILURE);
    }

    block_list = (BlockMeta*)malloc(sizeof(BlockMeta));
    if (block_list == NULL) {
        free(global_pool);
        fprintf(stderr, "Error: Metadata allocation failed\n");
        exit(EXIT_FAILURE);
    }

    block_list->size = pool_size;
    block_list->available = 1;
    block_list->start = global_pool;
    block_list->next = NULL;
    pool_total_size = pool_size;
}


void* mem_alloc(size_t request_size) {
    BlockMeta* curr = block_list;

    while (curr != NULL) {
        if (curr->available == 1 && curr->size >= request_size) {
            if (curr->size > request_size) {
                BlockMeta* split = (BlockMeta*)malloc(sizeof(BlockMeta));
                if (split == NULL) return NULL;

                split->size = curr->size - request_size;
                split->available = 1;
                split->start = (char*)curr->start + request_size;
                split->next = curr->next;

                curr->size = request_size;
                curr->available = 0;
                curr->next = split;

                return curr->start;
            } else if (curr->size == request_size) {
                curr->available = 0;
                return curr->start;
            }
        }
        curr = curr->next;
    }

    return NULL;
}


void mem_free(void* ptr) {
    if (ptr == NULL) return;

    BlockMeta* node = block_list;
    while (node != NULL) {
        if (node->start == ptr) {
            if (node->available == 1) return;

            node->available = 1;

            while (node->next != NULL && node->next->available == 1) {
                BlockMeta* to_merge = node->next;
                node->size += to_merge->size;
                node->next = to_merge->next;
                free(to_merge);
            }
            return;
        }
        node = node->next;
    }

    fprintf(stderr, "Warning: Tried to free untracked memory at %p\n", ptr);
}


void* mem_resize(void* ptr, size_t new_size) {
    if (ptr == NULL) return mem_alloc(new_size);

    BlockMeta* blk = block_list;
    while (blk != NULL) {
        if (blk->start == ptr) {
            if (blk->size >= new_size) return ptr;

            void* new_mem = mem_alloc(new_size);
            if (new_mem != NULL) {
                memcpy(new_mem, ptr, blk->size);
                mem_free(ptr);
            }
            return new_mem;
        }
        blk = blk->next;
    }

    fprintf(stderr, "Resize failed: memory at %p not tracked\n", ptr);
    return NULL;
}


void mem_deinit() {
    free(global_pool);
    global_pool = NULL;

    BlockMeta* current = block_list;
    while (current != NULL) {
        BlockMeta* following = current->next;
        free(current);
        current = following;
    }

    block_list = NULL;
    pool_total_size = 0;
}
