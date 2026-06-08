/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:53:37 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:25:29 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_simulation	new_simulation(t_args args)
{
	t_simulation	simulation;

	simulation.args = args;
	simulation.start_time = get_time_ms();
	simulation.stop = 0;
	simulation.coders = NULL;
	simulation.dongles = NULL;
	simulation.request_heap.size = 0;
	simulation.request_heap.capacity = args.num_of_coders;
	simulation.request_heap.next_arrival_order = 0;
	pthread_mutex_init(&simulation.log_mutex, NULL);
	pthread_mutex_init(&simulation.stop_mutex, NULL);
	pthread_mutex_init(&simulation.scheduler_mutex, NULL);
	pthread_cond_init(&simulation.scheduler_cond, NULL);
	return (simulation);
}

int	simulation_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->stop_mutex);
	stopped = simulation->stop;
	pthread_mutex_unlock(&simulation->stop_mutex);
	return (stopped);
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->stop_mutex);
	simulation->stop = 1;
	pthread_mutex_unlock(&simulation->stop_mutex);
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
