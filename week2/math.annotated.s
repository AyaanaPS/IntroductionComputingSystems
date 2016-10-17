/* This file contains IA32 assembly-language implementations of three
 * basic, very common math operations.
 *
 * The common theme of all three implementations is the use of conditionals,
 * which are the compare statements. These implementations avoid the use
 * of branching (jumping) because this is less efficient. This is due to 
 * pipelining, which is where an instruction is processed via a sequence of
 * stages, each performing one small portion of the required operations. This
 * outperforms branching because steps of the successive instructions are
 * overlapped to make the function more efficient. An example of this is the
 * cmovg operator, which both compares and moves data. Thus, the compiler would
 * want to avoid using the naive implementation because it is less time
 * efficient.
 */

    .text

/*====================================================================
 * int f1(int x, int y)
 * This implements the MIN math operation. In other words, this finds
 * the minimum of two numbers.
 */
.globl f1
f1:
	pushl	%ebp			/* Push old base pointer */
	movl	%esp, %ebp		/* Current stack is the new base */
	movl	8(%ebp), %edx	/* Argument1 put into edx */
	movl	12(%ebp), %eax 	/* Argument2 put into eax */
	cmpl	%edx, %eax 		/* Compares arguments 1 and 2 */
	cmovg	%edx, %eax 		/* If arg2 > arg1, move arg1 into eax. */
	popl	%ebp 			/* Pop old base of frame. */
	ret 					/* Return */


/*====================================================================
 * int f2(int x)
 * This implements the Absolute Value math operation.
 */
.globl f2
f2:
	pushl	%ebp			/* Push old base pointer */
	movl	%esp, %ebp		/* Current stack is the new base */
	movl	8(%ebp), %eax	/* Argument put into edx */
	movl	%eax, %edx		/* Move argument into edx */
	sarl	$31, %edx		/* Arithmetic shift right by 31 bits.
							 * Results in 0 if the input is positive and
							 * -1 if the input is negative.
							 */
	xorl	%edx, %eax		/* This does an xor between eax and the result of
							 * the above sarl. If the input is positive then
							 * the result of this is just the original number.
							 * If the input is negative, this results in the
							 * bit flipped version of the number.
							 */
	subl	%edx, %eax		/* This subtracts either a string of 0s from the
							 * above result (if the input is positive) or int
							 * subtracts a string of 1s (if the input is 
							 * negative). This results in a positive input not 
							 * changing. Furthermore, subtracting the flipped
							 * version of a negative number with a string of 1s
							 * simply results in negating the negative number.
							 * Thus, both numbers will be positive now.
							 */
	popl	%ebp			/* Pop old base of frame */
	ret 					/* Return */


/*====================================================================
 * int f3(int x)
 * This function returns the sign of the inputted number.
 * If the number is positive, the function returns 1.
 * If the number is negative, the function returns -1.
 * If the number is 0, the function returns 0.
 */
.globl f3
f3:
	pushl	%ebp			/* Push old base pointer */
	movl	%esp, %ebp		/* Current stack is the new base */
	movl	8(%ebp), %edx	/* Argument put into edx */
	movl	%edx, %eax		/* Move argument into edx */
	sarl	$31, %eax		/* Arithmetic shift right by 31 bits.
							 * Results in 0 if the input is positive and
							 * -1 if the input is negative.
							 */
	testl	%edx, %edx		/* This saves the original input value for later 
							 * comparison (in the cmovg line)
							 */
	movl	$1, %edx		/* This moves the value 1 into edx. */
	cmovg	%edx, %eax		/* This performs the comparison of edx to eax. 
							 * Note that because of the testl operator, cmovg
							 * compares the original value of edx and not 1. 
							 * If the original value is greater than 0 (i.e. 
							 * it is a positive number) then 1 will be moved
							 * into eax. If it is less than zero, eax will 
							 * remain -1 (it is -1 because of the sarl). If it
							 * is not 0, eax will remain 0 (it is 0 because of
							 * the sarl).
							 */
	popl	%ebp			/* Pop old base of frame */
	ret 					/* Return */
