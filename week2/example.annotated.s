    .file   "example.c"
    .section    .text.unlikely,"ax",@progbits
.LCOLDB0:
    .text
.LHOTB0:
    .p2align 4,,15
    .globl  ex
    .type   ex, @function
ex:
.LFB0:
    .cfi_startproc
    movl    8(%esp), %eax   // This step moves arg2 into the reg 
    subl    12(%esp), %eax  // This step subtracts arg3 from the reg (b-c)
    imull   4(%esp), %eax   // This step multiplies arg1 with the reg: a(b-c)
    addl    16(%esp), %eax  // This step adds arg4 to the reg: a(b-c) + d
    ret
    .cfi_endproc
.LFE0:
    .size   ex, .-ex
    .section    .text.unlikely
.LCOLDE0:
    .text
.LHOTE0:
    .ident  "GCC: (GNU) 4.9.2 20150304 (prerelease)"
    .section    .note.GNU-stack,"",@progbits
