.globl fact	           /* Ignore everything beginning with a "." */
.text
	.align 4
fact:
	pushl %ebp         /* Push old base pointer. */
	movl %esp,%ebp     /* Current stack is new base. */
	cmpl $0,8(%ebp)    /* See if arg equal zero. */
	jne fact_continue  /* If not, continue. */
	movl $1,%eax       /* Else result is one. */
	jmp fact_return
	.align 4           /* This is generated by the compiler to align */
                           /* code so that the CPU can process it better. */
fact_continue:
	movl 8(%ebp),%eax  /* The argument arg. */
	subl $1,%eax       /* Compute arg-1 for recursive call. */
	pushl %eax         /* Push arg-1 onto stack. */
	call fact          /* Make recursive call. */
	
fact_resume:		
	imull 8(%ebp),%eax /* Upon return multiply result by arg. */
	                   /* Result ends up in return register (eax). */
	jmp fact_return    /* We had to align return as a branch target. */
	                   /* This jumps around alignment space. */
	.align 4
fact_return:
	movl %ebp,%esp     /* Pop local stack. */
	popl %ebp          /* Pop old base of frame. */
	ret
