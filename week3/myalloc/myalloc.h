/*! \file
 * Declarations for a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2009.
 * All rights reserved.
 */


/*! Specifies the size of the memory pool the allocator has to work with. */
extern int MEMORY_SIZE;

/* Defines the headers that separate the blocks in the memory pool */
typedef struct header {
    
    /* Negative size means the block is allocated, */
    /* positive size means the block is available. */
    int size;

    /* Points to the previous header (block) in memory. */
    struct header *prev;

    /* Points to the next header (block) in memory. */
    struct header *next;
} header ;

/* Initializes allocator state, and memory pool state too. */
void init_myalloc();

/* Splits the currently free block into two blocks */
void splitBlock(int address, int size);

/* Attempt to allocate a chunk of memory of "size" bytes. */
unsigned char * myalloc(int size);


/* Free a previously allocated pointer. */
void myfree(unsigned char *oldptr);

