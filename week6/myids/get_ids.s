.globl get_ids           /* Ignore everything beginning with a "." */
.text
    .align 4

get_ids:
    pushl %ebp          /* Push old base pointer. */
    movl %esp,%ebp      /* Current stack is new base. */
    movl 8(%ebp), %edx  /* Move the first address into the edx register */
    movl 12(%ebp), %ecx /* Move the second address into the ecx register */

get_uid:
    movl $24, %eax      /* Move 24 into eax. This is the UNIX API call for */
                            /* the uid. */
    int  $0x80          /* This makes the system call and stores the result */
                            /* in eax. */
    movl %eax, (%edx)   /* This moves the result of eax into the dereferenced */
                            /* edx, which held the first address. */

get_gid:
    movl $47, %eax      /* Move 47 into eax. This is the UNIX API call for */
                            /* the gid. */
    int  $0x80          /* This makes the system call and stores the result */
                            /* in eax. */
    movl %eax, (%ecx)   /* This moves the result of eax into the dereferenced */
                            /* ecx, which held the second address. */


return_get_ids:
    movl %ebp,%esp     /* Pop local stack. */
    popl %ebp          /* Pop old base of frame. */
    ret                /* Return statement */
    

