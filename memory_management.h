#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>

#define META_BLOCK_SIZE sizeof(meta_block)

// meta_block* addr   addr+META_BLOCK_SIZE = DATA_START_ADDR
typedef struct mem_meta_block{
    size_t allocated_block_data_size;
    struct mem_meta_block* prev;
    struct mem_meta_block* next;
    bool free; // 1 indicated is a free block

} meta_block;

meta_block *first_fit_search(size_t size);

void* extend_heap(meta_block* address,size_t size);

void * get_payload_from_meta_address(meta_block* meta_block_address);

size_t round_align(size_t size,uint64_t round_byte);

void* _malloc(size_t size); // return the payload address

void* _free(void* ptr);