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
    return (void*)(meta_block_address + 1); // add size of meta_block
}

// You can apply different kinds of search method
meta_block *first_fit_search(size_t size){
    // assert size has already been aligned to 8
    assert((size&0x3)==0);
    // always search from the 1st place
    meta_block *curr_pos = heap_start_address;
    while(curr_pos!=NULL){
        if(curr_pos->free&&curr_pos->allocated_block_data_size>=size){
            return curr_pos;
        } else{
            curr_pos = curr_pos->next;
        }
    }
    return NULL;
}

void * split_block(){

}


size_t round_align(size_t size,uint64_t round_byte){
    return round_byte * ((size + round_byte - 1) / round_byte);
}

void*_malloc(size_t size) {
    size = round_align(size,8);
    if (heap_start_address) {

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
            heap_start_address = meta_address ;
            return get_payload_from_meta_address(meta_address);
        }
    }

}


void *_free(void *ptr) {

}