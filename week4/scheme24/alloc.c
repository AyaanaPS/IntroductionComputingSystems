#include "alloc.h"
#include "ptr_vector.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>


/*! Change to #define to output garbage-collector statistics. */
#define GC_STATS

/*!
 * Change to #undef to cause the garbage collector to only run when it has to.
 * This dramatically improves performance.
 *
 * However, while testing GC, it's easiest if you try it all the time, so that
 * the number of objects being manipulated is small and easy to understand.
 */
#define ALWAYS_GC


/* Change to #define for other verbose output. */
#undef VERBOSE


void free_value(Value *v);
void free_lambda(Lambda *f);
void free_environment(Environment *env);

void mark_value(Value *v);
void mark_lambda(Lambda *f);
void mark_environment(Environment *env);

void mark_eval_stack(PtrStack *evalStack);

void sweep_values();
void sweep_lambdas();
void sweep_environments();

/*!
 * A growable vector of pointers to all Value structs that are currently
 * allocated.
 */
static PtrVector allocated_values;


/*!
 * A growable vector of pointers to all Lambda structs that are currently
 * allocated.  Note that each Lambda struct will only have ONE Value struct that
 * points to it.
 */
static PtrVector allocated_lambdas;


/*!
 * A growable vector of pointers to all Environment structs that are currently
 * allocated.
 */
static PtrVector allocated_environments;


#ifndef ALWAYS_GC

/*! Starts at 1MB, and is doubled every time we can't stay within it. */
static long max_allocation_size = 1048576;

#endif


void init_alloc() {
    pv_init(&allocated_values);
    pv_init(&allocated_lambdas);
    pv_init(&allocated_environments);
}


/*!
 * This helper function prints some helpful details about the current allocation
 * status of the program.
 */
void print_alloc_stats(FILE *f) {
    /*
    fprintf(f, "Allocation statistics:\n");
    fprintf(f, "\tAllocated environments:  %u\n", allocated_environments.size);
    fprintf(f, "\tAllocated lambdas:  %u\n", allocated_lambdas.size);
    fprintf(f, "\tAllocated values:  %u\n", allocated_values.size);
    */

    fprintf(f, "%d vals \t%d lambdas \t%d envs\n", allocated_values.size,
        allocated_lambdas.size, allocated_environments.size);
}


/*!
 * This helper function returns the amount of memory currently being used by
 * garbage-collected objects.  It is NOT the total amount of memory being used
 * by the interpreter!
 */ 
long allocation_size() {
    long size = 0;
    
    size += sizeof(Value) * allocated_values.size;
    size += sizeof(Lambda) * allocated_lambdas.size;
    size += sizeof(Value) * allocated_environments.size;
    
    return size;
}


/*!
 * This function heap-allocates a new Value struct, initializes it to be empty,
 * and then records the struct's pointer in the allocated_values vector.
 */
Value * alloc_value(void) {
    Value *v = malloc(sizeof(Value));
    memset(v, 0, sizeof(Value));

    pv_add_elem(&allocated_values, v);

    return v;
}


/*!
 * This function frees a heap-allocated Value struct.  Since a Value struct can
 * represent several different kinds of values, the function looks at the
 * value's type tag to determine if additional memory needs to be freed for the
 * value.
 *
 * Note:  It is assumed that the value's pointer has already been removed from
 *        the allocated_values vector!  If this is not the case, serious errors
 *        will almost certainly occur.
 */
void free_value(Value *v) {
    assert(v != NULL);

    /*
     * If value refers to a lambda, we don't free it here!  Lambdas are freed
     * by the free_lambda() function, and that is called when cleaning up
     * unreachable objects.
     */

    if (v->type == T_String || v->type == T_Atom || v->type == T_Error)
        free(v->string_val);

    free(v);
}



/*!
 * This function heap-allocates a new Lambda struct, initializes it to be empty,
 * and then records the struct's pointer in the allocated_lambdas vector.
 */
Lambda * alloc_lambda(void) {
    Lambda *f = malloc(sizeof(Lambda));
    memset(f, 0, sizeof(Lambda));

    pv_add_elem(&allocated_lambdas, f);

    return f;
}


/*!
 * This function frees a heap-allocated Lambda struct.
 *
 * Note:  It is assumed that the lambda's pointer has already been removed from
 *        the allocated_labmdas vector!  If this is not the case, serious errors
 *        will almost certainly occur.
 */
void free_lambda(Lambda *f) {
    assert(f != NULL);

    /* Lambdas typically reference lists of Value objects for the argument-spec
     * and the body, but we don't need to free these here because they are
     * managed separately.
     */

    free(f);
}


/*!
 * This function heap-allocates a new Environment struct, initializes it to be
 * empty, and then records the struct's pointer in the allocated_environments
 * vector.
 */
Environment * alloc_environment(void) {
    Environment *env = malloc(sizeof(Environment));
    memset(env, 0, sizeof(Environment));

    pv_add_elem(&allocated_environments, env);

    return env;
}


/*!
 * This function frees a heap-allocated Environment struct.  The environment's
 * bindings are also freed since they are owned by the environment, but the
 * binding-values are not freed since they are externally managed.
 *
 * Note:  It is assumed that the environment's pointer has already been removed
 *        from the allocated_environments vector!  If this is not the case,
 *        serious errors will almost certainly occur.
 */
void free_environment(Environment *env) {
    int i;

    /* Free the bindings in the environment first. */
    for (i = 0; i < env->num_bindings; i++) {
        free(env->bindings[i].name);
        /* Don't free the value, since those are handled separately. */
    }
    free(env->bindings);

    /* Now free the environment object itself. */
    free(env);
}

/*
 * This function marks the necessary parts of the passed in value. 
 * First, it asserts that the value is not null and that it is not marked. 
 * Then, it sets the marked value to 1 and then it checks the type of
 * the value. If the type is Lambda, then mark_lambda is called on the
 * lambda_val. If the type is ConsPair, then mark_value is called on both
 * p_car and p_cdr.
 */

void mark_value(Value *v) {
    assert(v != NULL);

    if(v->marked != 1)
    {
        v->marked = 1;

        if(v->type == T_Lambda)
        {
            mark_lambda(v->lambda_val);
        }

        else if (v->type == T_ConsPair)
        {
            mark_value(v->cons_val.p_car);
            mark_value(v->cons_val.p_cdr);
        }
    }
}

/*
 * This function marks the necessary parts of the passed in lambda. 
 * First, it asserts that the lambda is not null. 
 * Then, it sets the marked value to 1 and then it checks the type of
 * the lambda. It also marks the parent environment of the lambda.
 * If it isn't a native implementation, we mark the arg_spec and
 * body value. 
 */
void mark_lambda(Lambda *f) {
    assert(f != NULL);

    f->marked = 1;

    if(f->native_impl == 0)
    {
        mark_value(f->arg_spec);
        mark_value(f->body);
    }

    mark_environment(f->parent_env);
}

/*
 * This function marks the necessary parts of the passed in environment. 
 * First, it asserts that the environment is not null. 
 * Then, it sets the marked value to 1. Next, if the parent environment
 * is not NULL, it marks that. Then, it iterates through the bindings
 * and marks each value.
 */
void mark_environment(Environment *env) {
    assert(env != NULL);
    int i;

    env->marked = 1;

    if(env->parent_env != NULL)
    {
        mark_environment(env->parent_env);
    }

    for(i = 0; i < env->num_bindings; i++)
    {
        mark_value(env->bindings[i].value);
    }
}

/*
 * This function marks the evaluation stack, which is made up of
 * EvaluationContexts. The function goes through each evaluation
 * context in the stack and checks if the first 3 values of the 
 * evaluation context are null. If not, then they are marked with the
 * respective mark function. Then, the local vals pointer vector
 * is iterated through and each value is marked if it is not null. 
 */
void mark_eval_stack(PtrStack *evalStack)
{
    int i, j;
    EvaluationContext *current;
    for(i = 0; i < evalStack->size; i++)
    {
        current = pv_get_elem(evalStack, i);
        if(current->current_env != NULL)
        {
            mark_environment(current->current_env);
        }
        if(current->expression != NULL)
        {
            mark_value(current->expression);
        }
        if(current->child_eval_result != NULL)
        {
            mark_value(current->child_eval_result);
        }

        for(j = 0; j < current->local_vals.size; j++)
        {
            Value **ppv = (Value **) pv_get_elem(&current->local_vals, j);
            if(*ppv != NULL)
                mark_value(*ppv);
        }
    }
}

/*
 * This sweeps all the values that were not marked with a value of 1. 
 * This is done by iterating through the allocated values pointer vector
 * and checking the marked value of each value in the vector.
 * If marked, switch the value to 0 for the next collection. Else, 
 * free the value and set the pointer to NULL.
 * After going through all the values, pv_compact was called to get
 * rid of all the NULL pointers in the vector.
 */
void sweep_values()
{
    Value *func;
    int i;
    for(i = 0; i < allocated_values.size; i++)
    {
        func = (Value *) pv_get_elem(&allocated_values, i); 
        if(func->marked == 1)
        {
            func->marked = 0;
        } 
        else
        {   
            free_value(func); 
            pv_set_elem(&allocated_values, i, NULL);   
        }
    }

    pv_compact(&allocated_values);
}

/*
 * This sweeps all the lambdas that were not marked with a value of 1. 
 * This is done by iterating through the allocated lambdas pointer vector
 * and checking the marked value of each lambda in the vector.
 * If marked, switch the value to 0 for the next collection. Else, 
 * free the lambda and set the pointer to NULL.
 * After going through all the lambdas, pv_compact was called to get
 * rid of all the NULL pointers in the vector.
 */
void sweep_lambdas()
{
    Lambda *func;
    int i;
    for(i = 0; i < allocated_lambdas.size; i++)
    {
        func = (Lambda *) pv_get_elem(&allocated_lambdas, i);
        if(func->marked == 1)
        {
            func->marked = 0;
        } 
        else
        {   
            free_lambda(func); 
            pv_set_elem(&allocated_lambdas, i, NULL);   
        }
    }

    pv_compact(&allocated_lambdas);
    
}

/*
 * This sweeps all the environments that were not marked with a value of 1. 
 * This is done by iterating through the allocated environments pointer vector
 * and checking the marked value of each environments in the vector.
 * If marked, switch the value to 0 for the next collection. Else, 
 * free the environment and set the pointer to NULL.
 * After going through all the environments, pv_compact was called to get
 * rid of all the NULL pointers in the vector.
 */
void sweep_environments()
{
    Environment *func;
    int i;
    for(i = 0; i < allocated_environments.size; i++)
    {
        func = (Environment *) pv_get_elem(&allocated_environments, i);
        if(func->marked == 1)
        {
            func->marked = 0;
        }  
        else
        {   
            free_environment(func); 
            pv_set_elem(&allocated_environments, i, NULL);   
        }
    }

    pv_compact(&allocated_environments);
}

/*!
 * This function performs the garbage collection for the Scheme interpreter.
 * It also contains code to track how many objects were collected on each run,
 * and also it can optionally be set to do GC when the total memory used grows
 * beyond a certain limit.
 */
void collect_garbage() {
    Environment *global_env;
    PtrStack *eval_stack;

#ifdef GC_STATS
    int vals_before, procs_before, envs_before;
    int vals_after, procs_after, envs_after;

    vals_before = allocated_values.size;
    procs_before = allocated_lambdas.size;
    envs_before = allocated_environments.size;
#endif

#ifndef ALWAYS_GC
    /* Don't perform garbage collection if we still have room to grow. */
    if (allocation_size() < max_allocation_size)
        return;
#endif


    /* The following is the marking phase of the garbage collection. */
    global_env = get_global_environment();
    eval_stack = get_eval_stack();

    mark_environment(global_env);
    mark_eval_stack(eval_stack);

    /* The following is the sweeping phase of the garbage collection. */
    sweep_values();
    sweep_lambdas();
    sweep_environments();

    



#ifndef ALWAYS_GC
    /* If we are still above the maximum allocation size, increase it. */
    if (allocation_size() > max_allocation_size) {
        max_allocation_size *= 2;

        printf("Increasing maximum allocation size to %ld bytes.\n",
            max_allocation_size);
    }
#endif
    
#ifdef GC_STATS
    vals_after = allocated_values.size;
    procs_after = allocated_lambdas.size;
    envs_after = allocated_environments.size;

    printf("GC Results:\n");
    printf("\tBefore: \t%d vals \t%d lambdas \t%d envs\n",
            vals_before, procs_before, envs_before);
    printf("\tAfter:  \t%d vals \t%d lambdas \t%d envs\n",
            vals_after, procs_after, envs_after);
    printf("\tChange: \t%d vals \t%d lambdas \t%d envs\n",
            vals_after - vals_before, procs_after - procs_before,
            envs_after - envs_before);
#endif
}

