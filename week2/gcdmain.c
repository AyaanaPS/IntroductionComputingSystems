#include <stdio.h>
#include <stdlib.h>

/* This states that the method is outside of the actual file. */
extern int gcd(int arg1, int arg2);

int main(int argc, char **argv) {
    int input1, input2;
    int result;

    /* This performs input checking. If inputs are incorrect,
     * a usage statement is printed.
     */

    if(argc != 3)
    {
        fprintf(stderr, "usage %s arg1 arg2 (both positive)\n", argv[0]);
        exit(1);
    }
    else
    {
        input1 = atoi(argv[1]);
        input2 = atoi(argv[2]);
        if(input1 <= 0 || input2 <= 0)
        {
            fprintf(stderr, "usage %s arg1 arg2 (both >= 0)\n", argv[0]);
            exit(1);
        }
    }

    /* The following ensures that the values are passed into the gcd
     * function in the correct order. The correct order is largest first.
     */
    if(input1 > input2)
    {
        result = gcd(input1, input2);
    }
    else
    {
        result = gcd(input2, input1);
    }
    
    /* This prints the results. */
    printf("GCD of %d and %d: %d\n", input1, input2, result);
    return 0;
}