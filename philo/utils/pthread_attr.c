/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pthread_attr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:13 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/05 12:55:59 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"

void    analyze_pthread_attr()
{
	pthread_attr_t attr1;
	pthread_attr_t attr2;
	pthread_attr_init(&attr1);
	pthread_attr_init(&attr2);
	pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
	unsigned char *p1 = (unsigned char *)&attr1;
	unsigned char *p2 = (unsigned char *)&attr2;

	for (int i = 0; i < sizeof(pthread_attr_t); i++)
	{
		if (p1[i] != p2[i])
			printf("Byte %d differs: %02x vs %02x\n", i, p1[i], p2[i]);
	}
}

// Helper function to print all attributes (for debugging)
void ft_pthread_attr_print(const pthread_attr_t *attr)
{
    if (!attr) {
        printf("Invalid attribute pointer\n");
        return;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    
    printf("pthread_attr_t contents:\n");
    printf("  detachstate: %s\n", 
        	internal->detachstate == PTHREAD_CREATE_JOINABLE ? "JOINABLE" : "DETACHED");
    printf("  guardsize: %zu bytes\n", internal->guardsize);
    printf("  stackaddr: %p\n", internal->stackaddr);
    printf("  stacksize: %zu bytes\n", internal->stacksize);
    printf("  scope: %s\n", 
        	internal->scope == PTHREAD_SCOPE_SYSTEM ? "SYSTEM" : "PROCESS");
    printf("  policy: %d\n", internal->policy);
    printf("  priority: %d\n", internal->schedparam.sched_priority);
    printf("  inheritsched: %s\n", 
        	internal->inheritsched == PTHREAD_INHERIT_SCHED ? "INHERIT" : "EXPLICIT");
}


// Function to validate an attribute structure
int ft_pthread_attr_validate(const pthread_attr_t *attr) {
    if (!attr) {
        return EINVAL;
    }
    
    t_pthread_attr  *internal = ATTR_TO_INTERNAL(attr);
    
    // Validate detach state
    if (internal->detachstate != PTHREAD_CREATE_JOINABLE && 
        internal->detachstate != PTHREAD_CREATE_DETACHED) {
        return EINVAL;
    }
    
    // Validate stack size
    if (internal->stacksize < PTHREAD_STACK_MIN) {
        return EINVAL;
    }
    
    // Validate scope
    if (internal->scope != PTHREAD_SCOPE_SYSTEM && 
        internal->scope != PTHREAD_SCOPE_PROCESS) {
        return EINVAL;
    }
    
    // Validate policy
    if (internal->policy != SCHED_OTHER && 
        internal->policy != SCHED_FIFO && 
        internal->policy != SCHED_RR) {
        return EINVAL;
    }
    
    // Validate inheritance
    if (internal->inheritsched != PTHREAD_INHERIT_SCHED && 
        internal->inheritsched != PTHREAD_EXPLICIT_SCHED) {
        return EINVAL;
    }
    
    return 0;
}

//=============================================================================
// INHERITANCE SCHEDULING FUNCTIONS
//=============================================================================

int ft_pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched) {
    if (!attr) {
        return EINVAL;
    }
    
    if (inheritsched != PTHREAD_INHERIT_SCHED && inheritsched != PTHREAD_EXPLICIT_SCHED) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    internal->inheritsched = inheritsched;
    
    return 0;
}

int ft_pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched) {
    if (!attr || !inheritsched) {
        return EINVAL;
    }
    
	t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    *inheritsched = internal->inheritsched;
    
    return 0;
}

//=============================================================================
// SCHEDULING PARAMETER FUNCTIONS
//=============================================================================

int ft_pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param) {
    if (!attr || !param) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    
    // Validate priority based on policy
    int min_prio = sched_get_priority_min(internal->policy);
    int max_prio = sched_get_priority_max(internal->policy);
    
    if (param->sched_priority < min_prio || param->sched_priority > max_prio) {
        return EINVAL;
    }
    
    internal->schedparam = *param;
    
    return 0;
}

int ft_pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param) {
    if (!attr || !param) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    *param = internal->schedparam;
    
    return 0;
}

//=============================================================================
// SCHEDULING POLICY FUNCTIONS
//=============================================================================

int ft_pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy) {
    if (!attr) {
        return EINVAL;
    }
    
    // Validate scheduling policy
    if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    internal->policy = policy;
    
    return 0;
}

int ft_pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy) {
    if (!attr || !policy) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    *policy = internal->policy;
    
    return 0;
}

//=============================================================================
// SCHEDULING SCOPE FUNCTIONS
//=============================================================================

int ft_pthread_attr_setscope(pthread_attr_t *attr, int scope) {
    if (!attr) {
        return EINVAL;
    }
    
    if (scope != PTHREAD_SCOPE_SYSTEM && scope != PTHREAD_SCOPE_PROCESS) {
        return EINVAL;
    }
    
    // Some systems only support PTHREAD_SCOPE_SYSTEM
    if (scope == PTHREAD_SCOPE_PROCESS) {
        return ENOTSUP;  // Not supported on many systems
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    internal->scope = scope;
    
    return 0;
}

int ft_pthread_attr_getscope(const pthread_attr_t *attr, int *scope) {
    if (!attr || !scope) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    *scope = internal->scope;
    
    return 0;
}

/**
 * Pthread attribute
 * helpful for thread creation & Management
 * Mutex synchronization
 * Deadlock prevention
 * Timeout based approach
 * Condition Variables for advanced solutions
 * 
 * Order the resource: use mutex in consistent order
 * Waiter solution: Central coordinator using condition variables
 * Chandy/misra solution:token based approahch with message parsing
 * Semaphore solution: limit concurrent diners
 */

int ft_pthread_attr_init(pthread_attr_t *attr)
{
	if (!attr)
		return (EINVAL);
	memset(attr, 0, sizeof(pthread_attr_t));
	t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
	internal->detachstate = PTHREAD_CREATE_JOINABLE;
	internal->guardsize = DEFAULT_GUARD_SIZE;
	internal->stackaddr = NULL;
	internal->stacksize = DEFAULT_STACK_SIZE;
	internal->scope = PTHREAD_SCOPE_SYSTEM;
	internal->policy = SCHED_OTHER;
	internal->schedparam.sched_priority = 0;
	internal->inheritsched = PTHREAD_INHERIT_SCHED;
	return (0);
}

int	ft_pthread_attr_destroy(pthread_attr_t *attr)
{
	if (!attr)
		return (EINVAL);
	memset(attr, 0, sizeof(pthread_attr_t));
	return (0);
}

int	ft_pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
	if (!attr)
		return (EINVAL);
	if (detachstate != PTHREAD_CREATE_JOINABLE
	&& detachstate != PTHREAD_CREATE_JOINABLE)
		return (EINVAL);
	t_pthread_attr *internal;

	internal = ATTR_TO_INTERNAL(attr);
	internal->detachstate = detachstate;
	return (0);
}

int	ft_pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
	if (!attr)
		return (EINVAL);
	t_pthread_attr *internal;

	internal = ATTR_TO_INTERNAL(attr);
	internal->guardsize = guardsize;
	return (0);
}

int ft_pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *guardsize)
{
	if (!attr || !guardsize)
		return (EINVAL);
	t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
	*guardsize = internal->guardsize;
	return (0);
}

// Check minimum stack size (usually PTHREAD_STACK_MIN)
int ft_pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize) {
    if (!attr)
        return (EINVAL);
    if (stacksize < PTHREAD_STACK_MIN)
        return (EINVAL);
    t_pthread_attr *internal;

	internal = 	ATTR_TO_INTERNAL(attr);
    internal->stacksize = stacksize;
    return (0);
}

int	ft_pthread_attr_getstacksize(const pthread_attr_t *attr, size_t *stacksize)
{
	if (!attr || !stacksize)
		retrun (EINVAL);
	t_pthread_attr *internal;

	internal = ATTR_TO_INTERNAL(attr);
	*stacksize = internal->stacksize;
	return (0);
}

int ft_pthread_attr_setstackaddr(pthread_attr_t *attr, void *stackaddr) {
    if (!attr) {
        return EINVAL;
    }
    
    t_pthread_attr *internal;

	internal = ATTR_TO_INTERNAL(attr);
    internal->stackaddr = stackaddr;
    
    return 0;
}

int ft_pthread_attr_getstackaddr(const pthread_attr_t *attr, void **stackaddr) {
    if (!attr || !stackaddr) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    *stackaddr = internal->stackaddr;
    
    return 0;
}

int ft_pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize) {
    if (!attr) {
        return EINVAL;
    }
    
    if (stacksize < PTHREAD_STACK_MIN) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    internal->stackaddr = stackaddr;
    internal->stacksize = stacksize;
    
    return 0;
}

int ft_pthread_attr_getstack(const pthread_attr_t *attr, void **stackaddr, size_t *stacksize) {
    if (!attr || !stackaddr || !stacksize) {
        return EINVAL;
    }
    
    t_pthread_attr *internal = ATTR_TO_INTERNAL(attr);
    *stackaddr = internal->stackaddr;
    *stacksize = internal->stacksize;
    
    return 0;
}

