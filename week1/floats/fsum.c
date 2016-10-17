#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "ffunc.h"


/* This function takes an array of single-precision floating point values,
 * and computes a sum in the order of the inputs.  Very simple.
 */
float fsum(FloatArray *floats) {
    float sum = 0;
    int i;

    for (i = 0; i < floats->count; i++)
        sum += floats->values[i];

    return sum;
}


/* The following is a more precise and accurate methodology for computing
 * the sum of floating point numbers. This is known as the Kahan summation.
 * This approach increases the accuracy of the results by keeping track of
 * how much precision is lost to errors (i.e. a running compensation variable).
 * This compensation variable is then factored in to the final sum to give a
 * more precise sum.
 */

float my_fsum(FloatArray *floats) {

    float sum = 0.0;
    float compensation = 0.0;
    int i = 0;

    for(i = 0; i < floats->count; i++)
    {
        float y = floats->values[i] - compensation;
        float t = sum + y;
        compensation = (t - sum) - y; 
        //Above: (t - sum) cancels the high order part of y. Then, subtracting
        // the y recovers the low part of y. On the next iteration, the lost
        // low part will be added in another try.
        sum = t;
    }

    return sum;
}


int main() {
    FloatArray floats;
    float sum1, sum2, sum3, my_sum;

    load_floats(stdin, &floats);
    printf("Loaded %d floats from stdin.\n", floats.count);

    /* Compute a sum, in the order of input. */
    sum1 = fsum(&floats);

    /* Use my_fsum() to compute a sum of the values.  Ideally, your
     * summation function won't be affected by the order of the input floats.
     */
    my_sum = my_fsum(&floats);

    /* Compute a sum, in order of increasing magnitude. */
    sort_incmag(&floats);
    sum2 = fsum(&floats);

    /* Compute a sum, in order of decreasing magnitude. */
    sort_decmag(&floats);
    sum3 = fsum(&floats);


    /* %e prints the floating-point value in full precision,
     * using scientific notation.
     */
    printf("Sum computed in order of input:  %e\n", sum1);
    printf("Sum computed in order of increasing magnitude:  %e\n", sum2);
    printf("Sum computed in order of decreasing magnitude:  %e\n", sum3);
    printf("My sum:  %e\n", my_sum);
    

    return 0;
}

