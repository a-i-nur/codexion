/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_priority.c                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:12:27 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:14:15 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	request_has_priority(t_simulation *simulation,
	t_request *first, t_request *second)
{
	if (ft_strncmp(simulation->args.scheduler, "edf", 3) == 0
		&& first->deadline != second->deadline)
		return (first->deadline < second->deadline);
	return (first->arrival_order < second->arrival_order);
}

/* Returns 1 if two requests need at least one same dongle. */
static int	requests_conflict(t_request *first, t_request *second)
{
	t_coder	*a;
	t_coder	*b;

	a = first->coder;
	b = second->coder;
	return (a->left_dongle == b->left_dongle
		|| a->left_dongle == b->right_dongle
		|| a->right_dongle == b->left_dongle
		|| a->right_dongle == b->right_dongle);
}

/* Returns the index of the best unconsidered request, or -1 if none exists. */
static int	find_best_request(t_simulation *simulation)
{
	t_request_heap	*heap;
	int				best;
	int				i;

	heap = &simulation->request_heap;
	best = -1;
	i = 0;
	while (i < heap->size)
	{
		if (!heap->requests[i].considered && (best == -1
				|| request_has_priority(simulation, &heap->requests[i],
					&heap->requests[best])))
			best = i;
		i++;
	}
	return (best);
}

/* Returns 1 if the request does not conflict with already selected requests. */
static int	can_select_request(t_request_heap *heap, int best)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->requests[i].selected
			&& requests_conflict(&heap->requests[best], &heap->requests[i]))
			return (0);
		i++;
	}
	return (1);
}

int	request_heap_coder_has_priority(t_simulation *simulation,
	t_coder *coder)
{
	t_request_heap	*heap;
	int				best;
	int				i;

	heap = &simulation->request_heap;
	i = -1;
	while (++i < heap->size)
	{
		heap->requests[i].considered = 0;
		heap->requests[i].selected = 0;
	}
	i = -1;
	while (++i < heap->size)
	{
		best = find_best_request(simulation);
		heap->requests[best].considered = 1;
		heap->requests[best].selected = can_select_request(heap, best);
	}
	i = -1;
	while (++i < heap->size)
		if (heap->requests[i].coder == coder)
			return (heap->requests[i].selected);
	return (0);
}
