/*
 * An implementation of malloc() and free() as per exercise 7-2 in
 * TLPI.
 * 
 * Copyright 2012, Paul Osborne
 *
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

typedef struct block_header {
    size_t block_length;
    struct block_header * prev_free_chunk;
    struct block_header * next_free_chunk;
} block_header_t;

static block_header_t * free_ll_head = NULL;

#define FREE_BLOCK_HEADER(ptr) ((block_header_t *) ((unsigned long)ptr - sizeof(block_header_t)))
#define FREE_BLOCK_MEMORY(ptr) ((void *)((unsigned long)ptr + sizeof(block_header_t)))

int ll_append(block_header_t * prev, block_header_t * next)
{
	prev->next_free_chunk = next;
	next->prev_free_chunk = prev;
	return 0;
}

/*
 * Remove the specified list_item from the free nodes linked list
 */
int ll_remove(block_header_t * list_item)
{
	/* make prev and next point to each other, if not NULL */
	if (!list_item->prev_free_chunk) {
		if (list_item->next_free_chunk) {
			free_ll_head = list_item->next_free_chunk;
		} else {
			free_ll_head = NULL;
		}
	} else {
		list_item->prev_free_chunk->next_free_chunk = list_item->next_free_chunk;
	}

	if (list_item->next_free_chunk) {
		list_item->next_free_chunk->prev_free_chunk = list_item->prev_free_chunk;
	}
	return 0;
}

/*
 * Replace the first list_item with the second list_item in the
 * freed nodes linked list.
 */
int ll_replace(block_header_t *old_item, block_header_t *new_item)
{
	/* update references that must point to new item */
	if (old_item->prev_free_chunk)
		old_item->prev_free_chunk->next_free_chunk = new_item;
	else
		free_ll_head = new_item;

	if (old_item->next_free_chunk)
		old_item->next_free_chunk->prev_free_chunk = new_item;

	/* update references for the new item itself */
	new_item->prev_free_chunk = old_item->prev_free_chunk;
	new_item->next_free_chunk = old_item->next_free_chunk;
	return 0;
}


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
    block_header_t *current_header, *new_free_header, *block_header;
    void *memory_to_use = NULL;
    const size_t size_plus_header = size + sizeof(block_header_t);
    new_free_header = block_header = NULL;
    current_header = free_ll_head;
    while (current_header) {
        if (current_header->block_length >= size) {
            memory_to_use = FREE_BLOCK_MEMORY(current_header);
            if (current_header->block_length > size_plus_header) {
                new_free_header = (block_header_t *)((unsigned long)memory_to_use + size);
                new_free_header->block_length = current_header->block_length - size;
                ll_replace(current_header, new_free_header);
             } else {
            	 ll_remove(current_header);
             }
            return memory_to_use;
        } else {
            current_header = current_header->next_free_chunk;
        }
    }

    // if we are here, we didn't get what we needed from scanning the
    // freed list, tack on to the end of memory
    size_t expand_size;
    if (current_header) {
    	expand_size = (size_plus_header - current_header->block_length);
    } else {
    	expand_size = size_plus_header;
    }
    block_header = (block_header_t *)sbrk(expand_size);
    memory_to_use = FREE_BLOCK_MEMORY(block_header);
    if (!block_header) {
    	/* failed to allocate more memory, return NULL */
    	return NULL;
    }

    /* current_header points to end of current LL unless first */
	block_header->next_free_chunk = NULL;
	block_header->prev_free_chunk = NULL;
	block_header->block_length = size;
    return memory_to_use;
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
 *
 * TODO: we currently don't scan the entire linked list or manage blocks in
 * such a way that we can avoid fragmentation.  For instance, we leave a bunch
 * of gaps on the table when running simple_free_test().
 */
void po_free(void *ptr)
{
	/* we just insert this item into the front of the linked list.  Note
	 * that we don't touch the length as we assume this is still correct. */
	block_header_t * new_freed_header = FREE_BLOCK_HEADER(ptr);
	new_freed_header->prev_free_chunk = NULL;
	if (free_ll_head) {
		new_freed_header->next_free_chunk = free_ll_head;
		free_ll_head->prev_free_chunk = ptr;
		free_ll_head = new_freed_header;
	} else {
		new_freed_header->next_free_chunk = NULL;
		free_ll_head = new_freed_header;
	}
}

void malloc_only_test() {
	int i, j;
	char * buf;
	char * oldbuf = NULL;
	char * bufs[100 - 2];
	for (i = 2; i < 100; i++) {
		buf = (char *)po_malloc(i);
		if (oldbuf) {
			printf("  diff: %lu\n", (unsigned long)buf - (unsigned long)oldbuf);
		}
		oldbuf = buf;
		bufs[i - 2] = buf;
		for (j = 0; j < i - 1; j++) {
			buf[j] = '0' + (i % 10); // put digit into string
		}
		buf[i - 1] = '\0';  // nul terminate
	}

	for (i = 0; i < 100 - 2; i++) {
		printf("%d -> %s\n", i, bufs[i]);
	}
}

void simple_free_test() {
	/* do some mallocs, free it all, do the mallocs again (sbrk should not move) */
	int i, j, len;
	char * bufs[100];
	void * cur_brk;
	for (i = 0; i < 100; i++) {
		len = 100 + i;
		bufs[i] = po_malloc(len);
		for (j = 0; j < len - 1; j++) {
			bufs[i][j] = '0' + (i % 10);
		}
		bufs[i][len - 1] = '\0';
	}
	for (i = 0; i < 100; i++) {
		printf("%02d [0x%08lX]: %s\n", i, (unsigned long)bufs[i], (char *)bufs[i]);
	}
	cur_brk = sbrk(0);
	for (i = 0; i < 100; i++) {
		po_free(bufs[i]);
	}
	for (i = 0; i < 100; i++) {
		len = 100 + i;
		bufs[i] = po_malloc(len);
		for (j = 0; j < len - 1; j++) {
			bufs[i][j] = '0' + (i % 10);
		}
		bufs[i][len - 1] = '\0';
	}
	printf("== sbrk(0): bfr 0x%08lX, after 0x%08lX ==\n",
			(unsigned long)cur_brk,
			(unsigned long)sbrk(0));
	for (i = 0; i < 100; i++) {
		printf("%02d [0x%08lX]: %s\n", i, (unsigned long)bufs[i], (char *)bufs[i]);
	}

}

int main(int argc, char *argv[]) {
	simple_free_test();
	// malloc_only_test();
	return 0;
}
