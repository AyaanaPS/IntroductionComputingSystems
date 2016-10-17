#include "my_setjmp.h"
#include <stdlib.h>
#include <stdio.h>

/* The neighbor1 and neighbor2 variables serve as a test for neighboring
 * memory corruption. Their values are checked in the main function. 
 */
int neighbor1 = 10;

/* This initializes the jump buffer */
static jmp_buf env;
int neighbor2 = 20;

/* These are the function declarations for the 'Jump over multiple functions
 * test'
 */
int f(int x);
int g(int x);
int h(int x);

/* These tests ensure that a call like longjmp(buf, 0) causes setjmp to return
 * 1 and a call like longjmp(buf, n) for n != 0 returns n.
 */
void test_setjmpReturn()
{
    printf("TESTING SETJMP RETURN VALUES\n");
    printf("These tests ensure that a call like longjmp(buf, 0) causes \n"); 
    printf("setjmp to return 1 and a call like longjmp(buf, n) for n != 0 \n");
    printf("returns n.\n\n");

    int testValues[] = {1, 10, 0, 17};
    int i, n, result, counter;

    for(i = 0; i < 4; i++)
    {
        n = testValues[i];
        counter = 0;
        result = setjmp(env);

        if(result == 0)
        {   

            if(counter > 0)
            {
                printf("FAILED for value %d\n", n);
            }
            counter ++;
            longjmp(env, n);
        }
        else
        {
            if(result == n)
            {
                printf("PASSED for value %d\n", n);
            }
            else if(n == 0 && result == 1)
            {
                printf("PASSED for value %d\n", n);
            }
            else
            {
                printf("FAILED for value %d\n", n);
            }
        }
        
    }
}

/* This function tests that longjmp is only called when needed and that
 * when longjmp isn't called, setjmp returns 0, which indicates no issue.
 */
void test_divisionZeroError()
{
    printf("TESTING LONGJMP Calls  \n");
    printf("This test ensures that longjmp is only called when needed and \n");
    printf("that when longjmp isn't called, setjmp returns 0 (indicating no\n");
    printf(" issue.\n\n");

    int testDenominators[] = {50, 2, 0, 3};
    int i, n, counter;

    for(i = 0; i < 4; i++)
    {
        n = testDenominators[i];
        counter = 0;

        if(setjmp(env) == 0)
        {
            if(n == 0)
            {
                if(counter > 0)
                {
                    printf("FAILED for value %d\n", n);
                }
                else
                {
                    counter ++;
                    longjmp(env, 110);
                }
            }
            else
            {
                printf("PASSED for value %d\n", n);
            }
        }
        else
        {
            if(n == 0)
            {
                printf("PASSED for value %d\n", n);
            }
            else
            {
                printf("FAILED for value %d\n", n);
            }

        }
    }
}

/* These functions are for the 'Jump over multiple functions test'. */
int f(int x)
{
    if(setjmp(env) == 0)
    {
        return g(3 * x);
    }
    else
    {
        return -1;
    }
}

int g(int x)
{
    return h(15 - x);
}

/* This functions contains the longjmp call */
int h(int x)
{
    if(x < 5)
    {
        longjmp(env, 1);
    }

    return (x * x);
}

/* These tests ensure that a longjmp call look at the correct setjmp,
 * even if it is not in the same function.
 */
void testMultipleFunctions()
{
    printf("TESTING LONGJMP OVER MULTIPLE FUNCTIONS\n");
    printf("These tests ensure that a longjmp call can look at the \n"); 
    printf("correct setjmp, even if it is not in the same function.\n\n");


    int result;
    int i, n;
    int testValues[] = {-5, 0, 5, 10};

    for(i = 0; i < 4; i++)
    {
        n = testValues[i];
        result = f(n);
        printf("\nThe result for n = %d is %d\n", n, result);
        if(n == 5 || n == 10)
        {
            if(result == -1)
            {
                printf("PASSED for value %d\n", n);
            }
            else
            {
                printf("FAILED for value %d\n", n);
            }
        }
        else
        {
            if(result == -1)
            {
                printf("FAILED for value %d\n", n);
            }
            else
            {
                printf("PASSED for value %d\n", n);
            }
        }
    }
}

/* This runs the test and formats them nicely. */
int main()
{
    printf("-------------------------------------------------------------\n");
    printf("Testing SetJMP and LongJMP\n");
    printf("-------------------------------------------------------------\n\n");

    printf("The following tests ensure that setjmp and longjmp \n");
    printf("are working as desired.\n");

    printf("\n-----------------------------------------------------------\n");
    printf("\nTest 1\n\n");
    test_setjmpReturn();

    printf("\n-----------------------------------------------------------\n");
    printf("\nTest 2\n\n");
    test_divisionZeroError();

    printf("\n-----------------------------------------------------------\n");
    printf("\nTest 3\n\n");
    testMultipleFunctions();

    printf("\n-----------------------------------------------------------\n");
    printf("\nTest 4\n\n");
    printf("TESTING TO ENSURE NO NEIGHBOR MEMORY CORRUPTION\n");
    printf("These tests ensure that no neighbor memory corruption occurs \n");
    printf("during setjmp and longjmp calls.\n\n");
    if(neighbor1 == 10 && neighbor2 == 20)
    {
        printf("PASSED\n");
    }
    else
    {
        printf("FAILED\n");
    }

    printf("\n-----------------------------------------------------------\n");

    return 0;


}