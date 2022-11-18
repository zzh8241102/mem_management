#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

#define META_BLOCK_SIZE sizeof(meta_block)

// [meta] ->(by add 1(sizeof(meta)))[data]
typedef struct mem_meta_block {
    size_t allocated_block_data_size;
    struct mem_meta_block *prev;
    struct mem_meta_block *next;
    bool free; // 1 indicated is a free block

} meta_block;

/*
 *-------------------------------
  util functions
 *-------------------------------
*/

meta_block *first_fit_search(size_t size);

meta_block *extend_heap(meta_block *address, size_t size);

void *get_payload_from_meta_address(meta_block *meta_block_address);

meta_block *get_meta_address_from_payload(void *payload_address);

bool split_block(size_t size, meta_block *meta_address);

size_t round_align(size_t size, u_int64_t round_byte);

void merge_block(meta_block *latter_block);

bool address_validation(void *p);

void last_free_chunk_handler();

/*
 *-------------------------------
  key functions
 *-------------------------------
*/

void *_malloc(size_t size); // return the payload address

void *_free(void *ptr);