#include "memory_management.h"
#include <assert.h>

#define DEBUG_MODE  // submit version should be set to 0 to avoid debug information
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
        // is an empty_linked list
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

static meta_block *fetch_heap_last_meta_address() {
    MODIFIED_GLOBAL_VARIABLE_WARNING
    assert(heap_start_address != NULL);

    // traverse the heap
    meta_block *curr = heap_start_address;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    return curr;


}

void *get_payload_from_meta_address(meta_block *meta_block_address) {
    return (void *) (meta_block_address + 1); // add size of meta_block
}

meta_block *get_meta_address_from_payload(void *payload_address) {
    return ((meta_block *) payload_address) - 1;
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
        } else if (curr_pos->allocated_block_data_size == size) {
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
    // the latter one's prev equal to new one
    meta_address->next->prev = new_free_block;
    // the older one's next one is equal to the new one
    meta_address->next = new_free_block;
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
//            meta_block *s = extend_heap(sbrk(0), size);
            meta_block *s = extend_heap(fetch_heap_last_meta_address(), size);
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


void merge_block(meta_block *latter_block) {
    // merge_block always accept the meta_block ptr of
    // the latter pointer, and merge it with the previous one

    assert(latter_block->prev->free == true);
    // record the size or the latter_block and set the space to be empty
    size_t size_of_meta_and_payload = META_BLOCK_SIZE + latter_block->allocated_block_data_size;
    memset(latter_block, 0, size_of_meta_and_payload);
    // give the previous block more control
    latter_block->prev->allocated_block_data_size += size_of_meta_and_payload;

}


bool address_validation(void *p) {
    if (p == NULL) return false;
    return true;
}


void *_free(void *_malloc_ptr) {
    // giving a ptr, and we set the pos to be freed [A F A]
    // And we do a "recursive-like" merge
    // [F F] A | [F F F] | A [F F]
    if (!address_validation(_malloc_ptr)) return NULL;
    //find the meta_data from the _malloc_ptr
    meta_block *curr_free_meta = get_meta_address_from_payload(_malloc_ptr);
    //set must-walk free curr
    curr_free_meta->free = true;
    // A F A or NULL F A or A F NULL
    // means no need to merge
    if((curr_free_meta->prev==NULL&&curr_free_meta->next->free==false)
      |(curr_free_meta->prev->free==false&&curr_free_meta->next->free==false)
      |(curr_free_meta->prev->free=false&&curr_free_meta->next==NULL))
    {
        // delete the payload info
        memset(_malloc_ptr,0, get_meta_address_from_payload(_malloc_ptr)->allocated_block_data_size);
        // and since free is set, just return
        return NULL;
    }
    // merge the previous
    // A F A -> F F A-> FB A
    if(curr_free_meta->prev!=NULL&&curr_free_meta->next->free==false&&curr_free_meta->prev->free==true){
        //
        merge_block(curr_free_meta);
        return NULL;
    }
    // merge the next
    //A A F -> A F F -> A FB
    if(curr_free_meta->next!=NULL&&curr_free_meta->prev->free==false&&curr_free_meta->next->free==true){
        merge_block(curr_free_meta->next);
    }
   // both merge (Actually same as previous)
   // F A F -> F F F
   // find the next and merge
   if(curr_free_meta->prev!=NULL&&curr_free_meta->next!=NULL&&curr_free_meta->prev->free&&curr_free_meta->next->free){
       merge_block(curr_free_meta->next);
   }


}