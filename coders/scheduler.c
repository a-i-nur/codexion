/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/08 14:05:51 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:17:22 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap_requests(t_request *first, t_request *second)
{
	t_request	temporary;

	temporary = *first;
	*first = *second;
	*second = temporary;
}

static int	has_priority(t_simulation *simulation,
	t_request *first, t_request *second)
{
	if (ft_strncmp(simulation->args.scheduler, "edf", 3) == 0)
	{
		if (first->deadline != second->deadline)
			return (first->deadline < second->deadline);
	}
	return (first->arrival_order < second->arrival_order);
}

int	request_heap_insert(t_simulation *simulation, t_coder *coder)
{
	t_request_heap	*heap;
	t_request		request;
	int				index;
	int				parent;

	heap = &simulation->request_heap;
	if (heap->size >= heap->capacity)
		return (1);
	request.coder = coder;
	request.arrival_order = heap->next_arrival_order++;
	request.deadline = get_last_compile_start(coder)
		+ simulation->args.time_to_burnout;
	index = heap->size;
	heap->requests[index] = request;
	heap->size++;
	while (index > 0)
	{
		parent = (index - 1) / 2;
		if (!has_priority(simulation, &heap->requests[index],
				&heap->requests[parent]))
			break ;
		swap_requests(&heap->requests[parent], &heap->requests[index]);
		index = parent;
	}
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

t_coder	*request_heap_remove(t_simulation *simulation)
{
	t_request_heap	*heap;
	t_coder			*winner;
	int				index;
	int				child;

	heap = &simulation->request_heap;
	if (heap->size == 0)
		return (NULL);
	winner = heap->requests[0].coder;
	heap->size--;
	if (heap->size == 0)
		return (winner);
	heap->requests[0] = heap->requests[heap->size];
	index = 0;
	while (index * 2 + 1 < heap->size)
	{
		child = index * 2 + 1;
		if (child + 1 < heap->size
			&& has_priority(simulation, &heap->requests[child + 1],
				&heap->requests[child]))
			child++;
		if (!has_priority(simulation, &heap->requests[child],
				&heap->requests[index]))
			break ;
		swap_requests(&heap->requests[index], &heap->requests[child]);
		index = child;
	}
	return (winner);
}

t_coder	*request_heap_peek(t_simulation *simulation)
{
	if (simulation->request_heap.size == 0)
		return (NULL);
	return (simulation->request_heap.requests[0].coder);
}
