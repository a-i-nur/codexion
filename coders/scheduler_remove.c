/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   scheduler_remove.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:12:45 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:14:00 by aakhmeto         ###   ########.fr       */
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

static void	shift_up(t_simulation *simulation, int index)
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

static void	shift_down(t_simulation *simulation, int index)
{
	t_request_heap	*heap;
	int				child;

	heap = &simulation->request_heap;
	while (index * 2 + 1 < heap->size)
	{
		child = index * 2 + 1;
		if (child + 1 < heap->size && request_has_priority(simulation,
				&heap->requests[child + 1], &heap->requests[child]))
			child++;
		if (!request_has_priority(simulation, &heap->requests[child],
				&heap->requests[index]))
			break ;
		swap_requests(&heap->requests[index], &heap->requests[child]);
		index = child;
	}
}

void	request_heap_remove_coder(t_simulation *simulation, t_coder *coder)
{
	t_request_heap	*heap;
	int				index;
	int				parent;

	heap = &simulation->request_heap;
	index = 0;
	while (index < heap->size && heap->requests[index].coder != coder)
		index++;
	if (index == heap->size)
		return ;
	heap->size--;
	if (index == heap->size)
		return ;
	heap->requests[index] = heap->requests[heap->size];
	parent = (index - 1) / 2;
	if (index > 0 && request_has_priority(simulation,
			&heap->requests[index], &heap->requests[parent]))
		shift_up(simulation, index);
	else
		shift_down(simulation, index);
}
