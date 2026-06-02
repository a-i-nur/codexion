/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:53:37 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/02 16:19:22 by aakhmeto         ###   ########.fr       */
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
	pthread_mutex_init(&simulation.log_mutex, NULL);
	pthread_mutex_init(&simulation.stop_mutex, NULL);
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
}

void	destroy_simulation(t_simulation *simulation)
{
	destroy_coders(simulation);
	pthread_mutex_destroy(&simulation->log_mutex);
	pthread_mutex_destroy(&simulation->stop_mutex);
}
