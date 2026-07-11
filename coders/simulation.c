/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:53:37 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:13:32 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/* Fills simulation fields with initial values. */
static void	init_simulation_values(t_simulation *simulation, t_args args)
{
	simulation->args = args;
	simulation->start_time = get_time_ms();
	simulation->stop = 0;
	simulation->ready = 0;
	simulation->started_threads = 0;
	simulation->coders = NULL;
	simulation->dongles = NULL;
	simulation->request_heap.requests = NULL;
	simulation->request_heap.size = 0;
	simulation->request_heap.capacity = args.num_of_coders;
	simulation->request_heap.next_arrival_order = 0;
}

/* Initializes sync objects. Returns 0 on success, 1 on error. */
static int	init_simulation_sync(t_simulation *simulation)
{
	if (pthread_mutex_init(&simulation->log_mutex, NULL) != 0)
		return (1);
	if (pthread_mutex_init(&simulation->stop_mutex, NULL) != 0)
		return (pthread_mutex_destroy(&simulation->log_mutex), 1);
	if (pthread_mutex_init(&simulation->scheduler_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->stop_mutex);
		pthread_mutex_destroy(&simulation->log_mutex);
		return (1);
	}
	if (pthread_cond_init(&simulation->scheduler_cond, NULL) != 0)
	{
		pthread_mutex_destroy(&simulation->scheduler_mutex);
		pthread_mutex_destroy(&simulation->stop_mutex);
		pthread_mutex_destroy(&simulation->log_mutex);
		return (1);
	}
	return (0);
}

int	init_simulation(t_simulation *simulation, t_args args)
{
	init_simulation_values(simulation, args);
	return (init_simulation_sync(simulation));
}

void	destroy_simulation(t_simulation *simulation)
{
	destroy_request_heap(simulation);
	destroy_dongles(simulation);
	destroy_coders(simulation);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->stop_mutex);
	pthread_cond_destroy(&simulation->scheduler_cond);
	pthread_mutex_destroy(&simulation->scheduler_mutex);
}
