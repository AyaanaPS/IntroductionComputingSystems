#include "cmp_swap.h"
#include <stdlib.h>
#include <stdio.h>

void test_cmp_success()
{
    printf("TESTING A SUCCESSFUL COMPARE AND SWAP\n\n");
    uint32_t destVal = 5;
    uint32_t oldVal = 5;
    uint32_t newVal = 15;
    uint32_t result;

    result = compare_and_swap(&destVal, oldVal, newVal);
    printf("Value of destination: %d\n", destVal);
    printf("\tExpected value: %d\n\n", 15);
    printf("Value of Return: %d\n", result);
    printf("\tExpected value: %d\n\n", 1);
}

void test_cmp_fail()
{
    printf("TESTING AN UNSUCCESSFUL COMPARE AND SWAP\n\n");
    uint32_t destVal = 100;
    uint32_t oldVal = 200;
    uint32_t newVal = 300;
    uint32_t result;

    result = compare_and_swap(&destVal, oldVal, newVal);
    printf("Value of destination: %d\n", destVal);
    printf("\tExpected value: %d\n\n", 100);
    printf("Value of Return: %d\n", result);
    printf("\tExpected value: %d\n\n", 0);
}


int main()
{
    printf("\nTesting the Compare and Swap Functions\n");
    printf("---------------------------------------------------------------\n");
    printf("These tests ensure that the compare and swap function works.\n\n");
    test_cmp_success();

    printf("\n");

    test_cmp_fail();

    return 0;
}