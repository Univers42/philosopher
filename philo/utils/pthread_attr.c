/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pthread_attr.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:13 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/06 19:59:04 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosopher.h"
#include <stdlib.h>

#ifndef PTHREAD_STACK_MIN
# define PTHREAD_STACK_MIN 2000
#endif

// Internal registry: map pthread_attr_t* -> t_pthread_attr (stored separately)
typedef struct s_attr_node {
	pthread_attr_t			*key;
	t_pthread_attr			val;
	struct s_attr_node		*next;
}	t_attr_node;

// Replace global list with a singleton accessor
static t_attr_node	**attr_list(void)
{
	static t_attr_node	*head = NULL;

	return (&head);
}

static t_pthread_attr *attr_find(const pthread_attr_t *key)
{
	t_attr_node *n = *attr_list();

	while (n)
	{
		if (n->key == key)
			return (&n->val);
		n = n->next;
	}
	return (NULL);
}

static void	attr_set_defaults(t_pthread_attr *a)
{
	// Default values aligned with your header constants
	a->detachstate = PTHREAD_CREATE_JOINABLE;
	a->guardsize = DEFAULT_GUARD_SIZE;
	a->stackaddr = NULL;
	a->stacksize = DEFAULT_STACK_SIZE;
	a->scope = PTHREAD_SCOPE_SYSTEM;
	a->policy = SCHED_OTHER;
	a->schedparam.sched_priority = 0;
	a->inheritsched = PTHREAD_INHERIT_SCHED;
}

static t_pthread_attr *attr_ensure(pthread_attr_t *key)
{
	t_attr_node *n;
	t_attr_node **headp;

	n = (t_attr_node *)malloc(sizeof(*n));
	if (!n)
		return (NULL);
	n->key = key;
	attr_set_defaults(&n->val);
	headp = attr_list();
	n->next = *headp;
	*headp = n;
	return (&n->val);
}

static int	attr_remove(pthread_attr_t *key)
{
	t_attr_node **pp;
	t_attr_node *cur;

	pp = attr_list();
	cur = *pp;
	while (cur)
	{
		if (cur->key == key)
		{
			*pp = cur->next;
			free(cur);
			return (0);
		}
		pp = &cur->next;
		cur = cur->next;
	}
	return (EINVAL);
}

void    analyze_pthread_attr(void)
{
	// Not supported without real pthread_attr_* access; keep as a stub util.
	printf("analyze_pthread_attr: disabled (no direct pthread_attr_* usage allowed)\n");
}

// Helper function to print all attributes (for debugging)
void ft_pthread_attr_print(const pthread_attr_t *attr)
{
	t_pthread_attr	*internal;

	if (!attr)
	{
		printf("Invalid attribute pointer\n");
		return ;
	}
	internal = attr_find(attr);
	if (!internal)
	{
		printf("Attribute not initialized\n");
		return ;
	}
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
int ft_pthread_attr_validate(const pthread_attr_t *attr)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	if (internal->detachstate != PTHREAD_CREATE_JOINABLE && 
		internal->detachstate != PTHREAD_CREATE_DETACHED)
		return (EINVAL);
	if (internal->stacksize < PTHREAD_STACK_MIN)
		return (EINVAL);
	if (internal->scope != PTHREAD_SCOPE_SYSTEM && 
		internal->scope != PTHREAD_SCOPE_PROCESS)
		return (EINVAL);
	if (internal->policy != SCHED_OTHER && 
		internal->policy != SCHED_FIFO && 
		internal->policy != SCHED_RR)
		return (EINVAL);
	if (internal->inheritsched != PTHREAD_INHERIT_SCHED && 
		internal->inheritsched != PTHREAD_EXPLICIT_SCHED)
		return (EINVAL);
	return (0);
}

//=============================================================================
// INHERITANCE SCHEDULING FUNCTIONS
//=============================================================================

int ft_pthread_attr_setinheritsched(pthread_attr_t *attr, int inheritsched)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	if (inheritsched != PTHREAD_INHERIT_SCHED && inheritsched != PTHREAD_EXPLICIT_SCHED)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->inheritsched = inheritsched;
	return (0);
}

int ft_pthread_attr_getinheritsched(const pthread_attr_t *attr, int *inheritsched)
{
	t_pthread_attr	*internal;

	if (!attr || !inheritsched)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*inheritsched = internal->inheritsched;
	return (0);
}

//=============================================================================
// SCHEDULING PARAMETER FUNCTIONS
//=============================================================================

int ft_pthread_attr_setschedparam(pthread_attr_t *attr, const struct sched_param *param)
{
	t_pthread_attr	*internal;
	int				min_prio;
	int				max_prio;

	if (!attr || !param)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	min_prio = sched_get_priority_min(internal->policy);
	max_prio = sched_get_priority_max(internal->policy);
	if (param->sched_priority < min_prio || param->sched_priority > max_prio)
		return (EINVAL);
	internal->schedparam = *param;
	return (0);
}

int ft_pthread_attr_getschedparam(const pthread_attr_t *attr, struct sched_param *param)
{
	t_pthread_attr	*internal;

	if (!attr || !param)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*param = internal->schedparam;
	return (0);
}

//=============================================================================
// SCHEDULING POLICY FUNCTIONS
//=============================================================================

int ft_pthread_attr_setschedpolicy(pthread_attr_t *attr, int policy)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	if (policy != SCHED_OTHER && policy != SCHED_FIFO && policy != SCHED_RR)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->policy = policy;
	return (0);
}

int ft_pthread_attr_getschedpolicy(const pthread_attr_t *attr, int *policy)
{
	t_pthread_attr	*internal;

	if (!attr || !policy)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*policy = internal->policy;
	return (0);
}

//=============================================================================
// SCHEDULING SCOPE FUNCTIONS
//=============================================================================

int ft_pthread_attr_setscope(pthread_attr_t *attr, int scope)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	if (scope != PTHREAD_SCOPE_SYSTEM && scope != PTHREAD_SCOPE_PROCESS)
		return (EINVAL);
	if (scope == PTHREAD_SCOPE_PROCESS)
		return (ENOTSUP);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->scope = scope;
	return (0);
}

int ft_pthread_attr_getscope(const pthread_attr_t *attr, int *scope)
{
	t_pthread_attr	*internal;

	if (!attr || !scope)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*scope = internal->scope;
	return (0);
}

/**
 * Pthread attribute utils (no direct use of pthread_attr_*).
 */
int ft_pthread_attr_init(pthread_attr_t *attr)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	// Reset to defaults each init call.
	attr_set_defaults(internal);
	return (0);
}

int	ft_pthread_attr_destroy(pthread_attr_t *attr)
{
	if (!attr)
		return (EINVAL);
	return (attr_remove(attr));
}

int	ft_pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	if (detachstate != PTHREAD_CREATE_JOINABLE
		&& detachstate != PTHREAD_CREATE_DETACHED)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->detachstate = detachstate;
	return (0);
}

int ft_pthread_attr_getdetachstate(const pthread_attr_t *attr, int *detachstate)
{
	t_pthread_attr	*internal;

	if (!attr || !detachstate)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*detachstate = internal->detachstate;
	return (0);
}

int	ft_pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->guardsize = guardsize;
	return (0);
}

int ft_pthread_attr_getguardsize(const pthread_attr_t *attr,
									size_t *guardsize)
{
	t_pthread_attr	*internal;

	if (!attr || !guardsize)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*guardsize = internal->guardsize;
	return (0);
}

// Check minimum stack size (usually PTHREAD_STACK_MIN)
int ft_pthread_attr_setstacksize(pthread_attr_t *attr,
									size_t stacksize)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	if (stacksize < PTHREAD_STACK_MIN)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->stacksize = stacksize;
	return (0);
}

int	ft_pthread_attr_getstacksize(const pthread_attr_t *attr,
									size_t *stacksize)
{
	t_pthread_attr	*internal;

	if (!attr || !stacksize)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*stacksize = internal->stacksize;
	return (0);
}

int ft_pthread_attr_setstackaddr(pthread_attr_t *attr,
									void *stackaddr)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->stackaddr = stackaddr;
	return (0);
}

int ft_pthread_attr_getstackaddr(const pthread_attr_t *attr,
									void **stackaddr)
{
	t_pthread_attr	*internal;

	if (!attr || !stackaddr)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*stackaddr = internal->stackaddr;
	return (0);
}

int ft_pthread_attr_setstack(pthread_attr_t *attr,
								void *stackaddr,
								size_t stacksize)
{
	t_pthread_attr	*internal;

	if (!attr)
		return (EINVAL);
	if (stacksize < PTHREAD_STACK_MIN)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		internal = attr_ensure(attr);
	if (!internal)
		return (ENOMEM);
	internal->stackaddr = stackaddr;
	internal->stacksize = stacksize;
	return (0);
}

int	ft_pthread_attr_getstack(const pthread_attr_t *attr,
								void **stackaddr,
								size_t *stacksize)
{
	t_pthread_attr	*internal;

	if (!attr || !stackaddr || !stacksize)
		return (EINVAL);
	internal = attr_find(attr);
	if (!internal)
		return (EINVAL);
	*stackaddr = internal->stackaddr;
	*stacksize = internal->stacksize;
	return (0);
}
