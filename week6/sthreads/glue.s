#
# Keep a pointer to the main scheduler context.
# This variable should be initialized to %esp;
# which is done in the __sthread_start routine.
#
        .data
        .align 4
scheduler_context:      .long   0

#
# __sthread_schedule is the main entry point for the thread
# scheduler.  It has three parts:
#
#    1. Save the context of the current thread on the stack.
#       The context includes only the integer registers
#       and EFLAGS.
#    2. Call __sthread_scheduler (the C scheduler function),
#       passing the context as an argument.  The scheduler
#       stack *must* be restored by setting %esp to the
#       scheduler_context before __sthread_scheduler is
#       called.
#    3. __sthread_scheduler will return the context of
#       a new thread.  Restore the context, and return
#       to the thread.
#
        .text
        .align 4
        .globl __sthread_schedule
__sthread_schedule:

        # Save the process state onto its stack

        pushl %edx      /* Save edx */
        pushl %ecx      /* Save ecx */
        pushl %eax      /* Save eax */
        pushl %ebp      /* Save ebp */
        pushl %ebx      /* Save ebx */
        pushl %esi      /* Save esi */
        pushl %edi      /* Save edi */
        pushl %esp      /* Save esp */
        pushfl          /* Save EFLAGS */

        # Call the high-level scheduler with the current context as an argument
        movl    %esp, %eax                      
        movl    scheduler_context, %esp         
        pushl   %eax
        call    __sthread_scheduler

        # The scheduler will return a context to start.
        # Restore the context to resume the thread.
__sthread_restore:
        
        # Restore the process state saved on the stack.

        movl %eax, %esp /* Move the context returned from scheduler_context */
        popfl           /* Restore EFLAGS */
        popl %esp       /* Restore esp */
        popl %edi       /* Restore edi */
        popl %esi       /* Restore esi */
        popl %ebx       /* Restore ebx */
        popl %ebp       /* Restore ebp */
        popl %eax       /* Restore eax */
        popl %ecx       /* Restore ecx */
        popl %edx       /* Restore edx */

        ret

#
# Initialize a process context, given:
#    1. the stack for the process
#    2. the function to start
#    3. its argument
# The context should be consistent with the context
# saved in the __sthread_schedule routine.
#
# A pointer to the newly initialized context is returned to the caller.
# (This is the stack-pointer after the context has been set up.)
#
# This function is described in more detail in sthread.c.
#
#
        .globl __sthread_initialize_context
__sthread_initialize_context:

        
        pushl %ebp          /* Push old base pointer. */
        movl %esp,%ebp      /* Current stack is new base. */

        movl 8(%ebp), %esp              /* Align the stack pointers */
        pushl 16(%ebp)                  /* Push the argument. */
        pushl $__sthread_finish         /* Push sthread_finish */
        pushl 12(%ebp)                  /* Push the function */

        pushl %edx                      /* Save edx */
        pushl %ecx                      /* Save ecx */
        pushl %eax                      /* Save eax */
        pushl %ebp                      /* Save ebp */
        pushl %ebx                      /* Save ebx */
        pushl %esi                      /* Save esi */
        pushl %edi                      /* Save edi */
        pushl %esp                      /* Save esp */
        pushfl                          /* Save EFLAGS */

        movl %esp, %eax                 /* Move the stack pointer into eax */
        movl %ebp, %esp                 /* Move the base pointer */
        pop %ebp                        /* Pop the base pointer */

        ret

#
# The start routine initializes the scheduler_context variable,
# and calls the __sthread_scheduler with a NULL context.
# The scheduler will return a context to resume.
#
        .globl __sthread_start
__sthread_start:
        # Remember the context
        movl    %esp, scheduler_context

        # Call the scheduler with no context
        pushl   $0
        call    __sthread_scheduler

        # Restore the context returned by the scheduler
        jmp     __sthread_restore
