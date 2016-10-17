#include <stdio.h>
#include "sthread.h"


/*
 * Thread 1 prints "Should run 10 times.".
 * Thread 2 prints "Should run 15 times.".
 * Thread 3 prints "Should run 20 times.".
 * Thread 4 prints "Should run 25 times."
 *
 * This test has four threads that each run for a different length of time.
 * This makes sure the scheduler works as desired.
 */

/*! This thread-function prints "Should run 10 times." over and over again! */
static void loop1(void *arg) {
    int x = 10;
    while(x >= 0) {
        printf("Should run 10 times.\n");
        sthread_yield();
        x -= 1;
    }
}

/*! This thread-function prints "Should run 15 times." over and over again! */
static void loop2(void *arg) {
    int x = 15;
    while(x >= 0) {
        printf("Should run 15 times.\n");
        sthread_yield();
        x -= 1;
    }
}

/*! This thread-function prints "Should run 20 times." over and over again! */
static void loop3(void *arg) {
    int x = 20;
    while(x >= 0) {
        printf("Should run 20 times.\n");
        sthread_yield();
        x -= 1;
    }
}

/*! This thread-function prints "Should run 25 times." over and over again! */
static void loop4(void *arg) {
    int x = 25;
    while(x >= 0) {
        printf("Should run 25 times.\n");
        sthread_yield();
        x -= 1;
    }
}

/*
 * The main function starts the four loops in separate threads,
 * the start the thread scheduler.
 */
int main(int argc, char **argv) {
    sthread_create(loop1, NULL);
    sthread_create(loop2, NULL);
    sthread_create(loop3, NULL);
    sthread_create(loop4, NULL);
    sthread_start();
    return 0;
}



