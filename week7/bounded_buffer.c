/*
 * Define a bounded buffer containing records that describe the
 * results in a producer thread.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>

#include "sthread.h"
#include "bounded_buffer.h"
#include "semaphore.h"

/*
 * The bounded buffer data.
 */
struct _bounded_buffer {
    /* The maximum number of elements in the buffer */
    int length;

    /* The index of the first element in the buffer */
    int first;

    /* The number of elements currently stored in the buffer */
    int count;

    /* The values in the buffer */
    BufferElem *buffer;

    /* This defines the three semaphores needed. */
    /* The first semaphore is for the producers */
    Semaphore *producers;
    /* This semaphore is for the consumer. */
    Semaphore *consumers;
    /* This semaphore is used to prevent data from being overridden. */
    Semaphore *protect;
};

/*
 * For debugging, ensure that empty slots in the buffer are
 * set to a default value.
 */
static BufferElem empty = { -1, -1, -1 };

/*
 * Allocate a new bounded buffer.
 */
BoundedBuffer *new_bounded_buffer(int length) {
    BoundedBuffer *bufp;
    BufferElem *buffer;
    int i;

    /* Allocate the buffer */
    buffer = (BufferElem *) malloc(length * sizeof(BufferElem));
    bufp = (BoundedBuffer *) malloc(sizeof(BoundedBuffer));
    if (buffer == 0 || bufp == 0) {
        fprintf(stderr, "new_bounded_buffer: out of memory\n");
        exit(1);
    }

    /* Initialize */

    memset(bufp, 0, sizeof(BoundedBuffer));

    /* Here, we call the new_semaphore function to make a new semaphore
     * with the specified length. The producers one starts off with the
     * length inputted to the new_bounded_buffer. The consumers one starts
     * with 0 and then gets incremented later. The protect one starts with
     * 1. It's values act like a boolean.
     */
    bufp->producers = new_semaphore(length);
    bufp->consumers = new_semaphore(0);
    bufp->protect = new_semaphore(1);

    for (i = 0; i != length; i++)
        buffer[i] = empty;

    bufp->length = length;
    bufp->buffer = buffer;

    return bufp;
}

/*
 * Add an integer to the buffer.  Yield control to another
 * thread if the buffer is full.
 */
void bounded_buffer_add(BoundedBuffer *bufp, const BufferElem *elem) {

    /* First, we decrement producers semaphore since we have added a value
     * to the buffer. If the semaphore's value is 0, we will move to a new
     * thread (that is handled by the semaphore_wait function.)
     */
    semaphore_wait(bufp->producers);

    /* This changes the value of protect to 0. */ 
    semaphore_wait(bufp->protect);

    /* Now the buffer has space */
    bufp->buffer[(bufp->first + bufp->count) % bufp->length] = *elem;
    bufp->count++;

    /* This increments the consumers value. */
    semaphore_signal(bufp->consumers);

    /* This switches the value of protect back to 1. */
    semaphore_signal(bufp->protect);
}

/*
 * Get an integer from the buffer.  Yield control to another
 * thread if the buffer is empty.
 */
void bounded_buffer_take(BoundedBuffer *bufp, BufferElem *elem) {
    
    /* First, we decrement consumers semaphore since we have added a value
     * to the buffer. If the semaphore's value is 0, we will move to a new
     * thread (that is handled by the semaphore_wait function.)
     */
    semaphore_wait(bufp->consumers);

    /* This changes the value of protect to 0. */
    semaphore_wait(bufp->protect);

    /* Copy the element from the buffer, and clear the record */
    *elem = bufp->buffer[bufp->first];
    bufp->buffer[bufp->first] = empty;
    bufp->count--;
    bufp->first = (bufp->first + 1) % bufp->length;

    /* This increments the producers value. */
    semaphore_signal(bufp->producers);

    /* This switches the value of protect back to 1. */
    semaphore_signal(bufp->protect);
}

