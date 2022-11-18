/**
 * @FILE_NAME: memory_management.c
 * @DESCRIPTION:Memory management function impl
 * @MAIN_FUNC: void* _malloc(size_t size) and void* _free(void* ptr)
 * @AUTHOR: Zihan Zhou
 * @DATE: 2022-11
 * @PURPOSE: The coursework of operating system
 */

#include "memory_management.h"


#define DEBUG_MODE 1// submit version should be set to 0 to avoid debug information
#define MODIFIED_GLOBAL_VARIABLE_WARNING


/* -------------------------------
  The heap_start_address
 *-------------------------------
*/
meta_block *heap_start_address = NULL;


static meta_block *fetch_heap_last_meta_address() {
    assert(heap_start_address != NULL);

    // traverse the heap
    meta_block *curr = heap_start_address;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    return curr;


}

meta_block *extend_heap(meta_block *last_address, size_t size) {
    // notice here modified the global variable heap end address
    meta_block *current_break = (meta_block *) sbrk(0);
    // move the pointer to the last pos
    if (sbrk(META_BLOCK_SIZE + size) == (void *) -1) {
#if DEBUG_MODE
        printf("cannot extend the heap due to sbrk limitation, resort to another solution");
#endif
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
        } else if (curr_pos->free && curr_pos->allocated_block_data_size == size) {
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
//    new_free_block = meta_address->next;
    new_free_block = get_payload_from_meta_address(meta_address) + size;
    new_free_block->prev = meta_address;
    new_free_block->free = true;
    new_free_block->next = meta_address->next;
    new_free_block->allocated_block_data_size = meta_address->allocated_block_data_size - size - META_BLOCK_SIZE;
    // fix the older and the older subsequent one
    // the latter one's prev equal to new one
    meta_address->free = false; // since you should return it
    meta_address->allocated_block_data_size = size;
    if (meta_address->next != NULL) { meta_address->next->prev = new_free_block; }
    // the older one's next one is equal to the new one
    meta_address->next = new_free_block;
    return true;

}


size_t round_align(size_t size, u_int64_t round_byte) {
    return round_byte * ((size + round_byte - 1) / round_byte);
}

void *_malloc(size_t size) {
    if (size <= 0) return NULL;
    size = round_align(size, 8);
    // if already has an address
    if (heap_start_address) {
        meta_block *proper_meta_address = first_fit_search(size);
        if (proper_meta_address == NULL) {
#if DEBUG_MODE
            printf("we are allocating a new space since the place is not enough,");
#endif
            // allocate a new space
            // sbrk(0) or the end of the heap
//            meta_block *s = extend_heap(sbrk(0), size);
            meta_block *s = extend_heap(fetch_heap_last_meta_address(), size);
#if DEBUG_MODE
            printf("successfully extend, now the extended meta address is %p\n", s);
            printf("the extended payload address is %p\n", get_payload_from_meta_address(s));
#endif
            if (!s) return NULL;
            return get_payload_from_meta_address(s);
        } else {
            // if found a workable place
#if DEBUG_MODE
            printf("There are some good memory fits: ");
#endif
            if (!split_block(size, proper_meta_address)) {
                // a most proper place
#if DEBUG_MODE
                printf("No need to split, we apply the proper size for it, and the meta address is%p\n",
                       proper_meta_address);
                printf("No need to split, we apply the proper size for it, and the payload address is%p\n",
                       get_payload_from_meta_address(proper_meta_address));
#endif
                return get_payload_from_meta_address(proper_meta_address);
            } else {
                // return the newly split block's meta block position
                // no next
#if DEBUG_MODE
                printf("we split in a good way, the meta address is %p\n", proper_meta_address);
                printf("we apply the proper size for it, and the payload address is%p\n",
                       get_payload_from_meta_address(proper_meta_address));
#endif
                return get_payload_from_meta_address(proper_meta_address);
            }
        }


    } else {
#if DEBUG_MODE
        printf("Init the heap value here\n");
#endif
        // init the heap since the start_address is NULL
        meta_block *meta_address = extend_heap(heap_start_address, size);
        if (meta_address == NULL) {
#if DEBUG_MODE
            printf("There is not any space hah code is wrong ");
#endif
            return NULL;
        } else {
            MODIFIED_GLOBAL_VARIABLE_WARNING
            heap_start_address = meta_address;
#if DEBUG_MODE
            printf("successfully inited,the start meta_address is %p\n", meta_address);
            printf("the start payload_address is %p\n", get_payload_from_meta_address(meta_address));
#endif
            return get_payload_from_meta_address(meta_address);
        }
    }

}


void merge_block(meta_block *latter_block) {
    // merge_block always accept the meta_block ptr of
    // the latter pointer, and merge it with the previous one
    //         merge<-[][]
    //         [][size]->next
    //         => [][size+s2+meta] ->next = dropped next
    //         [][size]->next => [][size+expand]->next(deprecated)
    assert(latter_block->prev->free == true);
    assert(latter_block->prev->next = latter_block);
    // record the size or the latter_block and set the space to be empty
    size_t size_of_meta_and_payload = META_BLOCK_SIZE + latter_block->allocated_block_data_size;
    // give the previous block more control space
    latter_block->prev->allocated_block_data_size += size_of_meta_and_payload;
    // set the previous' next to the dropped next
    if (latter_block->next != NULL) { latter_block->next->prev = latter_block->prev; }
    latter_block->prev->next = latter_block->next; // a expand finished

}


bool address_validation(void *p) {
    if (p == NULL | p < (void *) heap_start_address + META_BLOCK_SIZE
        | p > get_payload_from_meta_address(fetch_heap_last_meta_address())) {
        return false;
    }
    return true;
}

void last_free_chunk_handler() {
    // back the last free blocks to os when it is appropriate
    meta_block *last = fetch_heap_last_meta_address();
    if (last->free && last->next == NULL) {
//        assert(last->prev->next != NULL);
        // delete the big free chunk
        if (last->prev != NULL && last->prev->next != NULL) { last->prev->next = NULL; }
        brk(last);
    }

}

void *_free(void *ptr) {
    // giving a ptr, and we set the pos to be freed [A F A]
    // And we do a "recursive-like" merge
    // [F F] A | [F F F] | A [F F]
    if (!address_validation(ptr)) return NULL;
    //find the meta_data from the ptr
    meta_block *curr_free_meta = get_meta_address_from_payload(ptr);
    //set must-walk free curr
    curr_free_meta->free = true;
    // A F A or NULL F A or A F NULL of NULL A NULL
    if (curr_free_meta->next == NULL && curr_free_meta->prev == NULL) {
        last_free_chunk_handler();
        // since already set free
        return NULL;
    }
    // means no need to merge
    if ((curr_free_meta->prev == NULL && curr_free_meta->next != NULL && curr_free_meta->next->free == false)
        | (curr_free_meta->next == NULL && curr_free_meta->prev != NULL && curr_free_meta->prev->free == false)
        | (curr_free_meta->next != NULL && curr_free_meta->prev != NULL
           && curr_free_meta->prev->free == false && curr_free_meta->next->free == false)
            ) {
        // delete the payload info
        // A[F]A F:[meta][value] => [meta->free][0000]
        // A[F]A F:[meta][value] => [meta->free][origin]
//        memset(ptr, 0, get_meta_address_from_payload(ptr)->allocated_block_data_size);
        // and since free is set, just return
        last_free_chunk_handler();
        return NULL;
    }
    // merge the previous
    // A F A -> F F A-> FB A

    // what if F F NULL
    if (curr_free_meta->prev != NULL && curr_free_meta->next == NULL &&
        curr_free_meta->prev->free == true) {
        //
        merge_block(curr_free_meta);
        last_free_chunk_handler();
        return NULL;
    }
    if (curr_free_meta->prev != NULL && curr_free_meta->next->free == false &&
        curr_free_meta->prev->free == true) {
        //
        merge_block(curr_free_meta);
        last_free_chunk_handler();
        return NULL;
    }
    // merge the next
    //A A F -> A F F -> A FB
    // what if NULL A F
    if (curr_free_meta->next != NULL && curr_free_meta->prev == NULL && curr_free_meta->next->free == true) {
        merge_block(curr_free_meta->next);
        last_free_chunk_handler();
        heap_start_address = NULL;
        return NULL;

    }
    if (curr_free_meta->prev != NULL) {
        if (curr_free_meta->next != NULL && curr_free_meta->prev->free == false && curr_free_meta->next->free == true) {
            merge_block(curr_free_meta->next);
            last_free_chunk_handler();
            return NULL;
        }
    }
    // both merge (Actually same as previous, For Reader-Friendly purpose attach it here)
    // F A F -> F F F
    // find the next and merge
    if (curr_free_meta->prev != NULL && curr_free_meta->next != NULL && curr_free_meta->prev->free &&
        curr_free_meta->next->free) {
        merge_block(curr_free_meta); // merge the first one
        merge_block(curr_free_meta->next); // merge tne second one
        last_free_chunk_handler();
        return NULL;
    }

    return NULL;
}