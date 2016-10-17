#include "accum.h"
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

#include <pthread.h>
#include <time.h>


/* Adds "value" to *accum, returning the new total that is held in *accum as
 * a result of this particular addition (i.e. irrespective of other concurrent
 * additions that may be occurring).
 *
 * This function should be thread-safe, allowing calls from multiple threads
 * without generating spurious results.
 */
uint32_t add_to_accum(uint32_t *accum, uint32_t value) {


    uint32_t old = *accum;

    /* This call only updates accum if the accumulator's currently value
     * has not been update since the call. If it has not been updated (i.e. 
     * no other thread has accessed and changed it), the compare and swap
     * function swaps its value with the inputted new value, which is 
     * the sum of the accumulator's value and the inputted value. If it 
     * has been updated, it does not change.
     */
    int result = compare_and_swap(accum, old, old + value);
    if(result == 0)
    {
        printf("Couldn't update the accumulator\n");
    }
    // return *accum += value;
    return *accum;
}

