#include <stdio.h>
#include "sthread.h"


/*
 * Thread 1 divides the inputted argument by 2.
 * Thread 2 multiplies the inputted argument by 3.
 *
 * This test has two threads that both take in an argument. 
 * This ensures that the argument is positioned properly in
 * the thread's machine context, such that the functions can
 * retrieve and use it.
 */

/*! This thread-function divides the input by 2. */
static void loop1(void *arg) {
    int x = 25;
    while(x >= 0) {
        printf("Dividing by 2\n");
        *((int *)arg) = *((int *)arg)/2;
        printf("New Num: %d\n", *((int *)arg));
        sthread_yield();
        x = x - 1;
    }
}

/*! This thread-function mulitplies the input by 3. */
static void loop2(void *arg) {
    int x = 25;
    while(x >= 0) {
        printf("Multiplying by 3\n");
        *((int *)arg) = *((int *)arg) * 3;
        printf("New Num: %d\n", *((int *)arg));
        sthread_yield();
        x = x - 1;
    } 
}

/*
 * The main function starts the two loops in separate threads,
 * the start the thread scheduler.
 */
int main(int argc, char **argv) {
    int num = 100;
    sthread_create(loop1, (void *) &num);
    sthread_create(loop2, (void *) &num);
    sthread_start();
    return 0;
}