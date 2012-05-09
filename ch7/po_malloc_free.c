/*
 * An implementation of malloc() and free() as per exercise 7-2 in
 * TLPI.
 * 
 * Copyright 2012, Paul Osborne
 *
 */

#include <stdlib.h>
#include <unistd.h>



typedef struct {
    size_t block_length;
    void *prev_free_chunk;
    void *next_free_chunk;
} free_block_header_t;

typedef struct {
    size_t block_length;
} memory_block_header_t;

static void *free_ll_start = NULL;
static void *start_of_free_heap = NULL;

#define FREE_BLOCK_HEADER(ptr) ((free_block_header_t *) ((void* )ptr - sizeof(memory_block_header_t)))

/* The malloc() function allocates size bytes and returns a pointer to
 * the allocated memory. The memory is not initialized. If size is 0,
 * then malloc() returns either NULL, or a unique pointer value that can
 * later be successfully passed to free().
 * ----
 * Implementation notes:
 * So, somebody wants a block of memory... fine!  Here's the game
 * plan for getting them said memory:
 * 
 * 1) Scan each block in our free list to see if any of them are
 *    big enough.  If we find one that is big enough, we will
 *    either use that memory as-is or chunk it into a couple smaller
 *    pieces (part of which is used, the other which is a new
 *    entry in the free linked list.
 * 2) If we do not find an item in the free list, we will add the
 *    new block onto the end of our heap.  We will attempt to expand
 *    the heap if needed.
 */
void * po_malloc(size_t size)
{
    free_block_header_t *current_header;
    void *memory_to_use = NULL;
    void *next_free_chunk = free_ll_start;
    while (next_free_chunk) {
        current_header = FREE_BLOCK_HEADER(next_free_chunk);
        if (current_header->block_length >= size) {
            memory_to_use = (void*)current_header + sizeof(memory_block_header_t);
            if (current_header->block_length > (size + sizeof(free_block_header_t))) {
                free_block_header_t *new_free_header = 
                    (free_block_header_t *)(memory_to_use + sizeof(memory_block_header_t) + size);
                new_free_header->prev_free_chunk = current_header->prev_free_chunk;
                new_free_header->next_free_chunk = current_header->next_free_chunk;
                new_free_header->block_length = current_header->block_length - size;  // TODO: is this right?
            }
            return memory_to_use;
        } else {
            // next!
            next_free_chunk = current_header->next_free_chunk;
        }
    }

    // if we are here, we didn't get what we needed from scanning the
    // freed list, tack on to the end of memory
    // TODO: continue here
    return NULL;
    
}

/*
 * The free() function frees the memory space pointed to by ptr, which
 * must have been returned by a previous call to malloc(), calloc() or
 * realloc(). Otherwise, or if free(ptr) has already been called before,
 * undefined behavior occurs. If ptr is NULL, no operation is performed.
 * ----
 * Implementation notes:
 * On free, we are given a pointer to an area of memory that is no longer
 * needed.  Immediately preceding this memory we should find a
 * memory_block_header_t with the length of the block.  Our full list of
 * steps are as follows:
 * 
 * 1) Add forward reference from previous free block to this free block.
 * 2) Add backref from this free block to previous free block.
 * 3) Other small stuff (set next pointer to NULL, etc.)
 * 
 * Basically, we are maintaining a linked list of freed blocks that we
 * may do with as we please.
 * 
 * TODO: we currently don't set the break to ungrow the heap space for this
 * process.  This should be added in the future.
 */
void po_free(void *ptr)
{
     
}

int main(int argc, char *argv[]) {
    return 0;
}
