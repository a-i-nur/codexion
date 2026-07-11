/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 14:05:51 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 16:00:00 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Swaps two heap requests. */
static void	swap_requests(t_request *first, t_request *second)
{
	t_request	temporary;

	temporary = *first;
	*first = *second;
	*second = temporary;
}

/* Moves a request up until the heap priority order is restored. */
static void	shift_request_up(t_simulation *simulation, int index)
{
	t_request_heap	*heap;
	int				parent;

	heap = &simulation->request_heap;
	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!request_has_priority(simulation, &heap->requests[index],
				&heap->requests[parent]))
			break ;
		swap_requests(&heap->requests[index], &heap->requests[parent]);
		index = parent;
	}
}

int	request_heap_insert(t_simulation *simulation, t_coder *coder)
{
	t_request_heap	*heap;
	t_request		*request;
	int				index;

	heap = &simulation->request_heap;
	if (heap->size >= heap->capacity)
		return (1);
	index = heap->size++;
	request = &heap->requests[index];
	request->coder = coder;
	request->arrival_order = heap->next_arrival_order++;
	request->deadline = get_last_compile_start(coder)
		+ simulation->args.time_to_burnout;
	request->considered = 0;
	request->selected = 0;
	shift_request_up(simulation, index);
	return (0);
}

int	init_request_heap(t_simulation *simulation)
{
	t_request_heap	*heap;

	heap = &simulation->request_heap;
	heap->requests = malloc(sizeof(t_request) * heap->capacity);
	if (heap->requests == NULL)
		return (1);
	return (0);
}

void	destroy_request_heap(t_simulation *simulation)
{
	free(simulation->request_heap.requests);
	simulation->request_heap.requests = NULL;
	simulation->request_heap.size = 0;
}
