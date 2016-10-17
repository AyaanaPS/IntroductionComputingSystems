#include <stdio.h>
#include <stdlib.h>

/* This states that the method is outside of the actual file. */
extern void get_ids(int * address1, int * address2);

int main(int argc, char **argv) {
    /* These are the two int's to store the result in. */
    int uid, gid;

    /* We pass the function the address of the above two ints. */
    get_ids(&uid, &gid);

    /* This prints out the result. */
    printf("User ID is %d. Group ID is %d.\n", uid, gid);

    return 0;
}