/*! \file
 * Implementation of a simple memory allocator.  The allocator manages a small
 * pool of memory, provides memory chunks on request, and reintegrates freed
 * memory back into the pool.
 *
 * Adapted from Andre DeHon's CS24 2004, 2006 material.
 * Copyright (C) California Institute of Technology, 2004-2010.
 * All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>

#include "myalloc.h"


/*!
 * These variables are used to specify the size and address of the memory pool
 * that the simple allocator works against.  The memory pool is allocated within
 * init_myalloc(), and then myalloc() and free() work against this pool of
 * memory that mem points to.
 */
int MEMORY_SIZE;
unsigned char *mem;

/*!
 * This function initializes both the allocator state, and the memory pool.  It
 * must be called before myalloc() or myfree() will work at all.
 *
 * Note that we allocate the entire memory pool using malloc().  This is so we
 * can create different memory-pool sizes for testing.  Obviously, in a real
 * allocator, this memory pool would either be a fixed memory region, or the
 * allocator would request a memory region from the operating system (see the
 * C standard function sbrk(), for example).
 *
 * initialization happens in constant time because it only involves setting
 * values and not iterating.
 */
void init_myalloc() 
{

    /*
     * Allocate the entire memory pool, from which our simple allocator will
     * serve allocation requests.
     */
    mem = (unsigned char *) malloc(MEMORY_SIZE);
    if (mem == 0) {
        fprintf(stderr,
                "init_myalloc: could not get %d bytes from the system\n",
                MEMORY_SIZE);
        abort();
    }

    /* This makes the memory pool into one block, denoted by the hInit header */
    header *hInit = (header *) mem;

    /* The header size is set to the size of the payload it can hold */
    int initialSize = MEMORY_SIZE - sizeof(header);
    hInit->size = initialSize;

    /* Initially, there are no other blocks, so the prev and next are NULL */
    hInit->prev = NULL;
    hInit->next = NULL;
}

/*
 * Splits the block and the inputted address into two blocks.
 * The first block is then set to allocated with a payload size equal to the
 * inputted size.
 * The second block is set to available with a payload size equal to the
 * old capacity minus the size given to the first block minus the size of the
 * header.
 * The function then adjusts the next and previous values of the two headers.
 * 
 * This function runs in constant time because it simply involves looking
 * at the value of the inputted block's size and creating a new block.
 */
void splitBlock(int address, int size)
{   
    /* This is the header at block of the inputted address. */
    header * current = (header *) (mem + address);
    int curSize = current->size;

    /* It's size is set to the size of the payload it will carry. */
    /* This size is negative because it has been allocated */
    current->size = (-1) * size;

    /* 
     * The new block's header is at a location that is away from the beginning
     * of the old header by the size of the payload plus the size of the first 
     * blocks header.
     */
    int newLocation = address + sizeof(header) + size;
    header * new = (header *) (mem + newLocation);

    /* 
     * The new block's payload size is set to the remaining size minus the
     * size of the header.
     */
    new->size = curSize - size - sizeof(header);

    /*
     * This adjusts the next and previous pointers of current and next */
    new->next = current->next;
    if(new->next != NULL)
    {
        (new->next)->prev = new;
    }
    new->prev = current;
    current->next = new;
}

/*!
 * Attempt to allocate a chunk of memory of "size" bytes.  Return 0 if
 * allocation fails.
 * The memory allocation algorithm implements a best fit approach.
 * This is because doing best fit avoids uneccessary and extra memory 
 * fragmentation, which in the long run, increases memory utilization.
 * The best fit approach is ideal in scenarios with a lot of memory
 * being allocated and then deallocated and then allocated in various
 * addresses. i.e. best fit is better for scenarios in which the memory pool
 * has holes of free memory and remaining errors of allocated memory.
 * This is because the best fit approach will check all the holes to fill the
 * one whose size is closest, rather than just the first one that's free. 
 * This leaves larger sizes for larger payloads.
 * The best fit approach is not ideal in cases where memory is allocated
 * for payloads of equal or similar sizes and then deallocated and 
 * reallocated with payloads of similar sizes again. This is because
 * it is now less efficient for best fit to look through every free block, since
 * the payloads all have equal/similar sizes and should just go into the
 * first free block found.
 *
 * The myalloc function runs in linear time, because it looks over all
 * of the blocks before choosing a block to allocate the memory to.
 */
unsigned char *myalloc(int size) 
{
    /* curAddress keeps track of where in the memory pool we are looking */
    int curAddress = 0;

    /* bestAddress holds the best location in memory encountered so far.
     * This is the address that holds the minimum size greater than the
     * desired payload size.
     */
    int bestAddress = -1;

    /* This allows us to change the value of bestAddress */
    int minGreaterSize = MEMORY_SIZE;

    /* Loops through all of the blocks in the memory pool. */
    while(curAddress < MEMORY_SIZE)
    {
        int curSize = * (int *) (mem + curAddress);

        /* if the block is already allocated we update address to the address
         * of the nextBlock
         */
        if(curSize < 0)
        {
            curAddress = curAddress + sizeof(header) - (curSize);
        }

        else
        {
            /* if the block has the capacity to hold the payload and a new
             * header:
             */
            if(curSize > size + sizeof(header))
            {
                /* if the capacity is smaller than the minimum size encountered
                 * so far:
                 */
                if(curSize < minGreaterSize)
                {
                    /* Update the best address to the current address */
                    bestAddress = curAddress;

                    /* Change the minimum size encountered to the curSize */
                    minGreaterSize = curSize;
                }

            }

            /* Update the current address to the address of the next block */
            curAddress = curAddress + sizeof(header) + curSize;
        }
    }

    /* If best address is still -1, we haven't found a suitable block */
    if(bestAddress < 0)
    {
        fprintf(stderr, "myalloc: cannot service request of size %d\n", size);
        return (unsigned char *) 0;
    }
    else
    {
        /* Split the block */
        splitBlock(bestAddress, size);

        /* Return the address of where the payload should be stored */
        unsigned char * result = mem + bestAddress + sizeof(header);
        return result;
    }
}

/*!
 * Free a previously allocated pointer.  oldptr should be an address returned by
 * myalloc().
 * 
 * This function implements coalescing to increase memory utilization.
 * Coalescing merges blocks that are newly freed, with blocks next to or
 * before it (or both) that are free. This makes memory allocation more
 * efficient, by avoiding over fragmentation of memory without putting
 * memory back together.
 */
void myfree(unsigned char *oldptr) 
{
    /* This is the header of the block to be freed */
    header * current = (header *) (oldptr - sizeof(header));
    
    /* The size is freed by taking the absolute value of the size */
    /* This takes care of the case where neither the next or previous
     * blocks are free */
    current->size = abs(current->size);

    /* These flags are raised if the next or previous block exist and
     * are free.
     * A raised flag has a value of 1.
     */
    int nextFree = 0, prevFree = 0;
    
    /* This checks if there is a next block */
    if(current->next != NULL)
    {
        /* This checks if the next block is free */
        if((current->next)->size > 0)
        {
            nextFree = 1; 
        }
    }

    /* This checks if there is a previous block */
    if(current->prev != NULL)
    {
        /* This checks if the previous block is free */
        if((current->prev)->size > 0)
        {
            prevFree = 1;
        }
    }

    
    /* 
     * If both next and prev are free, we have to merge all
     * 3 blocks and remove any pointers to current and current->next.
     * This is because we must remove the current and current->next blocks.
     */
    if(nextFree == 1 && prevFree == 1)
    {
        /* Thus, previous eats current and current->next. */
        header * prev = current->prev;
        prev->size += (sizeof(header) + current->size + sizeof(header) + 
            (current->next)->size);
        prev->next = (current->next)->next;
        if(prev->next != NULL)
        {
            (prev->next)->prev = prev;
        }
    }

    
    /*
     * If only the next block is free, we have to merge current and
     * current->next. We then remove any pointers to current->next,
     * because we are removing that block.
     */
    else if(nextFree == 1)
    {
        /* current eats current->next */
        current->size += (sizeof(header) + (current->next)->size);
        current->next = (current->next)->next;
        if(current->next != NULL)
        {
            (current->next)->prev = current;
        }
    }

    
    /*
     * If only the previous block is free, we have to merge previous and
     * current. We then remove any pointers to current, because we
     * are removing that block.
     */
    else if(prevFree == 1)
    {
        /* previous eats current */
        header * prev = current->prev;
        prev->size += (sizeof(header) + current->size);
        prev->next = current->next;
        if(prev->next != NULL)
        {
            (prev->next)->prev = prev;
        }
    }

}

