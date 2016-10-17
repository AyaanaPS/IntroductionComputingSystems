#include <stdio.h>
#include <stdlib.h>

/* This states that the method is outside of the actual file. */
extern int fact(int arg);

int main(int argc, char **argv) {
    int input;
    int result;

    /* This performs input checking. If inputs are incorrect, 
     * a usage statement is printed. 
     */
    if(argc != 2)
    {
        fprintf(stderr, "usage %s positive_integer\n", argv[0]);
        exit(1);
    }
    else
    {
        input = atoi(argv[1]);
        if(input < 0)
        {
            fprintf(stderr, "usage %s positive_integer\n", argv[0]);
            exit(1);
        }
    }

    /* This calls and stores the result of calling fact. */
    result = fact(input);
    /* This prints the results. */
    printf("Factorial of %d: %d\n", input, result);
    return 0;
}