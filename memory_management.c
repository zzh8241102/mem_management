#include "memory_management.h"
#include <assert.h>

#define DEBUG_MODE 1
#define MODIFIED_GLOBAL_VARIABLE_WARNING

/* -------------------------------
  The heap_start_address
 *-------------------------------
*/
meta_block *heap_start_address = NULL;


void *extend_heap(meta_block *last_address, size_t size) {
    // notice here modified the global variable heap end address
    meta_block *current_break = (meta_block *) sbrk(0);
    // move the pointer to the last pos
    if (sbrk(META_BLOCK_SIZE + size) == (void *) -1) {
        return NULL;
    } else {
        // here is used to limit the case that the heap initially
        // is a empty linked list
        if (last_address != NULL) {
            // attach to the last address
            last_address->next = current_break;
        }
        // allocate the memory
        current_break->next = NULL; //it is the last block
        current_break->prev = last_address;
        current_break->free = false;
        current_break->allocated_block_data_size = size;
        // find the payload address
        return current_break;
    }
}


void *get_payload_from_meta_address(meta_block *meta_block_address) {
    return (void *) (meta_block_address + 1); // add size of meta_block
}

// You can apply different kinds of search method
meta_block *first_fit_search(size_t size) {
    // assert size has already been aligned to 8
    assert((size & 0x3) == 0);
    // always search from the 1st place
    meta_block *curr_pos = heap_start_address;
    while (curr_pos != NULL) {
        // note that the size not only need to be bigger than the size
        if (curr_pos->free && curr_pos->allocated_block_data_size > size + META_BLOCK_SIZE) {
            // cannot equal to !!!
            return curr_pos;
        } else if (curr_pos->allocated_block_data_size) {
            // will be judged on the split_block
            return curr_pos;
        } else {
            curr_pos = curr_pos->next;
        }
    }
    return NULL;
}

bool split_block(size_t size, meta_block *meta_address) {
    // if the size of the original data filed is equal to size
    // of the to be split size, there is no need to split
    // we just use this proper node
    if (size == meta_address->allocated_block_data_size) return false;
    assert((size & 0x3) == 0);
    // split a new block
    // the back block
    meta_block *new_free_block;
    // for the new_block
    // the meta part
    // the meta address
    new_free_block = meta_address->next; // attach it to last linked list
    new_free_block->prev = meta_address;
    new_free_block->free = true;
    new_free_block->next = meta_address->next;
    new_free_block->allocated_block_data_size = meta_address->allocated_block_data_size - size - META_BLOCK_SIZE;
    // fix the older and the older subsequent one
    return true;

}


size_t round_align(size_t size, uint64_t round_byte) {
    return round_byte * ((size + round_byte - 1) / round_byte);
}

void *_malloc(size_t size) {
    size = round_align(size, 8);
    // if already has an address
    if (heap_start_address) {
        meta_block *proper_meta_address = first_fit_search(size);
        if (proper_meta_address == NULL) {
            // allocate a new space
            // sbrk(0) or the end of the heap
            meta_block *s = extend_heap(sbrk(0), size);
            if (!s) return NULL;
            return get_payload_from_meta_address(s);
        } else {
            // if found a workable place
            if (!split_block(size, proper_meta_address)) {
                // a most proper place
                return get_payload_from_meta_address(proper_meta_address);
            } else {
                // return the newly split block's meta block position
                return get_payload_from_meta_address(proper_meta_address->next);
            }
        }


    } else {
        // init the heap since the start_address is NULL
        meta_block *meta_address = extend_heap(heap_start_address, size);
        if (meta_address == NULL) {
#ifdef DEBUG_MODE
            printf("There is not any space hah code is wrong ");
#endif
            return NULL;
        } else {
            MODIFIED_GLOBAL_VARIABLE_WARNING
            heap_start_address = meta_address;
            return get_payload_from_meta_address(meta_address);
        }
    }

}


void *_free(void *ptr) {

}