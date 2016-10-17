/*
 * General implementation of semaphores.
 *
 *--------------------------------------------------------------------
 * Adapted from code for CS24 by Jason Hickey.
 * Copyright (C) 2003-2010, Caltech.  All rights reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>

#include "sthread.h"
#include "semaphore.h"


/*
 * The semaphore data structure contains a value indicating the number
 * of threads it contains and a queue that holds the threads.
 */
struct _semaphore {

    /* The number of threads. */
    int i;

    /* The queue of threads.  */
    Queue *threadQueue;
};

/************************************************************************
 * Top-level semaphore implementation.
 */

/*
 * Allocate a new semaphore.  The initial value of the semaphore is
 * specified by the argument.
 */
Semaphore *new_semaphore(int init) {

    /* This creates a new semophore and a new queue using malloc. */
    Semaphore *semp;
    semp = malloc(sizeof(Semaphore));
    Queue *newQueue = malloc(sizeof(Queue));
    newQueue->head = NULL;
    newQueue->tail = NULL;

    /* This initializes the values in the semaphore. */
    semp->i = init;
    semp->threadQueue = newQueue;

    return semp;
}

/*
 * Decrement the semaphore.
 * This operation is atomic, and it blocks iff the semaphore is zero.
 */
void semaphore_wait(Semaphore *semp) {

    /* First, we have to lock because we don't want any other threads to 
     * change the semaphore.
     */
    __sthread_lock();

    while(semp->i == 0)
    {
        /* This gets and appends the current thread to the semaphore's
         * threadQueue.
         */
        queue_append(semp->threadQueue, sthread_current());

        /* It then blocks the current thread */
        sthread_block(0);

        /* We have to relock the thread because the block function
         * unlocks the thread at the end of the function. We do this
         * to ensure that we don't double unlock in this function.
         */
        __sthread_lock();
    }

    /* We decrement the value of the semaphore. */
    semp->i--;

    /* We then unlock the thread so we can proceed. */
    __sthread_unlock();
}

/*
 * Increment the semaphore.
 * This operation is atomic.
 */
void semaphore_signal(Semaphore *semp) {

    /* First, we have to lock because we don't want any other threads to 
     * change the semaphore.
     */
    __sthread_lock();

    /* We increment the value of the semaphore. */
    semp->i++;

    Thread *temp;

    /* If the queue is not empty we do the following: */
    if(!queue_empty(semp->threadQueue))
    {
        /* We take off the thread from the queue */
        temp = queue_take(semp->threadQueue);

        /* We then unblock it since the queue is full of blocked threads and
         * we now need to use this thread.
         */
        sthread_unblock(temp, 0);

        /* We have to relock the thread because the unblock function
         * unlocks the thread at the end of the function. We do this
         * to ensure that we don't double unlock in this function.
         */
        __sthread_lock();
    }

    /* We then unlock the thread so we can proceed. */
    __sthread_unlock();
}







