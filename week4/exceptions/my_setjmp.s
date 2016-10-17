.globl my_setjmp
.globl my_longjmp
.text
    .align 4

/* The my_setjmp commands perform my implementation of the setjmp operation. */

my_setjmp:

    mov     4(%esp), %edx   /* Moves the jump buffer input into a register */
    mov     %esp, 0(%edx)   /* Moves the stack pointer into the 1st position*/   
    mov     %ebp, 4(%edx)   /* Moves the base pointer into the 2nd position*/
    mov     (%esp), %ecx    /* Moves the return address into a temp register */
    mov     %ecx, 8(%edx)   /* Moves the temp register into the 3rd pos */
    mov     %ebx, 12(%edx)  /* Moves the val at ebx register into the 4th pos */
    mov     %esi, 16(%edx)  /* Moves the val at esi register into the 5th pos */
    mov     %edi, 20(%edx)  /* Moves the val at edi register into the 6th pos */

    mov     $0, %eax        /* Moves 0 into the returning register. */
                            /* This is because 0 is the 'no issue' response. */
    ret

/* The my_longjmp commands perform my implementation of the longjmp operation */

my_longjmp:
    
    mov     4(%esp), %edx   /* Moves the jump buffer input into a register */
    mov     20(%edx), %edi  /* Moves the val at the 6th pos back into edi */
    mov     16(%edx), %esi  /* Moves the val at the 5th pos back into esi */
    mov     12(%edx), %ebx  /* Moves the val at the 4th pos back into ebx */
    mov     8(%edx), %ecx   /* Moves the val at the 3rd pos into a temp reg */
    mov     %ecx, (%esp)    /* Moves the temp reg val into the stack pointer */
    mov     4(%edx), %ebp   /* Moves the base pointer back into ebp */

    mov     8(%esp), %eax   /* Moves the second argument (the value) into eax */
    cmp     $0, %eax        /* Checks if the value = 0 */
    jne     longjmp_return  /* If the value does not equal 0 jump to */
                            /* longjmp_return */
    mov     $1, %eax        /* Else, move 1 into eax and proceed to */
                            /* longjmp_return */

/* This block of commands just returns the value at eax */

longjmp_return:
    ret
